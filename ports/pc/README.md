# PC port (SDL3)

The game's own code, unchanged from the matching build, compiled with clang and running on a PC version of the hardware it was written for. That hardware layer is the retail VU1 microcode in a recompiler that models the VU pipeline, a multithreaded software GS (AVX2) presented through SDL3, the IPU, PSS movies through libavcodec, a PC version of the IOP sound driver mixing to SDL3 audio, gamepads and the keyboard as DualShock 2 pads, and the memory card as a directory.

Target: **Linux x86-64** (✅ whole game at 30 FPS).

## Build and run

```sh
sudo pacman -S clang sdl3 ffmpeg zlib   # ffmpeg is optional: without it movies are skipped
make link                               # once: the port takes the VU microcode packets from it
python3 tools/port/build.py -j 8        # builds build/port/splashdown (--asan for an ASan build)
SPLASHDOWN_DISC=/path/to/splashdown.bin build/port/splashdown
```

The disc image defaults to `baserom/splashdown.bin`. Saves go to `~/.local/share/splashdown/memcard0/` (`PORT_MEMCARD_DIR` to move them). A window opens when `WAYLAND_DISPLAY` or `DISPLAY` is set. Without either, the game runs headless, which the test tools use (`PORT_SCREENSHOT`, `PORT_INPUT`, `tools/port/golden.py`).

## Controls

Any SDL3 gamepad (Xbox, PlayStation, ...) in the standard layout works: the first one is player 1 and a second one is player 2 for Vs. Analog triggers and rumble work. Xbox A/B/X/Y are Cross/Circle/Square/Triangle, Menu is Start and View is Select.

Keyboard: arrows (D-pad), WASD (left stick), IJKL (right stick), X or Space (Cross), C or Esc (Circle), Z (Square), V (Triangle), Enter (Start), Tab (Select), Q/E (L1/R1), 1/3 (L2/R2).

## Layout

```
main.cpp, runtime.cpp   start-up, the game's thread, the port heap
ee/                     the EE inline-asm / VU0 macro interpreter
vu/                     VIF1, VU1 interpreter and recompiler
gs/                     software GS rasterizer
dma.cpp, hw.cpp         DMA channels and hardware registers
ipu.cpp, video.cpp      IPU decoding, presentation through SDL3/OpenGL
input.cpp               SDL3 gamepads and the keyboard
audio/                  the IOP sound driver, mixed to SDL3 audio
sdk/                    Sony SDK replacements: CD/DVD, pads, memory card, MPEG, sound RPC
game/                   game-side code the PS2 build has as asm (C versions)
include/                prelude, 32-bit pointers, the asm interface
```
