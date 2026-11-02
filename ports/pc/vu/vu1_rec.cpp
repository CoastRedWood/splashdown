// The VU1 recompiler: each microprogram is translated, a basic block at a
// time, into threaded code (an array of pre-decoded operations, each a
// handler specialised by op, field mask and operand form, chained by tail
// calls), with the interpreter's timing model (vu1.cpp's header) worked out
// at translation time.
//
// A block starts at a PC with a known pipeline state (Sig: which VF fields
// and VI registers land in 1-3 cycles, how far Q and P are from landing, the
// flag updates in flight) and runs to a branch's delay slot, an [E]'s
// following instruction or 256 instructions. Within it every stall, every Q,
// P and flag landing is static: the translator replays the interpreter's
// Stall/Lands/SetQ/SetP/PushFlag on a Timing of its own and emits a landing
// operation where the interpreter would see the new value. The state at the
// block's end is static too, so a block's exits link straight to the
// successor compiled for that state. Values in flight (Q, P, flag updates)
// live in R.qNext/R.pNext/R.fl[4].
//
// MAC flags are computed only where something can see them: by an FMAND/
// FMEQ/FMOR (no program reads status: no FS*), or by being the last landed
// or still in flight at the block's end. Dropping the others keeps status's
// sticky Z/S bits right once both are set (they never clear: no FSSET), so a
// program runs translated only from then on (the interpreter before).
//
// The upper half runs before the lower one unless the lower reads or writes
// the upper's destination; then lower first if the upper doesn't read the
// lower's, else the upper's result goes through R.tmp (the interpreter's
// order: both read, then the lower writes, then the upper).
//
// Blocks are kept per PC and entry state, with the words they were made
// from: when an MPG changes micro memory (vu1_micro_changed) each block is
// compared with memory again at its next use, so the programs that share
// load addresses keep their translations.
//
// Branch with a branch or an [E] in its delay slot: the interpreter runs
// it (Interp until the next clean boundary), then translation resumes.
#include "vu1_internal.h"

#include "port.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace vu1i {

namespace {

struct Op;
typedef void (__attribute__((preserve_none)) *Fn)(const Op *);
struct Op {
    Fn fn;
    uint16_t a, b, c;
    uint8_t m, k;
};
static_assert(sizeof(Op) == 16);

#define HANDLER(name) __attribute__((preserve_none)) void name(const Op *op)
#define NEXT() [[clang::musttail]] return op[1].fn(op + 1)

inline char *Base()
{
    return (char *)&R;
}
inline float *F(uint16_t off)
{
    return (float *)(Base() + off);
}
inline uint16_t OffVf(int r)
{
    return (uint16_t)offsetof(Regs, vf) + 16 * r;
}
inline uint16_t OffVfField(int r, int k)
{
    return OffVf(r) + 4 * k;
}
const uint16_t kOffAcc = offsetof(Regs, acc), kOffTmp = offsetof(Regs, tmp), kOffSink = offsetof(Regs, sink);
const uint16_t kOffQ = offsetof(Regs, q), kOffI = offsetof(Regs, i);

inline void Put(float *d, __m128 v, uint32_t dest)
{
    __m128 m = _mm_castsi128_ps(kLaneMask[dest]);
    _mm_storeu_ps(d, _mm_or_ps(_mm_and_ps(m, v), _mm_andnot_ps(m, _mm_loadu_ps(d))));
}

template <bool FULL>
inline void PutF(float *d, __m128 v, uint32_t dest)
{
    if (FULL)
        _mm_storeu_ps(d, v);
    else
        Put(d, v, dest);
}

// ---- the upper half

enum { C_ADD, C_SUB, C_MUL, C_MADD, C_MSUB };

// a: destination (vf, acc, tmp or sink), b: fs, c: ft (VEC) or the
// broadcast scalar (a field, Q or I), m: dest, k: flag slot (MAC)
template <int CLS, bool VEC, bool FULL, bool MAC>
HANDLER(UpFmac)
{
    __m128 S = _mm_loadu_ps(F(op->b));
    __m128 T = VEC ? _mm_loadu_ps(F(op->c)) : _mm_set1_ps(*F(op->c));
    __m128 r;
    if (CLS == C_ADD)
        r = _mm_add_ps(S, T);
    else if (CLS == C_SUB)
        r = _mm_sub_ps(S, T);
    else if (CLS == C_MUL)
        r = _mm_mul_ps(S, T);
    else if (CLS == C_MADD)
        r = _mm_add_ps(_mm_loadu_ps(R.acc.f), _mm_mul_ps(S, T));
    else
        r = _mm_sub_ps(_mm_loadu_ps(R.acc.f), _mm_mul_ps(S, T));
    r = FixFast(r);
    if (MAC)
        R.fl[op->k] = MacOf(r, op->m);
    PutF<FULL>(F(op->a), r, op->m);
    NEXT();
}

template <int CLS, bool VEC, bool FULL, bool MAC>
constexpr Fn UpFmacFn()
{
    return UpFmac<CLS, VEC, FULL, MAC>;
}

#define UPROW(C, V) {{UpFmacFn<C, V, false, false>(), UpFmacFn<C, V, false, true>()}, \
                     {UpFmacFn<C, V, true, false>(), UpFmacFn<C, V, true, true>()}}
const Fn kUpFmac[5][2][2][2] = {
    {UPROW(C_ADD, false), UPROW(C_ADD, true)},   {UPROW(C_SUB, false), UPROW(C_SUB, true)},
    {UPROW(C_MUL, false), UPROW(C_MUL, true)},   {UPROW(C_MADD, false), UPROW(C_MADD, true)},
    {UPROW(C_MSUB, false), UPROW(C_MSUB, true)},
};
#undef UPROW

// MAX/MINI as clang compiles the interpreter's std::fmax(s, t)/fmin(s, t):
// t if s is NaN, else (t > s ? t : s) (maxss with t as its destination),
// then Fix; no flags. a: destination, b: fs, c: ft (VEC) or the scalar
template <bool MAX, bool VEC, bool FULL>
HANDLER(UpMinMax)
{
    __m128 S = _mm_loadu_ps(F(op->b));
    __m128 T = VEC ? _mm_loadu_ps(F(op->c)) : _mm_set1_ps(*F(op->c));
    __m128 mm = MAX ? _mm_max_ps(T, S) : _mm_min_ps(T, S);
    __m128 nan = _mm_cmpunord_ps(S, S);
    __m128 r = FixFast(_mm_or_ps(_mm_and_ps(nan, T), _mm_andnot_ps(nan, mm)));
    PutF<FULL>(F(op->a), r, op->m);
    NEXT();
}

// FTOI0/4/12/15: (int)(s * scale) truncated, +2^31 and up saturating to
// 0x7FFFFFFF (the interpreter's double product is exact: scale is a power of
// two; cvttps gives 0x80000000 for the rest out of range and NaN, as
// cvttsd2si does). a: ft, b: fs, m: dest
template <int SC>
HANDLER(UpFtoi)
{
    const __m128 sc = _mm_set1_ps((float)(1 << SC));
    __m128 v = _mm_mul_ps(_mm_loadu_ps(F(op->b)), sc);
    __m128i i = _mm_cvttps_epi32(v);
    __m128i big = _mm_castps_si128(_mm_cmpge_ps(v, _mm_set1_ps(2147483648.0f)));
    i = _mm_or_si128(_mm_andnot_si128(big, i), _mm_and_si128(big, _mm_set1_epi32(0x7FFFFFFF)));
    Put(F(op->a), _mm_castsi128_ps(i), op->m);
    NEXT();
}

template <int SC>
HANDLER(UpItof)
{
    const __m128 sc = _mm_set1_ps((float)(1 << SC));
    __m128 v = _mm_div_ps(_mm_cvtepi32_ps(_mm_castps_si128(_mm_loadu_ps(F(op->b)))), sc);
    Put(F(op->a), v, op->m);
    NEXT();
}

HANDLER(UpAbs)
{
    __m128 v = _mm_and_ps(_mm_loadu_ps(F(op->b)), _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF)));
    Put(F(op->a), v, op->m);
    NEXT();
}

// OPMULA/OPMSUB: (s.y t.z, s.z t.x, s.x t.y), w 0
template <bool SUB, bool MAC>
HANDLER(UpOpm)
{
    __m128 S = _mm_loadu_ps(F(op->b)), T = _mm_loadu_ps(F(op->c));
    __m128 c = _mm_mul_ps(_mm_shuffle_ps(S, S, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(T, T, _MM_SHUFFLE(3, 1, 0, 2)));
    __m128 r = SUB ? _mm_sub_ps(_mm_loadu_ps(R.acc.f), c) : c;
    r = _mm_and_ps(r, _mm_castsi128_ps(_mm_set_epi32(0, -1, -1, -1)));
    r = FixFast(r);
    if (MAC)
        R.fl[op->k] = MacOf(r, op->m);
    Put(F(op->a), r, op->m);
    NEXT();
}

// CLIP: a = fs, b = ft's w, k = flag slot
HANDLER(UpClip)
{
    const float *s = F(op->a);
    float wv = std::fabs(*F(op->b));
    uint32_t c = 0;
    if (s[0] > wv) c |= 1;
    if (s[0] < -wv) c |= 2;
    if (s[1] > wv) c |= 4;
    if (s[1] < -wv) c |= 8;
    if (s[2] > wv) c |= 16;
    if (s[2] < -wv) c |= 32;
    R.fl[op->k] = c;
    NEXT();
}

// Any other upper op, by Upper: the word in a/b, c: destination override
// (0: Upper's own), k: flag slot (0xFF: not wanted)
HANDLER(UpGeneric)
{
    uint32_t w = op->a | (uint32_t)op->b << 16;
    Pending out;
    Upper(w, out);
    if (out.flagKind >= 0 && op->k != 0xFF)
        R.fl[op->k] = out.flagV;
    if (out.reg >= 0) {
        if (op->c)
            _mm_storeu_ps(F(op->c), _mm_loadu_ps(out.v.f));
        else if (out.reg != 0)
            Put(out.reg == 32 ? R.acc.f : R.vf[out.reg].f, _mm_loadu_ps(out.v.f), out.dest & 15);
    }
    NEXT();
}

// the upper result kept in R.tmp, to a with dest m
HANDLER(TmpCommit)
{
    Put(F(op->a), _mm_loadu_ps(R.tmp.f), op->m);
    NEXT();
}

// ---- landing

HANDLER(LandQ)
{
    R.q = R.qNext;
    NEXT();
}
HANDLER(LandP)
{
    R.p = R.pNext;
    NEXT();
}
HANDLER(LandMacOp)
{
    LandMac(R.fl[op->k]);
    NEXT();
}
HANDLER(LandClipOp)
{
    LandClip(R.fl[op->k]);
    NEXT();
}

// ---- the lower half

// LQ/SQ: a = vf, b = vi, c = imm
template <bool FULL>
HANDLER(LowLq)
{
    __m128 v = _mm_loadu_ps((const float *)Mem((uint32_t)(Vi(op->b) + (int16_t)op->c)));
    PutF<FULL>(F(op->a), v, op->m);
    NEXT();
}
template <bool FULL>
HANDLER(LowSq)
{
    float *p = (float *)Mem((uint32_t)(Vi(op->b) + (int16_t)op->c));
    PutF<FULL>(p, _mm_loadu_ps(F(op->a)), op->m);
    NEXT();
}
// LQI/SQI/LQD/SQD: a = vf, b = vi (k: 1 if it is VI0, which stays)
template <bool FULL>
HANDLER(LowLqi)
{
    __m128 v = _mm_loadu_ps((const float *)Mem(R.vi[op->b]));
    PutF<FULL>(F(op->a), v, op->m);
    R.vi[op->b] += op->k ? 0 : 1;
    NEXT();
}
template <bool FULL>
HANDLER(LowSqi)
{
    float *p = (float *)Mem(R.vi[op->b]);
    PutF<FULL>(p, _mm_loadu_ps(F(op->a)), op->m);
    R.vi[op->b] += op->k ? 0 : 1;
    NEXT();
}
template <bool FULL>
HANDLER(LowLqd)
{
    R.vi[op->b] -= op->k ? 0 : 1;
    __m128 v = _mm_loadu_ps((const float *)Mem(R.vi[op->b]));
    PutF<FULL>(F(op->a), v, op->m);
    NEXT();
}
template <bool FULL>
HANDLER(LowSqd)
{
    R.vi[op->b] -= op->k ? 0 : 1;
    float *p = (float *)Mem(R.vi[op->b]);
    PutF<FULL>(p, _mm_loadu_ps(F(op->a)), op->m);
    NEXT();
}
// a = vi (not 0), b = vi, c = imm or vi
HANDLER(LowIaddiu)
{
    R.vi[op->a] = (uint16_t)(R.vi[op->b] + op->c);
    NEXT();
}
HANDLER(LowIsubiu)
{
    R.vi[op->a] = (uint16_t)(R.vi[op->b] - op->c);
    NEXT();
}
HANDLER(LowIaddi)
{
    R.vi[op->a] = (uint16_t)(Vi(op->b) + (int16_t)op->c);
    NEXT();
}
HANDLER(LowIadd)
{
    R.vi[op->a] = (uint16_t)(Vi(op->b) + Vi(op->c));
    NEXT();
}
HANDLER(LowIsub)
{
    R.vi[op->a] = (uint16_t)(Vi(op->b) - Vi(op->c));
    NEXT();
}
HANDLER(LowIand)
{
    R.vi[op->a] = R.vi[op->b] & R.vi[op->c];
    NEXT();
}
HANDLER(LowIor)
{
    R.vi[op->a] = R.vi[op->b] | R.vi[op->c];
    NEXT();
}
// MOVE/MR32: a = vf (not 0), b = vf
template <bool FULL>
HANDLER(LowMove)
{
    PutF<FULL>(F(op->a), _mm_loadu_ps(F(op->b)), op->m);
    NEXT();
}
HANDLER(LowMr32)
{
    __m128 v = _mm_loadu_ps(F(op->b));
    Put(F(op->a), _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 3, 2, 1)), op->m);
    NEXT();
}
// MFIR: a = vf, b = vi
HANDLER(LowMfir)
{
    Put(F(op->a), _mm_castsi128_ps(_mm_set1_epi32((int32_t)Vi(op->b))), op->m);
    NEXT();
}
// MTIR: a = vi (not 0), b = the field
HANDLER(LowMtir)
{
    R.vi[op->a] = (uint16_t)AsU(*F(op->b));
    NEXT();
}
// MFP: a = vf
HANDLER(LowMfp)
{
    Put(F(op->a), _mm_set1_ps(R.p), op->m);
    NEXT();
}
// ILW: a = vi (not 0), b = vi, c = imm, k = the field; ILWR: c = 0, unsigned
HANDLER(LowIlw)
{
    R.vi[op->a] = (uint16_t) * (uint32_t *)(Mem((uint32_t)(Vi(op->b) + (int16_t)op->c)) + op->k * 4);
    NEXT();
}
HANDLER(LowIlwr)
{
    R.vi[op->a] = (uint16_t) * (uint32_t *)(Mem(R.vi[op->b]) + op->k * 4);
    NEXT();
}
// ISW: a = vi (the value), b = vi (the address), c = imm, m = dest; ISWR: c = 0
HANDLER(LowIsw)
{
    uint32_t *p = (uint32_t *)Mem((uint32_t)(Vi(op->b) + (int16_t)op->c));
    uint32_t v = R.vi[op->a];
    for (int k = 0; k < 4; k++)
        if (Has(op->m, k))
            p[k] = v;
    NEXT();
}
HANDLER(LowIswr)
{
    uint32_t *p = (uint32_t *)Mem(R.vi[op->b]);
    uint32_t v = R.vi[op->a];
    for (int k = 0; k < 4; k++)
        if (Has(op->m, k))
            p[k] = v;
    NEXT();
}
// DIV/SQRT/RSQRT and the EFU ops: the word in a/b
HANDLER(LowQ)
{
    R.qNext = QValue(op->a | (uint32_t)op->b << 16);
    NEXT();
}
// DIV with the fields' offsets in a and b (QValue's 0x38 inline)
HANDLER(LowDiv)
{
    float a = *F(op->a), b = *F(op->b);
    R.qNext = b == 0 ? (std::signbit(a) != std::signbit(b) ? -3.4028235e38f : 3.4028235e38f) : Fix(a / b);
    NEXT();
}
HANDLER(LowP)
{
    R.pNext = PValue(op->a | (uint32_t)op->b << 16);
    NEXT();
}
HANDLER(LowXgkick)
{
    Kick(R.vi[op->a]);
    NEXT();
}
HANDLER(LowFcset)
{
    R.clip = op->a | (uint32_t)op->b << 16;
    NEXT();
}
// anything else by Lower: the word in a/b, c = pc
HANDLER(LowGeneric)
{
    bool stop = false;
    Lower(op->a | (uint32_t)op->b << 16, op->c, stop);
    NEXT();
}
HANDLER(SetI)
{
    R.i = AsF(op->a | (uint32_t)op->b << 16);
    NEXT();
}

// branches: the condition into R.br (read at the block's exit, after the
// delay slot), a link register, a jump's target into R.jt
// a = it, b = is
HANDLER(BrAlways)
{
    R.br = 1;
    NEXT();
}
// BAL/JALR: a = link vi (0: none), c = the link value
HANDLER(BrBal)
{
    R.br = 1;
    if (op->a)
        R.vi[op->a] = op->c;
    NEXT();
}
HANDLER(BrJr)
{
    R.br = 1;
    R.jt = R.vi[op->b];
    NEXT();
}
HANDLER(BrJalr)
{
    R.br = 1;
    R.jt = R.vi[op->b];
    if (op->a)
        R.vi[op->a] = op->c;
    NEXT();
}
HANDLER(BrEq)
{
    R.br = Vi(op->a) == Vi(op->b);
    NEXT();
}
HANDLER(BrNe)
{
    R.br = Vi(op->a) != Vi(op->b);
    NEXT();
}
HANDLER(BrLtz)
{
    R.br = Vi(op->b) < 0;
    NEXT();
}
HANDLER(BrGtz)
{
    R.br = Vi(op->b) > 0;
    NEXT();
}
HANDLER(BrLez)
{
    R.br = Vi(op->b) <= 0;
    NEXT();
}
HANDLER(BrGez)
{
    R.br = Vi(op->b) >= 0;
    NEXT();
}

HANDLER(Ret)
{
    (void)op;
}

// ---- blocks

// The pipeline at a block boundary, relative to the next issue cycle.
struct Sig {
    uint32_t vf[3][4];  // [d - 1][field]: VF registers whose field lands in d cycles
    uint16_t vi[3];     // VI registers that land in d cycles
    uint8_t q, p;       // cycles until Q/P land (0: none in flight)
    uint8_t nfl;        // flag updates in flight
    uint8_t fl[3];      // each: kind | slot << 1 | delta << 3
    uint16_t pad;
    bool operator==(const Sig &o) const { return !memcmp(this, &o, sizeof *this); }
};
static_assert(sizeof(Sig) == 64);

uint64_t Hash(const Sig &s)
{
    const uint64_t *w = (const uint64_t *)&s;
    uint64_t h = 0;
    for (int k = 0; k < 8; k++)
        h = (h ^ w[k]) * 0x9E3779B97F4A7C15ull;
    return h ^ (h >> 31);
}

enum { X_FALL, X_COND, X_JUMP, X_END, X_ENDBR, X_ENDJUMP, X_INTERP };

struct Block {
    uint16_t pc, n;  // n instructions
    uint32_t gen;
    uint64_t hash;
    Sig entry;
    std::vector<uint64_t> words;  // what it was made from (pc ...)
    std::vector<Op> ops;
    uint8_t exit;
    uint16_t target, fall;
    Sig out;
    uint64_t outHash;
    Block *link[2];  // the successor at fall, at target (or the last jump's)
};

std::vector<Block *> g_blocks[2048];
uint32_t g_gen = 1;

struct Stats {
    uint64_t blocks, ops, instrs, verify, lookups, interp, interpRuns, stickyRuns;
} g_stats;

inline bool IsBranch(uint64_t ins)
{
    uint32_t hi = (uint32_t)(ins >> 32), lo = (uint32_t)ins;
    if (hi & 0x80000000u)
        return false;
    uint32_t op = lo >> 25;
    return op == 0x20 || op == 0x21 || op == 0x24 || op == 0x25 || op == 0x28 || op == 0x29 ||
           (op >= 0x2C && op <= 0x2F);
}

// The VF registers a decoded half reads (any field).
uint32_t VfReads(const Dec &d)
{
    uint32_t m = 0;
    for (int i = 0; i < d.nvf; i++)
        m |= 1u << d.vfReg[i];
    return m;
}

// A flag update in flight at translation time.
struct FlagEnt {
    uint64_t ready;
    uint8_t kind, slot;
    int upOp;     // the op that computes it (-1: an earlier block's)
    int landOp;   // its landing op (-1: still in flight at the end)
};

struct Compiler {
    Block *b;
    Timing T;
    std::vector<FlagEnt> ents;  // every update seen, in order
    std::vector<int> pend;      // indices into ents still in flight
    std::vector<int> macReaders;
    unsigned seq;
    std::vector<uint8_t> dead;  // ops to drop

    int Emit(Fn fn, uint16_t a = 0, uint16_t bb = 0, uint16_t c = 0, uint8_t m = 0, uint8_t k = 0)
    {
        b->ops.push_back(Op{fn, a, bb, c, m, k});
        return (int)b->ops.size() - 1;
    }

    void LandFlag(int e)
    {
        FlagEnt &f = ents[e];
        f.landOp = Emit(f.kind == 0 ? LandMacOp : LandClipOp, 0, 0, 0, 0, f.slot);
    }

    // everything that has landed by cycle t (eagerly: the same values the
    // interpreter sees when it lands them lazily)
    void LandBy(uint64_t t)
    {
        if (T.qPend && T.qDone <= t) {
            Emit(LandQ);
            T.qPend = false;
        }
        if (T.pPend && T.pDone <= t) {
            Emit(LandP);
            T.pPend = false;
        }
        size_t k = 0;
        while (k < pend.size() && ents[pend[k]].ready <= t)
            LandFlag(pend[k++]);
        pend.erase(pend.begin(), pend.begin() + k);
    }

    void LandAll()
    {
        LandBy(~0ull);
    }

    void Lands(int reg, uint32_t dest, uint64_t t)
    {
        if (reg <= 0 || reg > 31)
            return;
        for (int k = 0; k < 4; k++)
            if (dest & (8 >> k))
                T.vf[reg][k] = t + 4;
        if (dest & 15)
            T.vfMax[reg] = t + 4;
    }

    void Start(const Sig &s)
    {
        memset(&T, 0, sizeof T);
        const uint64_t base = 16;
        T.cyc = base;
        for (int d = 0; d < 3; d++)
            for (int k = 0; k < 4; k++)
                for (int r = 1; r < 32; r++)
                    if (s.vf[d][k] & (1u << r)) {
                        T.vf[r][k] = base + d + 1;
                        if (T.vfMax[r] < base + d + 1)
                            T.vfMax[r] = base + d + 1;
                    }
        for (int d = 0; d < 3; d++)
            for (int r = 1; r < 16; r++)
                if (s.vi[d] & (1u << r))
                    T.vi[r] = base + d + 1;
        if (s.q) {
            T.qPend = true;
            T.qDone = base + s.q;
        }
        if (s.p) {
            T.pPend = true;
            T.pDone = base + s.p;
        }
        seq = 0;
        for (int i = 0; i < s.nfl; i++) {
            FlagEnt f{base + (s.fl[i] >> 3), (uint8_t)(s.fl[i] & 1), (uint8_t)((s.fl[i] >> 1) & 3), -1, -1};
            ents.push_back(f);
            pend.push_back((int)ents.size() - 1);
            seq = f.slot + 1u;
        }
    }

    // The state after the last instruction (what is in flight lands first
    // where it has by then).
    bool Finish(Sig &s)
    {
        LandBy(T.cyc);
        memset(&s, 0, sizeof s);
        for (int r = 1; r < 32; r++)
            for (int k = 0; k < 4; k++)
                if (T.vf[r][k] > T.cyc) {
                    uint64_t d = T.vf[r][k] - T.cyc;
                    if (d > 3)
                        return false;
                    s.vf[d - 1][k] |= 1u << r;
                }
        for (int r = 1; r < 16; r++)
            if (T.vi[r] > T.cyc) {
                uint64_t d = T.vi[r] - T.cyc;
                if (d > 3)
                    return false;
                s.vi[d - 1] |= (uint16_t)(1u << r);
            }
        if (T.qPend)
            s.q = (uint8_t)(T.qDone - T.cyc);
        if (T.pPend)
            s.p = (uint8_t)(T.pDone - T.cyc);
        if (pend.size() > 3)
            return false;
        s.nfl = (uint8_t)pend.size();
        for (size_t i = 0; i < pend.size(); i++) {
            const FlagEnt &f = ents[pend[i]];
            s.fl[i] = (uint8_t)(f.kind | f.slot << 1 | (f.ready - T.cyc) << 3);
        }
        return true;
    }

    // One instruction. Returns false if it can't be translated.
    bool Instr(uint32_t pc)
    {
        uint64_t ins = g_vu1.micro[pc & 2047];
        uint32_t lo = (uint32_t)ins, hi = (uint32_t)(ins >> 32);
        bool hasLower = !(hi & 0x80000000u);
        Dec d;
        Decode(ins, d);
        uint64_t t = StallOn(T, d);
        T.cyc = t;
        LandBy(t);

        int ureg;
        uint32_t udest;
        int ufk;
        UpperWrites(hi, ureg, udest, ufk);
        Dec du, dl;
        du = Dec{};
        dl = Dec{};
        DecodeStall(hi, lo, false, du);
        uint32_t upR = VfReads(du);
        uint32_t loR = 0;
        int loW = -1;
        uint32_t lop = lo >> 25;
        uint32_t ls = (((lo >> 6) & 31) << 2) | (lo & 3);
        bool special = lop == 0x40 && (lo & 63) >= 0x3C;
        if (hasLower) {
            DecodeStall(0x000002FFu, lo, true, dl);  // a NOP upper
            loR = VfReads(dl);
            loW = d.loVf;
        }
        bool fcset = hasLower && lop == 0x11;
        bool upVf = ureg >= 1 && ureg <= 31;
        bool orderA = !(upVf && ((loR >> ureg) & 1 || loW == ureg));
        bool orderB = !(loW > 0 && (upR >> loW) & 1);
        bool viaTmp = !orderA && !orderB;
        if (fcset && !orderA)
            return false;

        auto upper = [&]() {
            int fe = -1;
            if (ufk >= 0) {
                FlagEnt f{t + 4, (uint8_t)ufk, (uint8_t)(seq++ & 3), -1, -1};
                ents.push_back(f);
                fe = (int)ents.size() - 1;
                pend.push_back(fe);
            }
            uint16_t dst = viaTmp ? kOffTmp : ureg == 32 ? kOffAcc : ureg == 0 ? kOffSink : ureg > 0 ? OffVf(ureg) : 0;
            int idx = -1;
            if (d.ucls >= UC_ADD) {
                static const int kC[] = {0, 0, C_ADD, C_SUB, C_MUL, C_MADD, C_MSUB};
                bool vec = d.usrc == US_VEC;
                uint16_t src = vec ? OffVf(d.uft)
                               : d.usrc == US_BC ? OffVfField(d.uft, d.ubc)
                               : d.usrc == US_Q  ? kOffQ
                                                 : kOffI;
                bool full = viaTmp || ureg == 0 || d.udest == 15;
                idx = Emit(kUpFmac[kC[d.ucls]][vec][full][1], dst, OffVf(d.ufs), src, d.udest, ents[fe].slot);
            } else if (d.ucls == UC_NOP) {
                // nothing
            } else if (FastUpper(hi, dst, fe >= 0 ? ents[fe].slot : 0, idx, viaTmp || ureg == 0)) {
                // MAX/MINI, FTOI/ITOF, ABS, OPMULA/OPMSUB
            } else if (ufk == 1 && (hi & 63) == 0x3F && ((((hi >> 6) & 31) << 2) | (hi & 3)) == 0x1F) {
                idx = Emit(UpClip, OffVf((hi >> 11) & 31), OffVfField((hi >> 16) & 31, 3), 0, 0, ents[fe].slot);
            } else {
                idx = Emit(UpGeneric, (uint16_t)hi, (uint16_t)(hi >> 16), viaTmp ? kOffTmp : 0, 0,
                           fe >= 0 ? ents[fe].slot : 0xFF);
            }
            if (fe >= 0)
                ents[fe].upOp = idx;
        };
        auto lower = [&]() -> bool { return LowerOps(pc, lo, hasLower, d, special, lop, ls); };

        if (orderA) {
            upper();
            if (fcset) {
                LandAll();
            }
            if (!lower())
                return false;
        } else if (orderB) {
            if (!lower())
                return false;
            upper();
        } else {
            upper();
            if (!lower())
                return false;
            if (ureg > 0)
                Emit(TmpCommit, ureg == 32 ? kOffAcc : OffVf(ureg), 0, 0, (uint8_t)udest);
        }
        if (!hasLower)
            Emit(SetI, (uint16_t)lo, (uint16_t)(lo >> 16));

        // timing: what this instruction writes lands later
        if (d.loVf > 0)
            Lands(d.loVf, d.ldest, t);
        if (upVf)
            Lands(ureg, udest, t);
        if (hasLower) {
            int it = (lo >> 16) & 31;
            if ((lop == 0x04 || (special && ls == 0x3E)) && it < 16)
                T.vi[it] = t + 4;
            if (special && (ls == 0x38 || ls == 0x39 || ls == 0x3A)) {
                T.qPend = true;
                T.qDone = t + QLatency(lo);
            }
            if (special && ((ls >= 0x70 && ls <= 0x76) || (ls >= 0x78 && ls <= 0x7E && ls != 0x7B))) {
                T.pPend = true;
                T.pDone = t + PLatency(lo);
            }
        }
        T.cyc = t + 1;
        return true;
    }

    bool LowerOps(uint32_t pc, uint32_t lo, bool hasLower, const Dec &d, bool special, uint32_t lop, uint32_t ls)
    {
        if (!hasLower)
            return true;
        int it = (lo >> 16) & 31, is = (lo >> 11) & 31, id = (lo >> 6) & 31;
        uint32_t dest = (lo >> 21) & 15;
        int imm11 = Sx(lo & 0x7FF, 11);
        bool full = dest == 15;
        auto gen = [&]() { Emit(LowGeneric, (uint16_t)lo, (uint16_t)(lo >> 16), (uint16_t)pc); };
        if (special) {
            int fsf = (lo >> 21) & 3, ftf = (lo >> 23) & 3;
            switch (ls) {
            case 0x30:  // MOVE
                if (it && dest)
                    Emit(full ? LowMove<true> : LowMove<false>, OffVf(it), OffVf(is), 0, (uint8_t)dest);
                return true;
            case 0x31:
                if (it && dest)
                    Emit(LowMr32, OffVf(it), OffVf(is), 0, (uint8_t)dest);
                return true;
            case 0x34:
                if (it)
                    Emit(full ? LowLqi<true> : LowLqi<false>, OffVf(it), (uint16_t)(is & 15), 0, (uint8_t)dest,
                         (is & 15) == 0);
                else if (is & 15)
                    Emit(LowIaddiu, (uint16_t)(is & 15), (uint16_t)(is & 15), 1);
                return true;
            case 0x35:
                Emit(full ? LowSqi<true> : LowSqi<false>, OffVf(is), (uint16_t)(it & 15), 0, (uint8_t)dest,
                     (it & 15) == 0);
                return true;
            case 0x36:
                if (it)
                    Emit(full ? LowLqd<true> : LowLqd<false>, OffVf(it), (uint16_t)(is & 15), 0, (uint8_t)dest,
                         (is & 15) == 0);
                else if (is & 15)
                    Emit(LowIsubiu, (uint16_t)(is & 15), (uint16_t)(is & 15), 1);
                return true;
            case 0x37:
                Emit(full ? LowSqd<true> : LowSqd<false>, OffVf(is), (uint16_t)(it & 15), 0, (uint8_t)dest,
                     (it & 15) == 0);
                return true;
            case 0x38:
                Emit(LowDiv, OffVfField(is, fsf), OffVfField(it, ftf));
                return true;
            case 0x39: case 0x3A:
                Emit(LowQ, (uint16_t)lo, (uint16_t)(lo >> 16));
                return true;
            case 0x3B: case 0x7B:  // WAITQ/WAITP: the stall is static
                return true;
            case 0x3C:
                if (it & 15)
                    Emit(LowMtir, (uint16_t)(it & 15), OffVfField(is, fsf));
                return true;
            case 0x3D:
                if (it)
                    Emit(LowMfir, OffVf(it), (uint16_t)(is & 15), 0, (uint8_t)dest);
                return true;
            case 0x3E: {
                int k = 0;
                while (k < 4 && !Has(dest, k))
                    k++;
                if (k == 4)
                    k = 0;
                if (it & 15)
                    Emit(LowIlwr, (uint16_t)(it & 15), (uint16_t)(is & 15), 0, 0, (uint8_t)k);
                return true;
            }
            case 0x3F:
                Emit(LowIswr, (uint16_t)(it & 15), (uint16_t)(is & 15), 0, (uint8_t)dest);
                return true;
            case 0x64:
                if (it)
                    Emit(LowMfp, OffVf(it), 0, 0, (uint8_t)dest);
                return true;
            case 0x6C:
                Emit(LowXgkick, (uint16_t)(is & 15));
                return true;
            case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x76: case 0x78: case 0x79:
            case 0x7A: case 0x7C: case 0x7D: case 0x7E:
                Emit(LowP, (uint16_t)lo, (uint16_t)(lo >> 16));
                return true;
            }
            (void)ftf;
            gen();
            return true;
        }
        switch (lop) {
        case 0x40: {
            uint32_t f = lo & 63;
            if (f >= 0x30 && f <= 0x35 && f != 0x33) {
                int dst = f == 0x32 ? it : id;
                if (!(dst & 15))
                    return true;
                switch (f) {
                case 0x30: Emit(LowIadd, (uint16_t)(id & 15), (uint16_t)(is & 15), (uint16_t)(it & 15)); break;
                case 0x31: Emit(LowIsub, (uint16_t)(id & 15), (uint16_t)(is & 15), (uint16_t)(it & 15)); break;
                case 0x32: Emit(LowIaddi, (uint16_t)(it & 15), (uint16_t)(is & 15), (uint16_t)(int16_t)Sx(id, 5)); break;
                case 0x34: Emit(LowIand, (uint16_t)(id & 15), (uint16_t)(is & 15), (uint16_t)(it & 15)); break;
                case 0x35: Emit(LowIor, (uint16_t)(id & 15), (uint16_t)(is & 15), (uint16_t)(it & 15)); break;
                }
                return true;
            }
            gen();
            return true;
        }
        case 0x00:
            if (it)
                Emit(full ? LowLq<true> : LowLq<false>, OffVf(it), (uint16_t)(is & 15), (uint16_t)(int16_t)imm11,
                     (uint8_t)dest);
            return true;
        case 0x01:
            Emit(full ? LowSq<true> : LowSq<false>, OffVf(is), (uint16_t)(it & 15), (uint16_t)(int16_t)imm11,
                 (uint8_t)dest);
            return true;
        case 0x04: {
            int k = 0;
            while (k < 4 && !Has(dest, k))
                k++;
            if (k == 4)
                k = 0;
            if (it & 15)
                Emit(LowIlw, (uint16_t)(it & 15), (uint16_t)(is & 15), (uint16_t)(int16_t)imm11, 0, (uint8_t)k);
            return true;
        }
        case 0x05:
            Emit(LowIsw, (uint16_t)(it & 15), (uint16_t)(is & 15), (uint16_t)(int16_t)imm11, (uint8_t)dest);
            return true;
        case 0x08:
            if (it & 15)
                Emit(LowIaddiu, (uint16_t)(it & 15), (uint16_t)(is & 15), (uint16_t)d.limm);
            return true;
        case 0x09:
            if (it & 15)
                Emit(LowIsubiu, (uint16_t)(it & 15), (uint16_t)(is & 15), (uint16_t)d.limm);
            return true;
        case 0x11:
            Emit(LowFcset, (uint16_t)lo, (uint16_t)((lo >> 16) & 0xFF));
            return true;
        case 0x14: case 0x15: case 0x16: case 0x17: case 0x18: case 0x1A: case 0x1B:
            // status/MAC readers (FSSET too: it keeps Z/S)
            macReaders.push_back((int)b->ops.size());
            gen();
            return true;
        // branches
        case 0x20: Emit(BrAlways); return true;
        case 0x21: Emit(BrBal, (uint16_t)(it & 15), 0, (uint16_t)(pc + 2)); return true;
        case 0x24: Emit(BrJr, 0, (uint16_t)(is & 15)); return true;
        case 0x25: Emit(BrJalr, (uint16_t)(it & 15), (uint16_t)(is & 15), (uint16_t)(pc + 2)); return true;
        case 0x28: Emit(BrEq, (uint16_t)(it & 15), (uint16_t)(is & 15)); return true;
        case 0x29: Emit(BrNe, (uint16_t)(it & 15), (uint16_t)(is & 15)); return true;
        case 0x2C: Emit(BrLtz, 0, (uint16_t)(is & 15)); return true;
        case 0x2D: Emit(BrGtz, 0, (uint16_t)(is & 15)); return true;
        case 0x2E: Emit(BrLez, 0, (uint16_t)(is & 15)); return true;
        case 0x2F: Emit(BrGez, 0, (uint16_t)(is & 15)); return true;
        }
        gen();
        return true;
    }

    // The upper ops with handlers of their own besides the FMAC ones.
    bool FastUpper(uint32_t hi, uint16_t dst, uint8_t slot, int &idx, bool full)
    {
        uint32_t dest = (hi >> 21) & 15;
        int ft = (hi >> 16) & 31, fs = (hi >> 11) & 31;
        uint32_t op = hi & 63;
        full = full || dest == 15;
        auto mm = [&](bool max, int src) {
            // src: 0-3 a field of ft, 4 ft, 5 I
            bool vec = src == 4;
            uint16_t c = vec ? OffVf(ft) : src == 5 ? kOffI : OffVfField(ft, src);
            static const Fn t[2][2][2] = {
                {{UpMinMax<false, false, false>, UpMinMax<false, false, true>},
                 {UpMinMax<false, true, false>, UpMinMax<false, true, true>}},
                {{UpMinMax<true, false, false>, UpMinMax<true, false, true>},
                 {UpMinMax<true, true, false>, UpMinMax<true, true, true>}},
            };
            idx = Emit(t[max][vec][full], dst, OffVf(fs), c, (uint8_t)dest);
            return true;
        };
        if (op < 0x1C) {
            if ((op >> 2) == 4)
                return mm(true, op & 3);
            if ((op >> 2) == 5)
                return mm(false, op & 3);
            return false;
        }
        switch (op) {
        case 0x1D: return mm(true, 5);
        case 0x1F: return mm(false, 5);
        case 0x2B: return mm(true, 4);
        case 0x2F: return mm(false, 4);
        case 0x2E: idx = Emit(UpOpm<true, true>, dst, OffVf(fs), OffVf(ft), (uint8_t)dest, slot); return true;
        }
        if (op < 0x3C)
            return false;
        uint32_t sx = (((hi >> 6) & 31) << 2) | (hi & 3);
        static const Fn kFtoi[4] = {UpFtoi<0>, UpFtoi<4>, UpFtoi<12>, UpFtoi<15>};
        static const Fn kItof[4] = {UpItof<0>, UpItof<4>, UpItof<12>, UpItof<15>};
        if (sx >= 0x10 && sx < 0x14) {
            idx = Emit(kItof[sx & 3], dst, OffVf(fs), 0, (uint8_t)dest);
            return true;
        }
        if (sx >= 0x14 && sx < 0x18) {
            idx = Emit(kFtoi[sx & 3], dst, OffVf(fs), 0, (uint8_t)dest);
            return true;
        }
        if (sx == 0x1D) {
            idx = Emit(UpAbs, dst, OffVf(fs), 0, (uint8_t)dest);
            return true;
        }
        if (sx == 0x2E) {
            idx = Emit(UpOpm<false, true>, dst, OffVf(fs), OffVf(ft), (uint8_t)dest, slot);
            return true;
        }
        return false;
    }

    // Drops the MAC updates nothing can see and their landings.
    void Liveness()
    {
        std::vector<int> macs;
        for (size_t i = 0; i < ents.size(); i++)
            if (ents[i].kind == 0)
                macs.push_back((int)i);
        dead.assign(b->ops.size(), 0);
        for (size_t j = 0; j + 1 < macs.size(); j++) {
            const FlagEnt &e = ents[macs[j]], &n = ents[macs[j + 1]];
            if (e.landOp < 0 || n.landOp < 0)
                continue;  // in flight at the end: live
            bool read = false;
            for (int r : macReaders)
                if (r > e.landOp && r < n.landOp)
                    read = true;
            if (read)
                continue;
            dead[e.landOp] = 1;
            if (e.upOp >= 0) {
                Op &o = b->ops[e.upOp];
                for (int c = 0; c < 5; c++)
                    for (int v = 0; v < 2; v++)
                        for (int f = 0; f < 2; f++)
                            if (o.fn == kUpFmac[c][v][f][1])
                                o.fn = kUpFmac[c][v][f][0];
                if (o.fn == UpGeneric)
                    o.k = 0xFF;
                if (o.fn == UpOpm<true, true>)
                    o.fn = UpOpm<true, false>;
                if (o.fn == UpOpm<false, true>)
                    o.fn = UpOpm<false, false>;
            }
        }
        std::vector<Op> keep;
        keep.reserve(b->ops.size() + 1);
        for (size_t i = 0; i < b->ops.size(); i++)
            if (!dead[i])
                keep.push_back(b->ops[i]);
        b->ops.swap(keep);
    }
};

const int kMaxBlock = 256;

Block *Compile(uint32_t pc0, const Sig &sig, uint64_t hash)
{
    Block *b = new Block{};
    b->pc = (uint16_t)pc0;
    b->entry = sig;
    b->hash = hash;
    b->gen = g_gen;
    Compiler c;
    c.b = b;
    c.Start(sig);
    uint32_t pc = pc0;
    int n = 0;
    enum { NORMAL, DELAY, ENDNEXT } state = NORMAL;
    uint64_t brIns = 0;
    uint32_t brPc = 0;
    bool brEnd = false;
    for (;;) {
        pc &= 2047;
        uint64_t ins = g_vu1.micro[pc];
        uint32_t hi = (uint32_t)(ins >> 32);
        if (state == NORMAL) {
            if (n >= kMaxBlock) {
                b->exit = X_FALL;
                b->fall = (uint16_t)pc;
                break;
            }
            if (IsBranch(ins)) {
                uint64_t ds = g_vu1.micro[(pc + 1) & 2047];
                if (IsBranch(ds) || ((ds >> 32) & 0x40000000u)) {
                    b->exit = n ? X_FALL : X_INTERP;
                    b->fall = (uint16_t)pc;
                    if (!n)
                        b->words = {ins, ds};
                    break;
                }
            }
        }
        size_t mark = b->ops.size();
        if (!c.Instr(pc)) {
            // not translatable: the interpreter from here (from the block's
            // start if a branch or [E] is pending)
            if (state != NORMAL)
                n = 0, pc = pc0;
            b->ops.clear();
            b->exit = n ? X_FALL : X_INTERP;
            b->fall = (uint16_t)pc;
            if (!n)
                b->words.clear();
            (void)mark;
            break;
        }
        b->words.push_back(ins);
        n++;
        if (state == DELAY) {
            uint32_t op = (uint32_t)brIns >> 25;
            b->target = (uint16_t)((brPc + 1 + Sx((uint32_t)brIns & 0x7FF, 11)) & 2047);
            b->fall = (uint16_t)((pc + 1) & 2047);
            bool jump = op == 0x24 || op == 0x25;
            b->exit = brEnd ? (jump ? X_ENDJUMP : X_ENDBR) : jump ? X_JUMP : X_COND;
            break;
        }
        if (state == ENDNEXT) {
            b->exit = X_END;
            b->fall = (uint16_t)((pc + 1) & 2047);
            break;
        }
        if (IsBranch(ins)) {
            state = DELAY;
            brIns = ins;
            brPc = pc;
            brEnd = hi & 0x40000000u;
        } else if (hi & 0x40000000u) {
            state = ENDNEXT;
        }
        pc++;
    }
    b->n = (uint16_t)n;
    if (b->exit == X_END || b->exit == X_ENDBR || b->exit == X_ENDJUMP) {
        c.LandAll();
    } else if (b->exit != X_INTERP) {
        if (!c.Finish(b->out)) {
            // a state Sig can't hold: interpret
            b->ops.clear();
            b->n = 0;
            b->exit = X_INTERP;
            b->fall = b->pc;
            b->words.clear();
        }
        b->outHash = Hash(b->out);
    }
    if (b->exit != X_INTERP)
        c.Liveness();
    b->ops.push_back(Op{Ret, 0, 0, 0, 0, 0});
    g_stats.blocks++;
    g_stats.ops += b->ops.size();
    g_stats.instrs += b->n;
    return b;
}

bool Matches(const Block *b)
{
    for (size_t k = 0; k < b->words.size(); k++)
        if (g_vu1.micro[(b->pc + k) & 2047] != b->words[k])
            return false;
    return true;
}

Block *Lookup(uint32_t pc, const Sig &sig, uint64_t hash)
{
    g_stats.lookups++;
    std::vector<Block *> &v = g_blocks[pc];
    for (Block *b : v) {
        if (b->hash != hash || !(b->entry == sig))
            continue;
        if (b->gen == g_gen)
            return b;
        g_stats.verify++;
        if (Matches(b)) {
            b->gen = g_gen;
            return b;
        }
    }
    Block *b = Compile(pc, sig, hash);
    v.push_back(b);
    return b;
}

// The interpreter's pipeline from a Sig (cycle 1000) and back.
void SigToPipe(const Sig &s)
{
    memset(&Pipe, 0, sizeof Pipe);
    const uint64_t base = 1000;
    Pipe.cyc = base;
    for (int d = 0; d < 3; d++)
        for (int k = 0; k < 4; k++)
            for (int r = 1; r < 32; r++)
                if (s.vf[d][k] & (1u << r)) {
                    Pipe.vf[r][k] = base + d + 1;
                    if (Pipe.vfMax[r] < base + d + 1)
                        Pipe.vfMax[r] = base + d + 1;
                }
    for (int d = 0; d < 3; d++)
        for (int r = 1; r < 16; r++)
            if (s.vi[d] & (1u << r))
                Pipe.vi[r] = base + d + 1;
    if (s.q) {
        Pipe.qPend = true;
        Pipe.qDone = base + s.q;
        Pipe.qNext = R.qNext;
    }
    if (s.p) {
        Pipe.pPend = true;
        Pipe.pDone = base + s.p;
        Pipe.pNext = R.pNext;
    }
    for (int i = 0; i < s.nfl; i++)
        Pipe.flags[Pipe.tail++] = {base + (s.fl[i] >> 3), (uint8_t)(s.fl[i] & 1), R.fl[(s.fl[i] >> 1) & 3]};
}

bool PipeToSig(Sig &s)
{
    uint64_t c = Pipe.cyc;
    if (Pipe.qPend && Pipe.qDone <= c) {
        R.q = Pipe.qNext;
        Pipe.qPend = false;
    }
    if (Pipe.pPend && Pipe.pDone <= c) {
        R.p = Pipe.pNext;
        Pipe.pPend = false;
    }
    while (Pipe.head != Pipe.tail && Pipe.flags[Pipe.head & 63].ready <= c) {
        const Timing::Flag &f = Pipe.flags[Pipe.head++ & 63];
        if (f.kind == 0)
            LandMac(f.v);
        else
            LandClip(f.v);
    }
    memset(&s, 0, sizeof s);
    bool ok = true;
    for (int r = 1; r < 32; r++)
        for (int k = 0; k < 4; k++)
            if (Pipe.vf[r][k] > c) {
                uint64_t d = Pipe.vf[r][k] - c;
                if (d > 3)
                    ok = false;
                else
                    s.vf[d - 1][k] |= 1u << r;
            }
    for (int r = 1; r < 16; r++)
        if (Pipe.vi[r] > c) {
            uint64_t d = Pipe.vi[r] - c;
            if (d > 3)
                ok = false;
            else
                s.vi[d - 1] |= (uint16_t)(1u << r);
        }
    if (Pipe.qPend) {
        s.q = (uint8_t)(Pipe.qDone - c);
        R.qNext = Pipe.qNext;
    }
    if (Pipe.pPend) {
        s.p = (uint8_t)(Pipe.pDone - c);
        R.pNext = Pipe.pNext;
    }
    unsigned n = Pipe.tail - Pipe.head;
    if (n > 3)
        ok = false;
    for (unsigned i = 0; i < n && i < 3; i++) {
        const Timing::Flag &f = Pipe.flags[(Pipe.head + i) & 63];
        R.fl[i] = f.v;
        s.fl[i] = (uint8_t)(f.kind | i << 1 | (f.ready - c) << 3);
    }
    s.nfl = (uint8_t)(n < 3 ? n : 3);
    if (ok) {
        // the interpreter's pipeline is empty again (vu1_run's CommitAll)
        Pipe.qPend = Pipe.pPend = false;
        Pipe.head = Pipe.tail = 0;
    }
    return ok;
}

} // namespace

void RecRun(uint32_t pc, int &steps)
{
    static const Sig kStart{};
    static const uint64_t kStartHash = Hash(kStart);
    const Sig *sig = &kStart;  // the state at pc
    uint64_t hash = kStartHash;
    Sig fromInterp;
    Block *b = nullptr;
    const int interpFrom = kMaxSteps - (1 << 20);
    for (;;) {
        pc &= 2047;
        if (__builtin_expect((R.status & 0xC0) != 0xC0 || steps >= interpFrom, 0)) {
            // status's sticky bits not both set yet (or a runaway): the
            // interpreter runs the rest
            SigToPipe(*sig);
            g_stats.stickyRuns++;
            int s0 = steps;
            Interp(pc, false, steps);
            Commit(~0ull);
            g_stats.interp += steps - s0;
            return;
        }
        if (!b)
            b = Lookup(pc, *sig, hash);
        if (b->exit == X_INTERP) {
            SigToPipe(*sig);
            g_stats.interpRuns++;
            int s0 = steps;
            bool end = Interp(pc, true, steps);
            if (!end && !PipeToSig(fromInterp))
                end = Interp(pc, false, steps);
            g_stats.interp += steps - s0;
            if (end) {
                Commit(~0ull);
                return;
            }
            sig = &fromInterp;
            hash = Hash(fromInterp);
            b = nullptr;
            continue;
        }
        b->ops[0].fn(b->ops.data());
        steps += b->n;
        Block **link;
        switch (b->exit) {
        case X_END:
            g_vu1.pc = b->fall;
            return;
        case X_ENDBR:
            g_vu1.pc = R.br ? b->target : b->fall;
            return;
        case X_ENDJUMP:
            g_vu1.pc = R.jt & 2047;
            return;
        case X_FALL:
            pc = b->fall;
            link = &b->link[0];
            break;
        case X_COND:
            if (R.br) {
                pc = b->target;
                link = &b->link[1];
            } else {
                pc = b->fall;
                link = &b->link[0];
            }
            break;
        default:  // X_JUMP
            pc = R.jt & 2047;
            link = &b->link[1];
            break;
        }
        sig = &b->out;
        hash = b->outHash;
        Block *nb = *link;
        if (__builtin_expect(nb && nb->gen == g_gen && nb->pc == pc, 1)) {
            b = nb;
            continue;
        }
        nb = Lookup(pc, *sig, hash);
        *link = nb;
        b = nb;
    }
}

void RecStats()
{
    port_log("VU1 rec: %llu blocks, %llu ops for %llu instructions, %llu lookups, %llu rechecks, %llu interpreted "
             "(%llu runs, %llu before status's sticky bits)",
             (unsigned long long)g_stats.blocks, (unsigned long long)g_stats.ops, (unsigned long long)g_stats.instrs,
             (unsigned long long)g_stats.lookups, (unsigned long long)g_stats.verify,
             (unsigned long long)g_stats.interp, (unsigned long long)g_stats.interpRuns,
             (unsigned long long)g_stats.stickyRuns);
}

} // namespace vu1i

void vu1_micro_changed()
{
    vu1i::g_gen++;
}
