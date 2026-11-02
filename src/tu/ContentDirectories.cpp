// ContentDirectories.cpp  @ 0x0040C980 (no text; data only)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// The content directories as one translation unit: a
// data-only TU, the MAX_PATH buffers that JetskiGame::Initialize fills
// ("Anims\\", "Scenes\\Geometry\\", ...) and every loader prefixes to its
// file names. Their .data (0x40C980-0x40DDA4) sits between WaterObject.cpp's
// and MicroProgram.cpp's; none of the TUs between those two in link order
// (JetskiUIWidgets, JetskiRider, WaterWrecker, RainbowPS2Snd) uses most of
// them, so they are taken to be a TU of their own. The file name is a guess.
// The empty initializers put them in .data, where retail has them.

char RootDirectory[0x104] = "";
char AnimContentDirectory[0x104] = "";
char AudioContentDirectory[0x104] = "";
char CollisionContentDirectory[0x104] = "";
char DebugOverlayContentDirectory[0x104] = "";
char EcoGeometryContentDirectory[0x104] = "";
char EcoTextureContentDirectory[0x104] = "";
char FontContentDirectory[0x104] = "";
char ParticleTextureContentDirectory[0x104] = "";
char SceneContentDirectory[0x104] = "";
char SkyContentDirectory[0x104] = "";
char SltGeometryContentDirectory[0x104] = "";
char SltTextureContentDirectory[0x104] = "";
char TerrainTextureContentDirectory[0x104] = "";
char UIContentDirectory[0x104] = "";
char UITextureContentDirectory[0x104] = "";
char LocalizationContentDirectory[0x104] = "Loc\\";
char BaseWaterContentDirectory[0x104] = "";
char WaterContentDirectory[0x104] = "";
