// MSL C++ runtime: exception-handling structures of the PS2 (MIPS) port,
// reconstructed from the retail code that uses them.
#ifndef STD_NMWEXCEPTION_H
#define STD_NMWEXCEPTION_H

typedef int GPR_Type __attribute__((mode(TI)));     // 128-bit EE register
typedef unsigned int FPR_Type;                  // raw bits of an FP register

struct CatchInfo {
    void *location;
    void *typeinfo;
    void *dtor;
    void *sublocation;
    int pointercopy;                // 32-bit here (MSL: long)
    void *stacktop;
};

// Built on the stack by __throw (hand-written asm), then updated frame by
// frame while the stack is unwound.
struct ThrowContext {
    char *throwtype;                // +0x000
    void *location;                 // +0x004
    void *dtor;                     // +0x008
    CatchInfo *catchinfo;           // +0x00C
    char *returnaddr;               // +0x010
    char *throwSP;                  // +0x014: SP of the current frame
    char *frameBase;                // +0x018: its FP, or SP if it has none
    char *targetSP;                 // +0x01C: SP restored by __TransferControl
    GPR_Type GPR[32];               // +0x020
    unsigned int frameSize;         // +0x220
    unsigned int savedRegOffset;    // +0x224: from frameBase to the saved registers
    int savedGPRs;                  // +0x228: count of $s registers saved
    unsigned int returnSaved;       // +0x22C: frame saved $ra (flag 0x10)
    unsigned int usesFP;            // +0x230: frame uses $fp (flag 0x20)
    unsigned int savedFPRs;         // +0x234: count of FP registers saved
    FPR_Type FPR[32];               // +0x238
};

struct ExceptionTableIndex {
    char *function_start;
    unsigned int eti_field;     // bit 0: record stored inline; rest: function size
    char *exception_record;
};

struct ExceptionInfo {
    char *current_function;                     // +0x00
    char *exception_record;                     // +0x04
    char *action_pointer;                       // +0x08
    ExceptionTableIndex *exception_table_start; // +0x0C
    ExceptionTableIndex *exception_table_end;   // +0x10
    void *reserved;                             // +0x14: copied, never read here
};

// Walks a frame's action list (NextAction) with its own copy of the context.
struct ActionIterator {
    ExceptionInfo info;                         // +0x00
    ThrowContext context;                       // +0x20 (16-byte aligned)
};

typedef unsigned char exaction_type;

enum {
    EXCEPTION_ACTION_ENDOFLIST,                 // 0
    EXCEPTION_ACTION_BRANCH,                    // 1
    EXCEPTION_ACTION_DESTROYLOCAL,              // 2
    EXCEPTION_ACTION_DESTROYLOCALCOND,          // 3
    EXCEPTION_ACTION_DESTROYLOCALPOINTER,       // 4
    EXCEPTION_ACTION_DESTROYLOCALARRAY,         // 5
    EXCEPTION_ACTION_DESTROYBASE,               // 6
    EXCEPTION_ACTION_DESTROYMEMBER,             // 7
    EXCEPTION_ACTION_DESTROYMEMBERCOND,         // 8
    EXCEPTION_ACTION_DESTROYMEMBERARRAY,        // 9
    EXCEPTION_ACTION_DELETEPOINTER,             // 10
    EXCEPTION_ACTION_DELETEPOINTERCOND,         // 11
    EXCEPTION_ACTION_CATCHBLOCK,                // 12
    EXCEPTION_ACTION_ACTIVECATCHBLOCK,          // 13
    EXCEPTION_ACTION_TERMINATE,                 // 14
    EXCEPTION_ACTION_SPECIFICATION              // 15
};

#define EXCEPTION_ACTION_MASK 0x1F

char *__DecodeUnsignedNumber(char *p, unsigned int *pnumber);
char *__DecodeSignedNumber(char *p, int *pnumber);
char *__SkipUnwindInfo(char *p);

#endif
