// MicroProgram.cpp  @ 0x003367C0 (text 0x3367C0-0x336FD0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's MicroProgram.cpp as one translation unit: the microprogram and VU1
// offset string tables, unanchored inside WaterWrecker.cpp's tu_map row after
// RainbowPS2Snd's code. Its .rodata (@345..@414, ending with the StringTable,
// Vu1OffsetStringTable and MicroProgramStringTable RTTI at 0x434868) restarts
// the labels after RainbowPS2Snd's; StringTable.cpp (@409..) follows.
// __sinit_MicroProgram.cpp constructs GMicroProgramStringTable and
// GVu1OffsetStringTable. `make link` places its data as well as its code.



#include "StringTable/StringTable.h"

// Names of VU microprograms, each entry's pointer the microcode blob.
class MicroProgramStringTable : public StringTable {
public:
    MicroProgramStringTable();
    virtual ~MicroProgramStringTable() {}
};

// Names of VU1 micro-memory slots; each entry's pointer is its qword offset.
class Vu1OffsetStringTable : public StringTable {
public:
    Vu1OffsetStringTable();
    virtual ~Vu1OffsetStringTable() {}
};

MicroProgramStringTable GMicroProgramStringTable;
Vu1OffsetStringTable GVu1OffsetStringTable;
StringTable *LpMicroProgramStringTable = &GMicroProgramStringTable;
StringTable *LpVu1OffsetStringTable = &GVu1OffsetStringTable;

// VU microprograms (assembled microcode blobs).
extern char KickGSCode[];
extern char EcoSystemCode2D[];
extern char TerrainRendererClipCode[];
extern char StripRendererCode[];
extern char ProjectedStripRendererCode[];
extern char AnimShadowCastCode[];
extern char ParticleCode[];
extern char ProjectedShadowCode[];
extern char StripPreambleCode[];
extern char ScissorCoreCode[];
extern char StripScisRendererCode[];
extern char StripLitCode[];
extern char StripFogCullCode[];
extern char StripOptTexXformCode[];
extern char StripOptFogEmbossShiftedCode[];
extern char StripUnlitCode[];
extern char StripPrelitCode[];
extern char EnvmapCode[];
extern char Envmap2Code[];
extern char SpecularMapCode[];
extern char SpecularMap2Code[];
extern char ProjtexLitCode[];
extern char TerrainScisPreambleCode[];
extern char TerrainScissorCode[];
extern char TerrainShadowScisCode[];
extern char TerrainRendererHalfResCode[];
extern char TerrainScisHalfResCode[];
extern char TerrainShadowHalfResCode[];
extern char Quad2DRendererCode[];
extern char ProjtexPrelitCode[];
extern char StripDiffuseAndPrelitCode[];
extern char StripWireframeCode[];
extern char StripOptFogCodeNoKick[];
extern char NopLoop[];
extern char ParticlePhysicsCode[];
extern char StripLitCompressedCode[];
extern char StripPrelitCompressedCode[];
extern char Envmap2CompressedCode[];
extern char SpecularMapCompressedCode[];
extern char StripTexTransCompressedCode[];
extern char CubeSubdivCode[];
extern char WaterScreenSpaceRendererCode[];
extern char ShadowReceiverCode[];

// ---- src/MicroProgramStringTable/ctor.cpp

// Names the renderers look microprograms up by. Retail maps StripOptCode and
// StripOptFogCode to StripLitCode as well.
MicroProgramStringTable::MicroProgramStringTable()
{
    AddItem("KickGSCode", KickGSCode);
    AddItem("EcoSystemCode2D", EcoSystemCode2D);
    AddItem("TerrainRendererClipCode", TerrainRendererClipCode);
    AddItem("StripRendererCode", StripRendererCode);
    AddItem("ProjectedStripRendererCode", ProjectedStripRendererCode);
    AddItem("AnimShadowCastCode", AnimShadowCastCode);
    AddItem("ParticleCode", ParticleCode);
    AddItem("ProjectedShadowCode", ProjectedShadowCode);
    AddItem("StripPreambleCode", StripPreambleCode);
    AddItem("ScissorCoreCode", ScissorCoreCode);
    AddItem("StripScisRendererCode", StripScisRendererCode);
    AddItem("StripLitCode", StripLitCode);
    AddItem("StripOptCode", StripLitCode);
    AddItem("StripOptFogCode", StripLitCode);
    AddItem("StripFogCullCode", StripFogCullCode);
    AddItem("StripOptTexXformCode", StripOptTexXformCode);
    AddItem("StripOptFogEmbossShiftedCode", StripOptFogEmbossShiftedCode);
    AddItem("StripUnlitCode", StripUnlitCode);
    AddItem("StripPrelitCode", StripPrelitCode);
    AddItem("EnvmapCode", EnvmapCode);
    AddItem("Envmap2Code", Envmap2Code);
    AddItem("SpecularMapCode", SpecularMapCode);
    AddItem("SpecularMap2Code", SpecularMap2Code);
    AddItem("ProjtexLitCode", ProjtexLitCode);
    AddItem("TerrainScisPreambleCode", TerrainScisPreambleCode);
    AddItem("TerrainScissorCode", TerrainScissorCode);
    AddItem("TerrainShadowScisCode", TerrainShadowScisCode);
    AddItem("TerrainRendererHalfResCode", TerrainRendererHalfResCode);
    AddItem("TerrainScisHalfResCode", TerrainScisHalfResCode);
    AddItem("TerrainShadowHalfResCode", TerrainShadowHalfResCode);
    AddItem("Quad2DRendererCode", Quad2DRendererCode);
    AddItem("Diffuse", StripLitCode);
    AddItem("DiffuseTexTrans", StripOptTexXformCode);
    AddItem("Prelit", StripPrelitCode);
    AddItem("SpecularMap", SpecularMapCode);
    AddItem("EnvironmentMap", Envmap2Code);
    AddItem("ProjectedTexture", ProjtexLitCode);
    AddItem("ProjectedPrelit", ProjtexPrelitCode);
    AddItem("DiffuseAndPrelit", StripDiffuseAndPrelitCode);
    AddItem("StripWireframe", StripWireframeCode);
    AddItem("StripOptFogCodeNoKick", StripOptFogCodeNoKick);
    AddItem("NopLoop", NopLoop);
    AddItem("ParticlePhysics", ParticlePhysicsCode);
    AddItem("DiffuseCompressed", StripLitCompressedCode);
    AddItem("PrelitCompressed", StripPrelitCompressedCode);
    AddItem("EnvironmentMapCompressed", Envmap2CompressedCode);
    AddItem("SpecularMapCompressed", SpecularMapCompressedCode);
    AddItem("DiffuseTexTransCompressed", StripTexTransCompressedCode);
    AddItem("CubeSubdiv", CubeSubdivCode);
    AddItem("Water", WaterScreenSpaceRendererCode);
    AddItem("ShadowReceiver", ShadowReceiverCode);
}


// ---- src/Vu1OffsetStringTable/ctor.cpp

Vu1OffsetStringTable::Vu1OffsetStringTable()
{
    AddItem("TexGenMatrixRow1", (void *)0x1C);
    AddItem("TexGenMatrixRow2", (void *)0x1D);
    AddItem("TexGenMatrixRow3", (void *)0x1E);
    AddItem("TexGenMatrixRow4", (void *)0x1F);
}

