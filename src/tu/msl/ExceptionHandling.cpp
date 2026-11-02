// ExceptionHandling.cpp  @ 0x0039FE20
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// The MSL C++ runtime's exception handling for the PS2 (MIPS) port as one
// translation unit, 0x39FE20-0x3A1620: the number
// decoders, __end__catch, __ThrowHandler, __unexpected, std::bad_exception
// and the action-list walkers; its vtable copies end it (__TransferControl,
// __throw and the frame helpers after them are ExceptionFrames.cpp). Its
// .rodata (0x43D360-0x43D508) is one rising label run, @414-@1085: four
// jump tables, the bad_exception strings and RTTI; vtable bad_exception
// 0x44E460. Retail's .symtab has
// FindExceptionHandler, FindMostRecentException, UnwindStack, NextAction and
// FindExceptionRecord as locals, so they are static here. MSL's file for
// this is ExceptionPPC.cp on PowerPC; the name is a guess.
//
// One GetUnalignedPointer per form: UnwindStack's destructor and delete
// function reads need the form that assigns a char * (GetUnalignedPointerDirect);
// every other reader, UnwindStack's catch-type skip included, stores through
// the out pointer as an unsigned int.

// The C++ runtime (MSL) was built with exceptions on, unlike game code.
#pragma exceptions on

#include "NMWException.h"

namespace std {

class exception {
public:
    exception() {}
    virtual ~exception() {}
    virtual const char *what() const;
};

class bad_exception : public exception {
public:
    bad_exception() {}
    virtual ~bad_exception() {}
    virtual const char *what() const { return "bad_exception"; }
};

void unexpected();
void terminate();

}

extern "C" int __throw_catch_compare(char *throwtype, char *catchtype, long *offset_result);

int __FindExceptionTable(ExceptionInfo *info, char *retaddr);
void __SetupFrameInfo(ThrowContext *context, ExceptionInfo *info);
char *__PopStackFrame(ThrowContext *context, ExceptionInfo *info);
void __TransferControl(ThrowContext *context, ExceptionInfo *info, char *address);

static char *FindExceptionHandler(ThrowContext *context, ExceptionInfo *info, long *result_offset);
static CatchInfo *FindMostRecentException(ThrowContext *context, ExceptionInfo *info);
static void UnwindStack(ThrowContext *context, ExceptionInfo *info, char *catcher);
static exaction_type NextAction(ActionIterator *iter);
static void FindExceptionRecord(char *returnaddr, ExceptionInfo *info);

#define EXCEPTION_ACTION_END        0x80    // last action of a frame
#define EXCEPTION_ACTION_REGCOND    0x40    // condition held in a register
#define EXCEPTION_ACTION_REGPTR     0x20    // pointer held in a register

struct ex_catchblock {
    char *catch_type;
    unsigned int catch_pcoffset;
    int cinfo_ref;
};

struct ex_specification {
    unsigned int specs;         // number of types in the list
    unsigned int pcoffset;
    int cinfo_ref;
    char *spcl;                 // the type pointers, unaligned
};

// Exception-table pointers are stored unaligned, little-endian. Two forms
// of the (pointer, out) reader: this one stores through `out` as an
// unsigned int; UnwindStack's destructor and delete-function reads need the
// other (GetUnalignedPointerDirect, below), which assigns a char *.
static inline char *GetUnalignedPointer(char *p, char **out)
{
    unsigned char *s = (unsigned char *)p;
    *(unsigned int *)out = s[0] | (s[1] << 8) | (s[2] << 16) | (s[3] << 24);
    return p + 4;
}

static inline char *GetUnalignedPointer(char *p)
{
    char *value;

    *(unsigned int *)&value = ((unsigned char *)p)[0] | (((unsigned char *)p)[1] << 8) | (((unsigned char *)p)[2] << 16) | (((unsigned char *)p)[3] << 24);
    return value;
}

#define UNALIGNED_POINTER(p) \
    ((char *)(((unsigned char *)(p))[0] | (((unsigned char *)(p))[1] << 8) | (((unsigned char *)(p))[2] << 16) | (((unsigned char *)(p))[3] << 24)))

static inline char *GetUnalignedPointerDirect(char *p, char **out)
{
    *out = UNALIGNED_POINTER(p);
    return p + 4;
}

static inline int IsInSpecification(char *extype, ex_specification *spec)
{
    long offset;
    char *p;
    unsigned int i;

    for (i = 0, p = spec->spcl; i < spec->specs; p += 4, i++) {
        if (__throw_catch_compare(extype, GetUnalignedPointer(p), &offset))
            return 1;
    }
    return 0;
}

static inline exaction_type CurrentAction(ActionIterator *iter)
{
    return iter->info.action_pointer ? *(exaction_type *)iter->info.action_pointer & EXCEPTION_ACTION_MASK : EXCEPTION_ACTION_ENDOFLIST;
}

// Variable-length unsigned integer: the low bits of the first byte say how
// many bytes follow (xxxxxxx0: 1 byte, xxxxxx01: 2, xxxxx011: 3, else 4).
char *__DecodeUnsignedNumber(char *p, unsigned int *value)
{
    unsigned char *s = (unsigned char *)p;
    unsigned int c0 = s[0];
    if (!(c0 & 1)) {
        *value = c0 >> 1;
        return p + 1;
    }
    unsigned int c1 = s[1];
    if (!(c0 & 2)) {
        *value = ((c0 >> 2) << 8) | c1;
        return p + 2;
    }
    unsigned int c2 = s[2];
    if (!(c0 & 4)) {
        *value = ((c0 >> 3) << 16) | (c1 << 8) | c2;
        return p + 3;
    }
    *value = ((c0 >> 3) << 24) | (c1 << 16) | (c2 << 8) | s[3];
    return p + 4;
}

// Signed counterpart of __DecodeUnsignedNumber: the first byte is read
// signed, so its high bits are sign-extended into the result.
char *__DecodeSignedNumber(char *p, int *value)
{
    unsigned char *s = (unsigned char *)p;
    int c0 = (signed char)s[0];
    if (!(c0 & 1)) {
        *value = c0 >> 1;
        return p + 1;
    }
    unsigned int c1 = s[1];
    if (!(c0 & 2)) {
        *value = ((c0 >> 2) << 8) | c1;
        return p + 2;
    }
    unsigned int c2 = s[2];
    if (!(c0 & 4)) {
        *value = ((c0 >> 3) << 16) | (c1 << 8) | c2;
        return p + 3;
    }
    *value = ((c0 >> 3) << 24) | (c1 << 16) | (c2 << 8) | s[3];
    return p + 4;
}

extern "C" void __end__catch(CatchInfo *catchinfo)
{
    if (catchinfo->location && catchinfo->dtor)
        ((void (*)(void *, int))catchinfo->dtor)(catchinfo->location, -1);
}

// Called by __throw with the thrower's context. Finds the handler, unwinds
// to its frame, fills in its CatchInfo and jumps to it.
void __ThrowHandler(ThrowContext *context)
{
    ExceptionInfo info;
    ex_catchblock catchblock;
    long result_offset, offset;
    char *p, *ap;
    CatchInfo *catchinfo;

    FindExceptionRecord(context->returnaddr, &info);
    if (!info.exception_record)
        std::terminate();
    __SetupFrameInfo(context, &info);
    if (!context->throwtype)
        context->catchinfo = FindMostRecentException(context, &info);
    else
        context->catchinfo = 0;

    p = FindExceptionHandler(context, &info, &result_offset);
    ap = GetUnalignedPointer(p + 1, &catchblock.catch_type);
    // A no-op on this target (retail keeps the test).
    catchblock.catch_type = catchblock.catch_type ? catchblock.catch_type : 0;
    ap = __DecodeUnsignedNumber(ap, &catchblock.catch_pcoffset);
    __DecodeSignedNumber(ap, &catchblock.cinfo_ref);
    UnwindStack(context, &info, p);

    catchinfo = (CatchInfo *)(context->frameBase + (long)catchblock.cinfo_ref);
    offset = result_offset;   // read once for both arms below
    catchinfo->location = context->location;
    catchinfo->typeinfo = context->throwtype;
    catchinfo->dtor = context->dtor;
    if (*context->throwtype == '*') {
        catchinfo->sublocation = &catchinfo->pointercopy;
        catchinfo->pointercopy = *(int *)context->location + offset;
    } else {
        catchinfo->sublocation = (char *)context->location + offset;
    }
    __TransferControl(context, &info, info.current_function + catchblock.catch_pcoffset);
}

// Walk the action lists up the stack for the catch block that takes the
// thrown type. An exception specification that rejects the type transfers
// control to its handler (which calls __unexpected) instead.
static char *FindExceptionHandler(ThrowContext *context, ExceptionInfo *info, long *result_offset)
{
    ActionIterator iter;
    exaction_type action;

    iter.info = *info;
    iter.context = *context;
    for (action = CurrentAction(&iter);; action = NextAction(&iter)) {
        switch (action) {
        case EXCEPTION_ACTION_CATCHBLOCK: {
            ex_catchblock catchblock;
            char *ap;

            ap = iter.info.action_pointer;
            ap = GetUnalignedPointer(ap + 1, &catchblock.catch_type);
            catchblock.catch_type = catchblock.catch_type ? catchblock.catch_type : 0;
            ap = __DecodeUnsignedNumber(ap, &catchblock.catch_pcoffset);
            __DecodeSignedNumber(ap, &catchblock.cinfo_ref);
            if (!__throw_catch_compare(context->throwtype, catchblock.catch_type, result_offset))
                continue;
            break;
        }
        case EXCEPTION_ACTION_SPECIFICATION: {
            ex_specification spec;
            char *ap;
            CatchInfo *catchinfo;

            ap = __DecodeUnsignedNumber(iter.info.action_pointer + 1, &spec.specs);
            ap = __DecodeUnsignedNumber(ap, &spec.pcoffset);
            ap = __DecodeSignedNumber(ap, &spec.cinfo_ref);
            spec.spcl = ap;
            if (!IsInSpecification(context->throwtype, &spec)) {
                // the type is not in the specification: unwind to the
                // function and run its unexpected-exception handler
                ap = iter.info.action_pointer;
                UnwindStack(context, info, ap);
                catchinfo = (CatchInfo *)(context->frameBase + spec.cinfo_ref);
                catchinfo->location = context->location;
                catchinfo->typeinfo = context->throwtype;
                catchinfo->dtor = context->dtor;
                catchinfo->stacktop = ap;
                __TransferControl(context, info, info->current_function + spec.pcoffset);
            }
            continue;
        }
        case EXCEPTION_ACTION_BRANCH:
        case EXCEPTION_ACTION_TERMINATE:
        default:
            std::terminate();
            break;
        case EXCEPTION_ACTION_ENDOFLIST:
        case EXCEPTION_ACTION_DESTROYLOCAL:
        case EXCEPTION_ACTION_DESTROYLOCALCOND:
        case EXCEPTION_ACTION_DESTROYLOCALPOINTER:
        case EXCEPTION_ACTION_DESTROYLOCALARRAY:
        case EXCEPTION_ACTION_DESTROYBASE:
        case EXCEPTION_ACTION_DESTROYMEMBER:
        case EXCEPTION_ACTION_DESTROYMEMBERCOND:
        case EXCEPTION_ACTION_DESTROYMEMBERARRAY:
        case EXCEPTION_ACTION_DELETEPOINTER:
        case EXCEPTION_ACTION_DELETEPOINTERCOND:
        case EXCEPTION_ACTION_ACTIVECATCHBLOCK:
            continue;
        }
        break;
    }
    return iter.info.action_pointer;
}

// Called when a function throws something its exception specification
// does not allow. Calls std::unexpected(); if that throws, the new exception
// is rethrown when the specification allows it, replaced by
// std::bad_exception when that is allowed, else std::terminate() is called.
extern "C" void __unexpected(CatchInfo *catchinfo)
{
    char *unexp = (char *)catchinfo->stacktop;
    ex_specification spec;

#pragma exception_magic     // __exception_magic: the CatchInfo of the catch(...)

    try {
        std::unexpected();
    } catch (...) {
        char *p;

        p = __DecodeUnsignedNumber(unexp + 1, &spec.specs);
        p = __DecodeUnsignedNumber(p, &spec.pcoffset);
        p = __DecodeSignedNumber(p, &spec.cinfo_ref);
        spec.spcl = p;
        if (IsInSpecification((char *)((CatchInfo *)&__exception_magic)->typeinfo, &spec))
            throw;
        if (IsInSpecification("!std::bad_exception!!", &spec))
            throw std::bad_exception();
    }
    std::terminate();
}

// mwcc writes an out-of-line inline copy a function needs (__unexpected's
// thrown bad_exception: its dtor) at the next new top-level declaration;
// retail has bad_exception's dtor right after __unexpected, so these
// typedefs (UnwindStack's) come here, before FindMostRecentException (whose
// definition, prototyped above, doesn't count).
typedef void (*DtorFunc)(void *object, short flag);
typedef void (*DeleteFunc)(void *object);

// For a rethrow (`throw;`), find the active catch block of the current
// frame and rethrow the object it caught.
static CatchInfo *FindMostRecentException(ThrowContext *context, ExceptionInfo *info)
{
    ActionIterator iter;
    exaction_type action;
    CatchInfo *catchinfo;
    int offset;

    iter.info = *info;
    iter.context = *context;
    for (action = CurrentAction(&iter);; action = NextAction(&iter)) {
        switch (action) {
        case EXCEPTION_ACTION_ACTIVECATCHBLOCK:
            break;
        case EXCEPTION_ACTION_ENDOFLIST:
        case EXCEPTION_ACTION_DESTROYLOCAL:
        case EXCEPTION_ACTION_DESTROYLOCALCOND:
        case EXCEPTION_ACTION_DESTROYLOCALPOINTER:
        case EXCEPTION_ACTION_DESTROYLOCALARRAY:
        case EXCEPTION_ACTION_DESTROYBASE:
        case EXCEPTION_ACTION_DESTROYMEMBER:
        case EXCEPTION_ACTION_DESTROYMEMBERCOND:
        case EXCEPTION_ACTION_DESTROYMEMBERARRAY:
        case EXCEPTION_ACTION_DELETEPOINTER:
        case EXCEPTION_ACTION_DELETEPOINTERCOND:
        case EXCEPTION_ACTION_CATCHBLOCK:
        case EXCEPTION_ACTION_SPECIFICATION:
            continue;
        case EXCEPTION_ACTION_BRANCH:
        case EXCEPTION_ACTION_TERMINATE:
        default:
            std::terminate();
        }
        break;
    }

    __DecodeSignedNumber(iter.info.action_pointer + 1, &offset);
    catchinfo = (CatchInfo *)(iter.context.frameBase + offset);
    context->throwtype = (char *)catchinfo->typeinfo;
    context->location = catchinfo->location;
    context->dtor = 0;
    context->catchinfo = catchinfo;
    return catchinfo;
}

// A case's register flags, read from the action byte in memory rather than
// from `action`. Retail computes them at the top of the case and keeps them
// in $s registers across the decode calls: a right side that loads memory
// is not forward-substituted past those calls, and the action-byte temp
// is then created after the array counters' (action gets $s3).
#define ACTION_FLAG(info, flag) (*(exaction_type *)(info)->action_pointer & (flag))


#define DTORCALL_COMPLETE(dtor, objptr) (((DtorFunc)(dtor))((objptr), -1))
#define DTORCALL_PARTIAL(dtor, objptr) (((DtorFunc)(dtor))((objptr), 0))

// A local's value: from the saved register file or from the frame.
#define LOCAL_COND(context, flag, ref) \
    ((flag) ? (char)(context)->GPR[ref] : *(char *)((context)->frameBase + (ref)))
#define LOCAL_POINTER(context, flag, ref) \
    ((flag) ? (char *)(context)->GPR[ref] : *(char **)((context)->frameBase + (ref)))

// Run the cleanup actions (destroy locals, members, arrays, delete
// pointers, ...) of every frame between the throw and the catch block
// `catcher`, popping stack frames as their action lists end.
static void UnwindStack(ThrowContext *context, ExceptionInfo *info, char *catcher)
{
    exaction_type action;
    char *ap, *p;

    for (;;) {
        if (info->action_pointer == 0) {
            // no actions: go to the next stack frame
            FindExceptionRecord(__PopStackFrame(context, info), info);
            if (info->exception_record == 0)
                std::terminate();
            __SetupFrameInfo(context, info);
            if (info->action_pointer == 0)
                continue;
        }
        ap = info->action_pointer;
        action = *(exaction_type *)ap;
        switch (action & EXCEPTION_ACTION_MASK) {
        case EXCEPTION_ACTION_BRANCH: {
            int target;
            __DecodeSignedNumber(ap + 1, &target);
            info->action_pointer += target;
            break;
        }
        case EXCEPTION_ACTION_DESTROYLOCAL: {
            int local;
            char *dtor;
            p = __DecodeSignedNumber(ap + 1, &local);
            p = GetUnalignedPointerDirect(p, &dtor);
            DTORCALL_COMPLETE(dtor, context->frameBase + local);
            info->action_pointer = p;
            break;
        }
        case EXCEPTION_ACTION_DESTROYLOCALCOND: {
            int regcond = ACTION_FLAG(info, EXCEPTION_ACTION_REGCOND);
            int local, cond;
            char *dtor;
            p = __DecodeSignedNumber(ap + 1, &cond);
            p = __DecodeSignedNumber(p, &local);
            p = GetUnalignedPointerDirect(p, &dtor);
            if (LOCAL_COND(context, regcond, cond))
                DTORCALL_COMPLETE(dtor, context->frameBase + local);
            info->action_pointer = p;
            break;
        }
        case EXCEPTION_ACTION_DESTROYLOCALPOINTER: {
            int regptr = ACTION_FLAG(info, EXCEPTION_ACTION_REGPTR);
            int pointer;
            char *dtor;
            p = __DecodeSignedNumber(ap + 1, &pointer);
            p = GetUnalignedPointerDirect(p, &dtor);
            DTORCALL_COMPLETE(dtor, LOCAL_POINTER(context, regptr, pointer));
            info->action_pointer = p;
            break;
        }
        case EXCEPTION_ACTION_DESTROYLOCALARRAY: {
            unsigned int element_size, elements;
            int localarray;
            char *ptr, *dtor;
            unsigned int i;
            p = __DecodeSignedNumber(ap + 1, &localarray);
            p = __DecodeUnsignedNumber(p, &elements);
            p = __DecodeUnsignedNumber(p, &element_size);
            p = GetUnalignedPointerDirect(p, &dtor);
            ptr = context->frameBase + localarray;   // two steps, as retail adds into ptr's register
            for (ptr += elements * element_size, i = elements; i > 0; i--) {
                ptr -= element_size;
                DTORCALL_COMPLETE(dtor, ptr);
            }
            info->action_pointer = p;
            break;
        }
        case EXCEPTION_ACTION_DESTROYBASE: {
            int regptr = ACTION_FLAG(info, EXCEPTION_ACTION_REGPTR);
            int offset, pointer;
            char *dtor;
            p = __DecodeSignedNumber(ap + 1, &pointer);
            p = __DecodeSignedNumber(p, &offset);
            p = GetUnalignedPointerDirect(p, &dtor);
            DTORCALL_PARTIAL(dtor, LOCAL_POINTER(context, regptr, pointer) + offset);
            info->action_pointer = p;
            break;
        }
        case EXCEPTION_ACTION_DESTROYMEMBER: {
            int regptr = ACTION_FLAG(info, EXCEPTION_ACTION_REGPTR);
            int offset, objectptr;
            char *dtor;
            p = __DecodeSignedNumber(ap + 1, &objectptr);
            p = __DecodeSignedNumber(p, &offset);
            p = GetUnalignedPointerDirect(p, &dtor);
            DTORCALL_COMPLETE(dtor, LOCAL_POINTER(context, regptr, objectptr) + offset);
            info->action_pointer = p;
            break;
        }
        case EXCEPTION_ACTION_DESTROYMEMBERCOND: {
            int offset, objectptr, cond;
            char *dtor;
            // declared after the other locals: retail's $s0/$s1 for the flags
            int regcond = ACTION_FLAG(info, EXCEPTION_ACTION_REGCOND);
            int regptr = ACTION_FLAG(info, EXCEPTION_ACTION_REGPTR);
            p = __DecodeSignedNumber(ap + 1, &cond);
            p = __DecodeSignedNumber(p, &objectptr);
            p = __DecodeSignedNumber(p, &offset);
            p = GetUnalignedPointerDirect(p, &dtor);
            if (LOCAL_COND(context, regcond, cond))
                DTORCALL_COMPLETE(dtor, LOCAL_POINTER(context, regptr, objectptr) + offset);
            info->action_pointer = p;
            break;
        }
        case EXCEPTION_ACTION_DESTROYMEMBERARRAY: {
            int regptr = ACTION_FLAG(info, EXCEPTION_ACTION_REGPTR);
            unsigned int element_size, elements;
            int offset, objectptr;
            char *ptr, *dtor;
            unsigned int i;
            p = __DecodeSignedNumber(ap + 1, &objectptr);
            p = __DecodeSignedNumber(p, &offset);
            p = __DecodeUnsignedNumber(p, &elements);
            p = __DecodeUnsignedNumber(p, &element_size);
            p = GetUnalignedPointerDirect(p, &dtor);
            if (regptr)
                ptr = (char *)context->GPR[objectptr] + offset;
            else
                ptr = *(char **)(context->frameBase + objectptr) + offset;
            for (ptr += elements * element_size, i = elements; i > 0; i--) {
                ptr -= element_size;
                DTORCALL_COMPLETE(dtor, ptr);
            }
            info->action_pointer = p;
            break;
        }
        case EXCEPTION_ACTION_DELETEPOINTER: {
            int regptr = ACTION_FLAG(info, EXCEPTION_ACTION_REGPTR);
            int objectptr;
            char *deletefunc;
            p = __DecodeSignedNumber(ap + 1, &objectptr);
            p = GetUnalignedPointerDirect(p, &deletefunc);
            ((DeleteFunc)deletefunc)(LOCAL_POINTER(context, regptr, objectptr));
            info->action_pointer = p;
            break;
        }
        case EXCEPTION_ACTION_DELETEPOINTERCOND: {
            int regcond = ACTION_FLAG(info, EXCEPTION_ACTION_REGCOND);
            int regptr = ACTION_FLAG(info, EXCEPTION_ACTION_REGPTR);
            int objectptr, cond;
            char *deletefunc;
            p = __DecodeSignedNumber(ap + 1, &cond);
            p = __DecodeSignedNumber(p, &objectptr);
            p = GetUnalignedPointerDirect(p, &deletefunc);
            if (LOCAL_COND(context, regcond, cond))
                ((DeleteFunc)deletefunc)(LOCAL_POINTER(context, regptr, objectptr));
            info->action_pointer = p;
            break;
        }
        case EXCEPTION_ACTION_CATCHBLOCK: {
            int cinfo_ref;
            unsigned int catch_pcoffset;
            char *catch_type;
            if (catcher == ap)
                return;
            p = GetUnalignedPointer(ap + 1, &catch_type);
            p = __DecodeUnsignedNumber(p, &catch_pcoffset);
            info->action_pointer = __DecodeSignedNumber(p, &cinfo_ref);
            break;
        }
        case EXCEPTION_ACTION_ACTIVECATCHBLOCK: {
            int cinfo_ref;
            CatchInfo *catchinfo;
            p = __DecodeSignedNumber(ap + 1, &cinfo_ref);
            catchinfo = (CatchInfo *)(context->frameBase + cinfo_ref);
            if (catchinfo->dtor) {
                if (context->location == catchinfo->location) {
                    // the rethrown object: its new handler destroys it
                    context->dtor = catchinfo->dtor;
                } else {
                    DTORCALL_COMPLETE(catchinfo->dtor, catchinfo->location);
                }
            }
            info->action_pointer = p;
            break;
        }
        case EXCEPTION_ACTION_SPECIFICATION: {
            int cinfo_ref;
            unsigned int pcoffset, specs;
            if (catcher == ap)
                return;
            p = __DecodeUnsignedNumber(ap + 1, &specs);
            p = __DecodeUnsignedNumber(p, &pcoffset);
            p = __DecodeSignedNumber(p, &cinfo_ref);
            info->action_pointer = p + specs * 4;  // + the type list
            break;
        }
        default:
            std::terminate();
        }
        if (action & EXCEPTION_ACTION_END)
            info->action_pointer = 0;
    }
}

// Step an action iterator to the next action, popping stack frames at the
// end of each frame's action list and following branches.
static exaction_type NextAction(ActionIterator *iter)
{
    ThrowContext *context = &iter->context;
    exaction_type action;
    char *ap, *p;

    for (;;) {
        ap = iter->info.action_pointer;
        if (ap == 0 || (*(exaction_type *)ap & EXCEPTION_ACTION_END)) {
            // end of the action list: go to the next stack frame
            FindExceptionRecord(__PopStackFrame(context, &iter->info), &iter->info);
            if (iter->info.exception_record == 0)
                std::terminate();
            __SetupFrameInfo(context, &iter->info);
            if (iter->info.action_pointer == 0)
                continue;
            break;
        }
        switch (*(exaction_type *)ap & EXCEPTION_ACTION_MASK) {
        case EXCEPTION_ACTION_DESTROYLOCAL: {
            int local;
            p = __DecodeSignedNumber(ap + 1, &local);
            iter->info.action_pointer = p + 4;      // + the destructor
            break;
        }
        case EXCEPTION_ACTION_DESTROYLOCALCOND: {
            int cond, local;
            p = __DecodeSignedNumber(ap + 1, &local);
            p = __DecodeSignedNumber(p, &cond);
            iter->info.action_pointer = p + 4;
            break;
        }
        case EXCEPTION_ACTION_DESTROYLOCALPOINTER: {
            int pointer;
            p = __DecodeSignedNumber(ap + 1, &pointer);
            iter->info.action_pointer = p + 4;
            break;
        }
        case EXCEPTION_ACTION_DESTROYLOCALARRAY: {
            unsigned int element_size, elements;
            int localarray;
            p = __DecodeSignedNumber(ap + 1, &localarray);
            p = __DecodeUnsignedNumber(p, &elements);
            p = __DecodeUnsignedNumber(p, &element_size);
            iter->info.action_pointer = p + 4;
            break;
        }
        case EXCEPTION_ACTION_DESTROYBASE: {
            int offset, pointer;
            p = __DecodeSignedNumber(ap + 1, &pointer);
            p = __DecodeSignedNumber(p, &offset);
            iter->info.action_pointer = p + 4;
            break;
        }
        case EXCEPTION_ACTION_DESTROYMEMBER: {
            int offset, objectptr;
            p = __DecodeSignedNumber(ap + 1, &objectptr);
            p = __DecodeSignedNumber(p, &offset);
            iter->info.action_pointer = p + 4;
            break;
        }
        case EXCEPTION_ACTION_DESTROYMEMBERCOND: {
            int offset, objectptr, cond;
            p = __DecodeSignedNumber(ap + 1, &cond);
            p = __DecodeSignedNumber(p, &objectptr);
            p = __DecodeSignedNumber(p, &offset);
            iter->info.action_pointer = p + 4;
            break;
        }
        case EXCEPTION_ACTION_DESTROYMEMBERARRAY: {
            unsigned int element_size, elements;
            int offset, objectptr;
            p = __DecodeSignedNumber(ap + 1, &objectptr);
            p = __DecodeSignedNumber(p, &offset);
            p = __DecodeUnsignedNumber(p, &elements);
            p = __DecodeUnsignedNumber(p, &element_size);
            iter->info.action_pointer = p + 4;
            break;
        }
        case EXCEPTION_ACTION_DELETEPOINTER: {
            int objectptr;
            p = __DecodeSignedNumber(ap + 1, &objectptr);
            iter->info.action_pointer = p + 4;      // + operator delete
            break;
        }
        case EXCEPTION_ACTION_DELETEPOINTERCOND: {
            int objectptr, cond;
            p = __DecodeSignedNumber(ap + 1, &cond);
            p = __DecodeSignedNumber(p, &objectptr);
            iter->info.action_pointer = p + 4;
            break;
        }
        case EXCEPTION_ACTION_CATCHBLOCK: {
            int cinfo_ref;
            unsigned int catch_pcoffset;
            char *catch_type;
            p = GetUnalignedPointer(ap + 1, &catch_type);
            p = __DecodeUnsignedNumber(p, &catch_pcoffset);
            iter->info.action_pointer = __DecodeSignedNumber(p, &cinfo_ref);
            break;
        }
        case EXCEPTION_ACTION_ACTIVECATCHBLOCK: {
            int cinfo_ref;
            iter->info.action_pointer = __DecodeSignedNumber(ap + 1, &cinfo_ref);
            break;
        }
        case EXCEPTION_ACTION_SPECIFICATION: {
            int cinfo_ref;
            unsigned int pcoffset, specs;
            p = __DecodeUnsignedNumber(ap + 1, &specs);
            p = __DecodeUnsignedNumber(p, &pcoffset);
            p = __DecodeSignedNumber(p, &cinfo_ref);
            iter->info.action_pointer = p + specs * 4;  // + the type list
            break;
        }
        default:
            std::terminate();
        }
        break;
    }

    // follow branches
    while ((action = *iter->info.action_pointer & EXCEPTION_ACTION_MASK) == EXCEPTION_ACTION_BRANCH) {
        int target;
        __DecodeSignedNumber(iter->info.action_pointer + 1, &target);
        iter->info.action_pointer += target;
    }
    return action;
}

// Binary search of the exception table index for the function containing
// returnaddr.
static inline ExceptionTableIndex *FindIndex(ExceptionTableIndex *exceptionindex, ExceptionTableIndex *end, char *returnaddr)
{
    int i, m, n;
    ExceptionTableIndex *p;

    for (i = 0, n = (end - exceptionindex) - 1; i <= n;) {
        p = &exceptionindex[m = (i + n) >> 1];
        if (returnaddr < p->function_start)
            n = m - 1;
        else if (returnaddr > p->function_start + (p->eti_field & ~1))
            i = m + 1;
        else
            return p;
    }
    return 0;
}

// Find the exception table entry of the function containing a return
// address, then the action list that covers it.
static void FindExceptionRecord(char *returnaddr, ExceptionInfo *info)
{
    ExceptionTableIndex *p;
    unsigned int retoffset, offset, pcoffset, range, action;

    info->exception_record = 0;
    info->action_pointer = 0;
    if (!__FindExceptionTable(info, returnaddr))
        return;

    returnaddr = (char *)((int)returnaddr & ~1L);
    p = FindIndex(info->exception_table_start, info->exception_table_end, returnaddr);
    if (!p)
        return;

    if (p->eti_field & 1)
        info->exception_record = (char *)&p->exception_record;
    else
        info->exception_record = p->exception_record;
    info->current_function = p->function_start;
    retoffset = returnaddr - p->function_start;

    char *ap = __SkipUnwindInfo(info->exception_record);
    for (offset = 0;;) {
        ap = __DecodeUnsignedNumber(ap, &pcoffset);
        if (!pcoffset)
            return;
        ap = __DecodeUnsignedNumber(ap, &range);
        ap = __DecodeUnsignedNumber(ap, &action);
        offset += pcoffset;
        if (retoffset < offset)
            return;
        offset += range;
        if (retoffset <= offset) {
            info->action_pointer = info->exception_record + action;
            return;
        }
    }
}
