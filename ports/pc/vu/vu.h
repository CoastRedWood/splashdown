// VIF1 and VU1 on the PC (ports/pc/vu/). The VIF1 decoder takes the DMA
// channel's words (ports/pc/dma.cpp), unpacks into VU1 data memory, uploads
// microcode (MPG) into VU1 micro memory and starts it (MSCAL/MSCNT/MSCALF);
// VU1 runs the real microprogram in an interpreter (ports/pc/vu/vu1.cpp) whose
// XGKICKs go to the GS's PATH1. DIRECT/DIRECTHL go to PATH2.
#ifndef PORT_VU_H
#define PORT_VU_H

#include <cstddef>
#include <cstdint>

// VIF1: feed n words of the channel's stream (tag halves under TTE, data).
void vif1_write(const uint32_t *words, size_t n);

// VU1
struct Vu1 {
    alignas(16) uint8_t data[16384];
    uint64_t micro[2048];
    uint32_t pc;          // where MSCNT continues
    uint32_t top, itop;   // what XTOP/XITOP read
};
extern Vu1 g_vu1;

// Run VU1 from pc (in instructions) until an [E] bit.
void vu1_run(uint32_t pc);
// Instructions run so far.
uint64_t vu1_instructions();

// The microprogram uploaded to micro memory around pc, for logs.
const char *vu1_program_at(uint32_t pc);
// Micro memory changed (an MPG wrote different words): the recompiler's
// blocks are checked against it again.
void vu1_micro_changed();
// Record an MPG upload of n instructions at addr whose data came from src.
void vu1_note_upload(uint32_t addr, uint32_t n, const void *src);

#endif
