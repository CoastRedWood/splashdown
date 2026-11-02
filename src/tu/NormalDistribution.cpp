// NormalDistribution.cpp  @ 0x0025EF80 (text 0x25EF80-0x25F2D0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's NormalDistribution.cpp as one translation unit, the first of five
// TUs in its tu_map row: NormalDistribution's three functions, then
// GetOrientationVectors (no data of its own: nothing in the data decides
// between this TU's tail and Overlay.cpp's head), then the static initializer
// (GlobalNormalDistribution only; this TU does not include the axis-vector
// header). `make link` places the object's data as well as its code
// (tools/link_c.py).


#include "d3dtypes.h"
#include "NormalDistribution/NormalDistribution.h"

extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);

// ---- src/NormalDistribution/ctor.cpp
NormalDistribution::NormalDistribution(int resolution)
{
    m_tableSize = resolution + 1;
    m_table = (float *)malloc(m_tableSize * sizeof(float));
    float x = 0.0f;
    for (int i = 0; i < m_tableSize; i++) {
        // Cubic falloff over 0..3 sigma.
        m_table[i] = 1.0f - 0.948667f * x + 0.2964f * x * x - 0.030333f * x * x * x;
        x += 3.0f / m_tableSize;
    }
}

// ---- src/NormalDistribution/dtor.cpp
NormalDistribution::~NormalDistribution()
{
    if (m_table)
        free(m_table);
}

// ---- src/NormalDistribution/GetProbability.cpp
#pragma fast_fptosi on  // retail never calls fptosi: float->int is an inline cvt.w.s

// Retail's fabs is an inline-asm helper: abs.s into $f8, stored through a
// pointer to a stack temporary.
inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

float NormalDistribution::GetProbability(float mean, float x, float sigma)
{
    if (sigma <= 0.0f)
        return 0.0f;
    float d = x - mean;
    d = Fabs(d) / sigma;
    if (d >= 3.0f)
        return 0.0f;
    return m_table[(int)(d * m_tableSize / 3.0f)];
}
#pragma fast_fptosi reset

// ---- src/global/GetOrientationVectors.cpp
// `p` holds the pitched up->z and is then reused for forward->y * c: the second
// definition keeps p from being forward-substituted into its store, so z' is computed
// first (as retail) while still being stored before y'.


void FastSinCos(float x, float *sine, float *cosine);

// Builds the forward (+z) and up (+y) vectors for the given yaw, pitch and
// roll: roll about z, then pitch about x, then yaw about y.
int GetOrientationVectors(_D3DVECTOR *forward, _D3DVECTOR *up, float yaw, float pitch, float roll)
{
    float c;
    float s;
    float a;
    float b;
    float p;
    if (!up || !forward)
        return 0;
    up->x = 0.0f;
    up->y = 1.0f;
    up->z = 0.0f;
    forward->x = 0.0f;
    forward->y = 0.0f;
    forward->z = 1.0f;

    FastSinCos(roll, &s, &c);
    a = up->x * c + up->y * s;
    b = up->y * c - up->x * s;
    up->x = a;
    up->y = b;

    FastSinCos(pitch, &s, &c);
    p = up->z * c - up->y * s;
    b = up->y * c + up->z * s;
    up->z = p;
    up->y = b;
    a = forward->z * c - forward->y * s;
    float q = forward->z * s;
    p = forward->y * c;
    forward->z = a;
    forward->y = p + q;

    FastSinCos(yaw, &s, &c);
    a = up->x * c + up->z * s;
    b = up->z * c - up->x * s;
    up->x = a;
    up->z = b;
    b = forward->z * c - forward->x * s;
    a = forward->x * c + forward->z * s;
    forward->x = a;
    forward->z = b;
    return 1;
}


// ---- src/sinit/NormalDistribution.cpp
// Static initializer of NormalDistribution.cpp: the global lookup table.
NormalDistribution GlobalNormalDistribution(100);
NormalDistribution *LpNormalDistribution = &GlobalNormalDistribution;
