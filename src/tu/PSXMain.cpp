// PSXMain.cpp  @ 0x002729C0 (text 0x2729C0-0x272FB0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// main and the EE timer interrupt handlers as one translation unit, a TU
// without an initializer at the end of PSXCamera.cpp's tu_map row. The name is
// a guess that keeps the alphabetical link order (PSXInputDeviceType.cpp < it
// < PSXRenderTarget.cpp). Its .rodata (0x416FD0-0x4172E0) restarts the labels
// (@584) after PSXInputDeviceType.cpp's RTTI. It is not PSXRenderTarget.cpp's
// head: SyncDmaPath, next in .text, ASSERTs at PSXRenderTarget.cpp line 103,
// and this TU's .bss (GameInitialized .. __argv, 0x4745A0-0x4745C8) precedes
// PSXRenderTarget.cpp's axis vectors. `make link` places its data as well as
// its code.



#include "JetskiGame/JetskiGame.h"

// ---- src/global/main.cpp
extern "C" {
void mwInit(void);
void mwExit(void);
int scePrintf(const char *fmt, ...);
int sprintf(char *buf, const char *fmt, ...);
char *strcpy(char *dst, const char *src);
void exit(int status);
int AddIntcHandler(int cause, int (*handler)(int), int next);
int EnableIntc(int cause);
void sceSifInitRpc(unsigned int mode);
int sceCdInit(int mode);
int sceCdMmode(int media);
int sceSifRebootIop(const char *img);
int sceSifSyncIop(void);
int sceFsReset(void);
int sceSifLoadModule(const char *name, int argLength, const char *args);
int sceSifInitIopHeap(void);
int scePadInit(int mode);
int sceMtapInit(void);
int sceMtapPortOpen(int port);
int sceMtapGetConnection(int port);
int sceMcInit(void);
void sceGsResetGraph(short mode, short interlace, short omode, short ffmode);
void sceGsResetPath(void);
void sceDmaReset(int mode);
void sceVpu0Reset(void);
void sceDevVu0Reset(void);
void sceDevVu1Reset(void);
int sceDevVif1Reset(void);
void sceIpuInit(void);

}

// The globals (.bss 0x4745A0-0x4745C8, in retail's order).
int GameInitialized;
int TimerOverflowCounterT0;
int TimerOverflowCounterT1;
extern "C" {
int __argc;
char **__argv;
}

void PSXprintf(const char *fmt, ...);

int TimerHandlerT0(int cause);
int TimerHandlerT1(int cause);

#define T0_MODE ((volatile unsigned int *)0x10000010)
#define T1_MODE ((volatile unsigned int *)0x10000810)
#define IPU_CTRL ((volatile unsigned int *)0x10002010)

extern JetskiGame *LpGame;

extern "C" int main(int argc, char **argv)
{
    char path[0x110];
    char prefix[0x110];
    char suffix[0x10];
    char image[0x110];
    char commandLine[0x100];

    mwInit();
    scePrintf("\n\n------\nEnter executable\n------\n\n");
    __argc = argc;
    __argv = argv;
    AddIntcHandler(9, TimerHandlerT0, 0);
    EnableIntc(9);
    AddIntcHandler(10, TimerHandlerT1, 0);
    EnableIntc(10);
    *T0_MODE = 0;
    *T0_MODE |= 2;
    *T0_MODE |= 0x80;
    *T0_MODE |= 0x200;
    *T1_MODE = 0;
    *T1_MODE |= 2;
    *T1_MODE |= 0x80;
    *T1_MODE |= 0x200;
    PSXprintf("Begin Hardware Initialization . . .\n");
    sceSifInitRpc(0);
    sceCdInit(0);
    strcpy(image, "cdrom0:\\IOPRP214.IMG;1");
    while (!sceSifRebootIop(image))
        ;
    while (!sceSifSyncIop())
        ;
    sceCdInit(0);
    sceCdMmode(2);
    sceFsReset();
    strcpy(prefix, "cdrom0:\\");
    strcpy(suffix, ";1");
    sprintf(path, "%smem2MB.irx%s", prefix, suffix);
    if (sceSifLoadModule(path, 0, 0) < 0)
        scePrintf("Can't load module mem2MB.irx\n");
    sprintf(path, "%sSIO2MAN.IRX%s", prefix, suffix);
    if (sceSifLoadModule(path, 0, 0) < 0)
        scePrintf("Can't load module sio2man\n");
    sprintf(path, "%sPADMAN.IRX%s", prefix, suffix);
    if (sceSifLoadModule(path, 0, 0) < 0)
        scePrintf("Can't load module padman\n");
    sprintf(path, "%sMCMAN.IRX%s", prefix, suffix);
    if (sceSifLoadModule(path, 0, 0) < 0)
        scePrintf("Can't load module mcman\n");
    sprintf(path, "%sMCSERV.IRX%s", prefix, suffix);
    if (sceSifLoadModule(path, 0, 0) < 0)
        scePrintf("Can't load module mcserv\n");
    sprintf(path, "%sMTAPMAN.IRX%s", prefix, suffix);
    if (sceSifLoadModule(path, 0, 0) < 0)
        scePrintf("Can't load module mtapman\n");
    sprintf(path, "%sLIBSD.IRX%s", prefix, suffix);
    if (sceSifLoadModule(path, 0, 0) < 0)
        scePrintf("Can't load module libsd\n");
    sprintf(path, "%sPS2SOUND.IRX%s", prefix, suffix);
    if (sceSifLoadModule(path, 0, 0) < 0)
        scePrintf("Can't load module ps2sound\n");
    sceSifInitIopHeap();
    scePadInit(0);
    sceMtapInit();
    sceMtapPortOpen(0);
    sceMtapPortOpen(1);
    if (sceMtapGetConnection(0))
        PSXprintf("MultiTap in port 1 detected\n");
    else
        PSXprintf("MultiTap in port 1 not detected\n");
    if (sceMtapGetConnection(1))
        PSXprintf("MultiTap in port 2 detected\n");
    else
        PSXprintf("MultiTap in port 2 not detected\n");
    if (!sceMcInit())
        PSXprintf("Memory Card Initialized!!\n");
    sceGsResetGraph(0, 1, 2, 1);
    sceGsResetPath();
    sceDmaReset(1);
    sceVpu0Reset();
    sceDevVu0Reset();
    sceDevVu1Reset();
    sceDevVif1Reset();
    sceIpuInit();
    *IPU_CTRL = 0x40000000;
    PSXprintf("End Hardware Initialization . . .\n");
    if (!LpGame->PreGameInitialize())
        exit(0);
    commandLine[0] = 0;
    if (!LpGame->Initialize(commandLine))
        exit(-1);
    GameInitialized = 1;
    while (LpGame->Tick())
        ;
    LpGame->Terminate();
    mwExit();
    return 1;
}


// ---- src/global/TimerHandlerT1.cpp
// Timer 1 overflow interrupt handler: counts overflows, then acknowledges
// the interrupt (OVFF bit in T1_MODE) and re-enables interrupts.

int TimerHandlerT1(int)
{
    TimerOverflowCounterT1++;
    *(volatile unsigned int *)0x10000810 |= 0x200;  // T1_MODE: clear overflow flag
    asm("sync");
    asm("ei");
    return 0;
}


// ---- src/global/TimerHandlerT0.cpp
// Timer 0 overflow interrupt handler: counts overflows, then acknowledges
// the interrupt (OVFF bit in T0_MODE) and re-enables interrupts.

int TimerHandlerT0(int)
{
#ifdef SPLASHDOWN_PC
    // the timer thread, beside the game thread's read-and-clear
    __atomic_add_fetch(&TimerOverflowCounterT0, 1, __ATOMIC_SEQ_CST);
#else
    TimerOverflowCounterT0++;
#endif
    *(volatile unsigned int *)0x10000010 |= 0x200;  // T0_MODE: clear overflow flag
    asm("sync");
    asm("ei");
    return 0;
}


