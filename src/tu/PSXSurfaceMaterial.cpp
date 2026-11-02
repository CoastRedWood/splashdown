// PSXSurfaceMaterial.cpp  @ 0x00345880 (text 0x345880-0x347ED0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's PSXSurfaceMaterial.cpp as one translation unit: UVAnimationData,
// AnimatedUVCallback, PSXShaderDescriptor, MaterialAnimationData and
// PSXSurfaceMaterial, then the static initializer (the axis vectors). Its
// .rodata ends with the PSXSurfaceMaterial and PSXShaderDescriptor RTTI
// (0x43651C). `make link` places the object's data as well as its code
// (tools/link_c.py). The rest of the tu_map row is more TUs:
// PSXSurfaceShader.cpp (0x347ED0-0x349DD0, src/tu), PSXSurfaceGeometry.cpp
// (0x349DD0-0x34A5D0, src/tu, no initializer), then SurfaceMaterial,
// "SurfaceShader.cpp", SurfaceGeometry and SoundEmitters.cpp
// (0x34A5D0-0x34DC00, per-function). UVAnimationData is TU-local (no header).




#include "d3dtypes.h"
#include "ParameterBlock/ParameterBlock.h"
#include "PSXShaderDescriptor/PSXShaderDescriptor.h"
#include "MaterialAnimationData/MaterialAnimationData.h"
#include "TextureMapStringTable/TextureMapStringTable.h"
#include "soultree_assert_block.h"
#include "QWORD/QWORD.h"
#include "StringTable/StringTable.h"
#include "sceGifTag/sceGifTag.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "SurfaceMaterial/SurfaceMaterial.h"
#include "PSXSurfaceMaterial/PSXSurfaceMaterial.h"
#include "TextureMap/TextureMap.h"
#include "ShaderDescriptor/ShaderDescriptor.h"
#include "PSXSurfaceShaderInstance/PSXSurfaceShaderInstance.h"
#include "SurfaceShaderInstance/SurfaceShaderInstance.h"
#include "PSXTextureMapManager/PSXTextureMapManager.h"
#include "PSXSurfaceGeometry/PSXSurfaceGeometry.h"
#include "PSXSurfaceShaderList/PSXSurfaceShaderList.h"
#include "SurfaceShaderList/SurfaceShaderList.h"
#include "DmaRenderItem/DmaRenderItem.h"
#include "_sceDmaTag/_sceDmaTag.h"
#include "DataAccess/DataAccess.h"
#include "SurfaceGeometry/SurfaceGeometry.h"
#include "MaterialStringTable/MaterialStringTable.h"
#include "PSXTextureMap/PSXTextureMap.h"
#include "Game/Game.h"
#include "Water/Water.h"
#include "PSXWater/PSXWater.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "PSXCamera/PSXCamera.h"
#include "LightManager/LightManager.h"
#include "VifDataItem/VifDataItem.h"
#include "sinit/AxisVectors.h"

// Defined here for its retail .bss slot.
static int ShowDepthComplexity;
static int DebugFill;


// A texture-coordinate animation (MaterialAnimationData::m_uvAnimation,
// 0x70 bytes): the texgen matrix rows, their inverse, and the frame counter.
class UVAnimationData {
public:
    UVAnimationData(ParameterBlock *params);
    int NextFrame();
    void NextTexGenMatrix(QWORD *out, QWORD *in);

    QWORD m_matrix[3];              // +0x00, texgen rows 1-3
    QWORD m_altMatrix[3];           // +0x30, inverse of the 2D part
    int m_frame;                    // +0x60
    int m_numFrames;                // +0x64
    unsigned char m_pingPong : 1;   // +0x68 bit 0
    unsigned char m_loop : 1;       //        bit 1
    unsigned char m_reverse : 1;    //        bit 2, playing backwards: NextTexGenMatrix uses m_altMatrix
    unsigned char m_finished : 1;   //        bit 3
};

// ---- src/UVAnimationData/ctor.cpp

extern "C" double cos(double x);
extern "C" double sin(double x);


extern _D3DVECTOR XVector;
extern _D3DVECTOR YVector;
extern _D3DVECTOR ZVector;



UVAnimationData::UVAnimationData(ParameterBlock *params)
{
    m_frame = 0;
    _D3DVECTOR row;
    // y and z are read as (&row.x)[1] and [2]: with row.y/row.z mwcc CSEs their
    // addresses into the default-ctor stores, which retail does not.
    if (params->GetParameter("TexGenMatrixRow1", &XVector, &row)) {
        m_matrix[0].f[0] = row.x;
        m_matrix[0].f[1] = (&row.x)[1];
        m_matrix[0].f[2] = (&row.x)[2];
        m_matrix[0].f[3] = 0;
        params->GetParameter("TexGenMatrixRow2", &YVector, &row);
        m_matrix[1].f[0] = row.x;
        m_matrix[1].f[1] = (&row.x)[1];
        m_matrix[1].f[2] = (&row.x)[2];
        m_matrix[1].f[3] = 0;
        params->GetParameter("TexGenMatrixRow3", &ZVector, &row);
        m_matrix[2].f[0] = row.x;
        m_matrix[2].f[1] = (&row.x)[1];
        m_matrix[2].f[2] = (&row.x)[2];
        m_matrix[2].f[3] = 0;
    } else {
        m_matrix[0].m_value = 0;
        m_matrix[1].m_value = 0;
        m_matrix[2].m_value = 0;
        m_matrix[0].f[0] = 1.0f;
        m_matrix[1].f[1] = 1.0f;
        m_matrix[2].f[2] = 1.0f;
    }

    float center[2];
    float uVelocity = 0;
    float vVelocity = 0;
    float uScale = 1.0f;
    float vScale = 1.0f;
    float uShear = 0;
    float vShear = 0;
    float angularRate = 0;
    float sinA = 0;
    float cosA = 1.0f;
    int flag;
    params->GetParameter("u_center", 0.0f, &center[0]);
    params->GetParameter("v_center", 0.0f, &center[1]);
    if (params->GetParameter("uv_angular_rate", 0.0f, &angularRate)) {
        cosA = cos(angularRate);
        sinA = sin(angularRate);
    }
    params->GetParameter("u_scale", 1.0f, &uScale);
    params->GetParameter("v_scale", 1.0f, &vScale);
    params->GetParameter("u_shear", 0.0f, &uShear);
    params->GetParameter("v_shear", 0.0f, &vShear);
    m_matrix[0].f[0] = uScale * cosA - vShear * sinA;
    m_matrix[0].f[1] = uShear * cosA - vScale * sinA;
    m_matrix[1].f[0] = vShear * cosA + uScale * sinA;
    m_matrix[1].f[1] = vScale * cosA + uShear * sinA;
    m_matrix[2].f[0] = center[0] * (cosA - uScale) - center[1] * sinA;
    m_matrix[2].f[1] = center[1] * (cosA - vScale) + center[0] * sinA;
    params->GetParameter("u_velocity", 0.0f, &uVelocity);
    m_matrix[2].f[0] -= uVelocity;
    params->GetParameter("v_velocity", 0.0f, &vVelocity);
    m_matrix[2].f[1] -= vVelocity;
    params->GetParameter("uv_frames", 1, &m_numFrames);
    params->GetParameter("uv_Looping", 0, &flag);
    m_loop = flag;
    params->GetParameter("uv_ReverseLoopOnEnd", 0, &flag);
    m_pingPong = flag;
    m_reverse = 0;
    m_finished = 0;

    // Inverse of the 2D part.
    m_altMatrix[0].m_value = 0;
    m_altMatrix[1].m_value = 0;
    m_altMatrix[2].m_value = 0;
    float invDet = 1.0f / (m_matrix[0].f[0] * m_matrix[1].f[1] - m_matrix[1].f[0] * m_matrix[0].f[1]);
    m_altMatrix[0].f[0] = invDet * m_matrix[1].f[1];
    m_altMatrix[0].f[1] = -invDet * m_matrix[0].f[1];
    m_altMatrix[1].f[0] = -invDet * m_matrix[1].f[0];
    m_altMatrix[1].f[1] = invDet * m_matrix[0].f[0];
    m_altMatrix[2].f[0] = -m_matrix[2].f[0];
    m_altMatrix[2].f[1] = -m_matrix[2].f[1];
    m_altMatrix[2].f[2] = 1.0f;
}


// ---- src/UVAnimationData/NextFrame.cpp

int UVAnimationData::NextFrame()
{
    if (m_reverse)
        m_frame--;
    else
        m_frame++;
    if (m_loop) {
        if (m_pingPong) {
            if (m_frame <= 0) {
                m_reverse = 0;
            } else if (m_frame >= m_numFrames) {
                m_reverse = 1;
                m_frame = m_numFrames - 2;
            }
        } else if (m_frame >= m_numFrames) {
            m_frame = 0;
        }
    }
    return m_frame;
}


// ---- src/UVAnimationData/NextTexGenMatrix.cpp


// out = in * matrix (3x3, rows are QWORDs).
void UVAnimationData::NextTexGenMatrix(QWORD *out, QWORD *in)
{
    QWORD *m = m_reverse ? m_altMatrix : m_matrix;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++)
            out[i].f[j] = in[i].f[0] * m[0].f[j] + in[i].f[1] * m[1].f[j] + in[i].f[2] * m[2].f[j];
    }
}


// ---- src/global/AnimatedUVCallback.cpp
#pragma fast_fptosi on



// Retail's fabs: abs.s into $f8, stored through a stack temporary.
static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}



// Material callback: advance a UV animation and write its texgen matrix rows
// (identity when not animated). Returns the animation's finished flag.
int AnimatedUVCallback(void *context, MaterialAnimationData *data)
{
    UVAnimationData *uv = data->m_uvAnimation;
    if (!uv->m_frame) {
        data->m_parameters[0]->m_value = 0;
        data->m_parameters[1]->m_value = 0;
        data->m_parameters[2]->m_value = 0;
        data->m_parameters[0]->f[0] = 1.0f;
        data->m_parameters[1]->f[1] = 1.0f;
        data->m_parameters[2]->f[2] = 1.0f;
        uv->NextFrame();
        return uv->m_finished;
    }
    QWORD out[3];
    QWORD in[3];
    in[0] = *data->m_parameters[0];
    in[1] = *data->m_parameters[1];
    in[2] = *data->m_parameters[2];
    uv->NextTexGenMatrix(out, in);
    if (!(Fabs(out[2].f[0]) <= 5.0f))
        out[2].f[0] -= (int)out[2].f[0];
    if (!(Fabs(out[2].f[1]) <= 5.0f))
        out[2].f[1] -= (int)out[2].f[1];
    for (int i = 0; i < 3; i++) {
        out[i].f[3] = 0.0f;
        *data->m_parameters[i] = out[i];
    }
    uv->NextFrame();
    return uv->m_finished;
}


// ---- src/PSXShaderDescriptor/ctor.cpp


PSXShaderDescriptor::PSXShaderDescriptor()
{
    m_microProgram = -1;
    m_enabled = 1;
    m_numTextures = 0;
    m_textures[0] = -1;
    m_textures[1] = -1;
    m_texCoords[0] = 0;
    m_texCoords[1] = 0;
    m_textureColorComponent = 0;
    m_textureFunction = 0;
    m_fillTexture = 0;
    m_fillColor = 0xFF808080;
    m_packet = 0;
    m_tex0 = 0;
    m_animation = 0;
}


// ---- src/PSXShaderDescriptor/dtor.cpp
extern "C" void free(void *p);


// Absolute load, so declared const.
extern TextureMapStringTable *const LpTextureMapStringTable;



PSXShaderDescriptor::~PSXShaderDescriptor()
{
    if (m_packet)
        free(m_packet);
    if (m_animation) {
        delete m_animation;
        m_animation = 0;
    }
    if (m_textures[0] != -1)
        LpTextureMapStringTable->RevokeRequest(m_textures[0]);
    if (m_textures[1] != -1)
        LpTextureMapStringTable->RevokeRequest(m_textures[1]);
}


// ---- src/PSXShaderDescriptor/ReadFromFile_00346130.cpp

extern "C" void *memalign(unsigned int align, unsigned int size);
extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" int strcasecmp(const char *a, const char *b);




extern StringTable *LpMicroProgramStringTable;


// The shader's DMA packet: a REF tag pointing at the VIF data after it.
struct ShaderPacket {
    unsigned short qwc;                 // +0x00
    unsigned char mark;                 // +0x02
    unsigned char id;                   // +0x03
    PS2_PTR(QWORD) next;                        // +0x04
    unsigned int pad[2];                // +0x08
    QWORD data[0x10];                   // +0x10, 0x10 in ReadFromFile(char *); more in a pass packet
};


// Reads one ShaderPass_N block of a .psg.
int PSXShaderDescriptor::ReadFromFile(char *path, ParameterBlock *block, int pass, int firstPass)
{
    char blockName[0x40];
    char key[0x40];
    char program[0x40];
    char texture[0x40];
    char fullName[0x108];
    int alphaBlend = 0;
    sprintf(blockName, "%s%d", "ShaderPass_", pass + 1);
    block->ReadParameterBlock(blockName);
    block->GetParameter("microProgramName", "Diffuse", program, 0x40);
    if (firstPass == 1 && !strcasecmp(program, "Diffuse")) {
        int blending = 10;
        block->GetParameter("alphaBlendingOnOff", blending, &blending);
        if (blending == 10)
            alphaBlend = 1;
    }
    m_microProgram = LpMicroProgramStringTable->GetIndex(program);
    int enabled = 1;
    block->GetParameter("EnableThisShader", 1, &enabled);
    m_enabled = enabled;
    int numTextures;
    block->GetParameter("totalTextureMaps", 1, &numTextures);
    m_numTextures = numTextures;
    for (int i = 0; i < m_numTextures; i++) {
        sprintf(key, "%s%d", "TextureMap_", i + 1);
        block->GetParameter(key, "", texture, -1);
        if (!texture[0]) {
#line 339 "PSXSurfaceMaterial.cpp"
            ASSERTF(0, (".psg has no texture, but totalTextureMaps = %d", numTextures));
        }
        sprintf(fullName, "%s%s", path, texture);
        m_textures[i] = LpTextureMapStringTable->AddItem(fullName, 0);
        LpTextureMapStringTable->Request(m_textures[i]);
        sprintf(key, "%s%d", "TextureCoordinateIndex_", i + 1);
        int index;
        block->GetParameter(key, 0, &index);
        m_texCoords[0] = index;
    }
    if (!m_numTextures) {
        int fillColor = 0;
        block->GetParameter("EnableFillColor", 0, &fillColor);
        m_fillTexture = fillColor;
        if (m_fillTexture) {
            int alpha;
            int red;
            int green;
            int blue;
            block->GetParameter("FillColorAlpha", 0, &alpha);
            block->GetParameter("FillColorRed", 0, &red);
            block->GetParameter("FillColorGreen", 0, &green);
            block->GetParameter("FillColorBlue", 0, &blue);
            m_fillColor = alpha << 24 | blue << 16 | green << 8 | red;
        }
    }
    int size = (CountShaderParameters(block) * 8 + 0x40) * 4 + 0x10;
    m_packet = (ShaderPacket *)memalign(0x40, size);
    memset(m_packet, 0, size);
    QWORD *p = m_packet->data;
    p->ui[0] = 0x11000000;
    p->ui[1] = 0x03000000;
    p->ui[2] = 0x02000000;
    p->ui[3] = 0x6C0D8320;
    p++;
    ReadGSRegisterStates(&p, block, alphaBlend);
    ReadAnimationParameters(&p, block);
    ReadShaderParameters(&p, block);
    p->ui[0] = 0x14000000;
    ShaderPacket *packet = m_packet;
    packet->qwc = (unsigned int)(size - 0x10) >> 4;
    packet->mark = 0;
    packet->id = 0x30;      // REF
    packet->next = packet->data;
    packet->pad[0] = 0;
    packet->pad[1] = 0;
    return 1;
}


// ---- src/PSXShaderDescriptor/ReadFromFile.cpp
extern "C" void *memalign(unsigned int align, unsigned int size);
extern "C" void *memset(void *dst, int c, unsigned int n);




extern StringTable *LpMicroProgramStringTable;

class ParameterBlock;



// A plain diffuse shader using the named texture.
int PSXShaderDescriptor::ReadFromFile(char *name)
{
    m_microProgram = LpMicroProgramStringTable->GetIndex("Diffuse");
    m_numTextures = 1;
    m_textures[0] = LpTextureMapStringTable->AddItem(name, 0);
    m_texCoords[0] = 0;
    LpTextureMapStringTable->Request(m_textures[0]);
    m_packet = (ShaderPacket *)memalign(0x40, sizeof(ShaderPacket));
    memset(m_packet, 0, sizeof(ShaderPacket));
    QWORD *p = m_packet->data;
    p->ui[0] = 0x11000000;
    p->ui[1] = 0x03000000;
    p->ui[2] = 0x02000000;
    p->ui[3] = 0x6C0D8320;
    p++;
    ReadGSRegisterStates(&p, 0, 1);
    p->ui[0] = 0x14000000;
    ShaderPacket *packet = m_packet;
    packet->qwc = 0x10;
    packet->mark = 0;
    packet->id = 0x30;      // REF
    packet->next = packet->data;
    packet->pad[0] = 0;
    packet->pad[1] = 0;
    return 1;
}


// ---- src/PSXShaderDescriptor/ReadGSRegisterStates.cpp
typedef unsigned long u_long;


#define SCE_GS_SET_TEXA(ta0, aem, ta1) ((u_long)(ta0) | ((u_long)(aem) << 15) | ((u_long)(ta1) << 32))
#define SCE_GS_SET_ALPHA(a, b, c, d, fix) \
    ((u_long)(a) | ((u_long)(b) << 2) | ((u_long)(c) << 4) | ((u_long)(d) << 6) | ((u_long)(fix) << 32))
#define SCE_GS_SET_CLAMP(wms, wmt, minu, maxu, minv, maxv) \
    ((u_long)(wms) | ((u_long)(wmt) << 2) | ((u_long)(minu) << 4) | ((u_long)(maxu) << 14) | \
    ((u_long)(minv) << 24) | ((u_long)(maxv) << 34))
#define SCE_GS_SET_PRMODE(iip, tme, fge, abe, aa1, fst, ctxt, fix) \
    (((u_long)(iip) << 3) | ((u_long)(tme) << 4) | ((u_long)(fge) << 5) | ((u_long)(abe) << 6) | \
    ((u_long)(aa1) << 7) | ((u_long)(fst) << 8) | ((u_long)(ctxt) << 9) | ((u_long)(fix) << 10))
#define SCE_GS_SET_TEST(ate, atst, aref, afail, date, datm, zte, ztst) \
    ((u_long)(ate) | ((u_long)(atst) << 1) | ((u_long)(aref) << 4) | ((u_long)(afail) << 12) | \
    ((u_long)(date) << 14) | ((u_long)(datm) << 15) | ((u_long)(zte) << 16) | ((u_long)(ztst) << 17))
#define SCE_GS_SET_TEX1(lcm, mxl, mmag, mmin, mtba, l, k) \
    ((u_long)(lcm) | ((u_long)(mxl) << 2) | ((u_long)(mmag) << 5) | ((u_long)(mmin) << 6) | \
    ((u_long)(mtba) << 9) | ((u_long)(l) << 19) | ((u_long)(k) << 32))
#define SCE_GS_SET_ZBUF(zbp, psm, zmsk) ((u_long)(zbp) | ((u_long)(psm) << 24) | ((u_long)(zmsk) << 32))



extern PSXVideoCard *LpVideoCard;


struct GSRegisterPacket {
    sceGifTag m_tag;                    // +0x00
    u_long m_tex0;                      // +0x10
    u_long m_tex0Reg;                   // +0x18
    u_long m_texa;                      // +0x20
    u_long m_texaReg;                   // +0x28
    u_long m_x30;                       // +0x30
    u_long m_x30Reg;                    // +0x38
    u_long m_alpha;                     // +0x40
    u_long m_alphaReg;                  // +0x48
    u_long m_clamp;                     // +0x50
    u_long m_clampReg;                  // +0x58
    u_long m_colclamp;                  // +0x60
    u_long m_colclampReg;               // +0x68
    u_long m_fba;                       // +0x70
    u_long m_fbaReg;                    // +0x78
    u_long m_pabe;                      // +0x80
    u_long m_pabeReg;                   // +0x88
    u_long m_prmode;                    // +0x90
    u_long m_prmodeReg;                 // +0x98
    u_long m_test;                      // +0xA0
    u_long m_testReg;                   // +0xA8
    u_long m_tex1;                      // +0xB0
    u_long m_tex1Reg;                   // +0xB8
    u_long m_zbuf;                      // +0xC0
    u_long m_zbufReg;                   // +0xC8
};


// Builds the A+D packet of GS register states for the shader, each field
// overridable from the parameter block.
int PSXShaderDescriptor::ReadGSRegisterStates(QWORD **packet, ParameterBlock *block, int alphaBlend)
{
    GSRegisterPacket *p = (GSRegisterPacket *)*packet;
    *(int128_t *)&p->m_tag = 0;
    p->m_tag.FLG = 0;
    p->m_tag.EOP = 1;
    p->m_tag.NREG = 12;
    p->m_tag.NLOOP = 1;
    p->m_tag.REGS0 = 0xE;
    p->m_tag.REGS1 = 0xE;
    p->m_tag.REGS2 = 0xE;
    p->m_tag.REGS3 = 0xE;
    p->m_tag.REGS4 = 0xE;
    p->m_tag.REGS5 = 0xE;
    p->m_tag.REGS6 = 0xE;
    p->m_tag.REGS7 = 0xE;
    p->m_tag.REGS8 = 0xE;
    p->m_tag.REGS9 = 0xE;
    p->m_tag.REGS10 = 0xE;
    p->m_tag.REGS11 = 0xE;
    m_tex0 = &p->m_tex0;
    p->m_tex0 = 0x598007C00;
    p->m_tex0Reg = 6;                   // TEX0_1
    m_textureColorComponent = 1;
    m_textureFunction = 0;
    int value;
    if (block) {
        block->GetParameter("textureColorComponent", m_textureColorComponent, &value);
        m_textureColorComponent = value;
        block->GetParameter("textureFunction", m_textureFunction, &value);
        m_textureFunction = value;
    }

    int ta0 = 0xFF;
    int ta1 = 0xFF;
    int aem = 0;
    if (block) {
        block->GetParameter("sourceAlphaValueWhenAis0", ta0, &ta0);
        block->GetParameter("sourceAlphaValueWhenAis1", ta1, &ta1);
        block->GetParameter("methodOfExpandingTextureAlpha", aem, &aem);
    }
    p->m_texa = SCE_GS_SET_TEXA(ta0, aem, ta1);
    p->m_texaReg = 0x3B;                // TEXA
    p->m_x30 = 0;
    p->m_x30Reg = 0x1A;                 // PRMODECONT

    int a = 0;
    int b = 1;
    int c = 0;
    int d = 1;
    int fix = 0x80;
    if (block) {
        block->GetParameter("inputColorA", a, &a);
        block->GetParameter("inputColorB", b, &b);
        block->GetParameter("inputAlphaC", c, &c);
        block->GetParameter("inputAlphaD", d, &d);
        block->GetParameter("fixedAlphaValue", fix, &fix);
    }
    if (ShowDepthComplexity) {
        c = 2;
        fix = 0x10;
    }
    p->m_alpha = SCE_GS_SET_ALPHA(a, b, c, d, fix);
    p->m_alphaReg = 0x42;               // ALPHA_1

    int wms = 0;
    int wmt = 0;
    int minu = 0;
    int maxu = 0;
    int minv = 0;
    int maxv = 0;
    if (block) {
        block->GetParameter("wrapModeHorizontal", wms, &wms);
        block->GetParameter("wrapModeVertical", wmt, &wmt);
        block->GetParameter("minClampParameterInUDirection", minu, &minu);
        block->GetParameter("maxClampParameterInUDirection", maxu, &maxu);
        block->GetParameter("minClampParameterInVDirection", minv, &minv);
        block->GetParameter("maxClampParameterInVDirection", maxv, &maxv);
    }
    p->m_clamp = SCE_GS_SET_CLAMP(wms, wmt, minu, maxu, minv, maxv);
    p->m_clampReg = 8;                  // CLAMP_1

    int clamp = 1;
    if (block)
        block->GetParameter("colorClampingMethod", clamp, &clamp);
    p->m_colclamp = clamp;
    p->m_colclampReg = 0x46;            // COLCLAMP

    int fba = 0;
    if (block)
        block->GetParameter("alphaCorrectionValue", fba, &fba);
    p->m_fba = fba;
    p->m_fbaReg = 0x4A;                 // FBA_1

    int pabe = 0;
    if (block)
        block->GetParameter("alphaBlendingControlInUnitsOfPixels", pabe, &pabe);
    p->m_pabe = pabe;
    p->m_pabeReg = 0x49;                // PABE

    int iip = 1;
    int tme = 1;
    int fge = 1;
    int abe = alphaBlend ? 1 : 0;
    int aa1 = 0;
    int fst = 0;
    int ctxt = 0;
    int fixPrim = 0;
    if (block) {
        block->GetParameter("gouraudShadingOnOff", iip, &iip);
        block->GetParameter("textureMappingOnOff", tme, &tme);
        block->GetParameter("foggingOnOff", fge, &fge);
        block->GetParameter("alphaBlendingOnOff", abe, &abe);
        block->GetParameter("onePassAntiAliasingOnOff", aa1, &aa1);
        block->GetParameter("textureCoordinatesSTQorUV", fst, &fst);
        block->GetParameter("environmentContext0or1", ctxt, &ctxt);
        block->GetParameter("fragmentValueControlFixedOrNot", fixPrim, &fixPrim);
    }
    if (ShowDepthComplexity) {
        fge = 0;
        abe = 1;
    }
    p->m_prmode = SCE_GS_SET_PRMODE(iip, tme, fge, abe, aa1, fst, ctxt, fixPrim);
    p->m_prmodeReg = 0x1B;              // PRMODE

    int ate = 1;
    int atst = 6;
    int aref;
    int afail = 0;
    int date = 0;
    int datm = 1;
    int zte = 1;
    int ztst = 2;
    aref = 0x78;
    if (block) {
        block->GetParameter("alphaTestOnOff", ate, &ate);
        block->GetParameter("alphaTestMethod", atst, &atst);
        block->GetParameter("alphaReferenceValue", aref, &aref);
        block->GetParameter("processingMethodWhenFailedInAlphaTest", afail, &afail);
        block->GetParameter("destinationAlphaTestOnOff", date, &date);
        block->GetParameter("destinationAlphaTestMode", datm, &datm);
        block->GetParameter("depthTestOnOff", zte, &zte);
        block->GetParameter("depthTestMethod", ztst, &ztst);
    }
    if (ShowDepthComplexity)
        ztst = 1;
    p->m_test = SCE_GS_SET_TEST(ate, atst, aref, afail, date, datm, zte, ztst);
    p->m_testReg = 0x47;                // TEST_1

    int lcm = 0;
    int mxl = 0;
    int mmag = 1;
    int mmin = 5;
    int l = 0;
    int k = -75;
    if (block) {
        block->GetParameter("lodCalculationMethod", lcm, &lcm);
        block->GetParameter("maximumMipLevel", mxl, &mxl);
        block->GetParameter("magFilter", mmag, &mmag);
        block->GetParameter("minFilter", mmin, &mmin);
        block->GetParameter("L", l, &l);
        block->GetParameter("K", k, &k);
    }
    p->m_tex1 = SCE_GS_SET_TEX1(lcm, mxl, mmag, mmin, 0, l, k);
    p->m_tex1Reg = 0x14;                // TEX1_1

    int zmsk = 0;
    if (block)
        block->GetParameter("zWriteDisabled", zmsk, &zmsk);
    p->m_zbuf = SCE_GS_SET_ZBUF(LpVideoCard->GetZBufferAddress(), LpVideoCard->GetZBufferFormat(), zmsk);
    p->m_zbufReg = 0x4E;                // ZBUF_1

    *packet = (QWORD *)(p + 1);
    if (m_fillTexture)
        SetUpFillTextureAddress();
    return 0;
}


// ---- src/PSXShaderDescriptor/CountShaderParameters.cpp

extern StringTable *LpVu1OffsetStringTable;



// Count the VU1 offset parameters this block defines.
int PSXShaderDescriptor::CountShaderParameters(ParameterBlock *block)
{
    int i;
    int count = 0;
    for (i = 0; i < LpVu1OffsetStringTable->m_totalEntries; i++) {
        float values[4];
        if (block->GetParameter(LpVu1OffsetStringTable->GetString(i), values, 4))
            count++;
    }
    return count;
}


// ---- src/PSXShaderDescriptor/ReadShaderParameters.cpp

extern StringTable *LpVu1OffsetStringTable;

#define SCE_VIF1_SET_UNPACK(vuaddr, num, cmd, irq) \
    ((unsigned int)(vuaddr) | ((unsigned int)(num) << 16) | \
    ((unsigned int)(0x60 | (cmd)) << 24) | ((unsigned int)(irq) << 31))





// For each VU1 offset parameter the block defines, append a VIF upload of
// its four values to the packet.
void PSXShaderDescriptor::ReadShaderParameters(QWORD **packet, ParameterBlock *block)
{
    int i;
    QWORD *p = *packet;
    for (i = 0; i < LpVu1OffsetStringTable->m_totalEntries; i++) {
        float values[4];
        if (block->GetParameter(LpVu1OffsetStringTable->GetString(i), values, 4)) {
            int offset = (int)LpVu1OffsetStringTable->GetPointer(i);
            p[0].ui[0] = 0x11000000;
            p[0].ui[1] = 0x03000000;
            p[0].ui[2] = 0x02000000;
            p[0].ui[3] = SCE_VIF1_SET_UNPACK(offset | 0x8000, 1, 0x0C, 0);
            if (m_animation)
                m_animation->m_parameters[i] = &p[1];
            p[1].f[0] = values[0];
            p[1].f[1] = values[1];
            p[1].f[2] = values[2];
            p[1].f[3] = values[3];
            p += 2;
        }
    }
    *packet = p;
}


// ---- src/PSXShaderDescriptor/ReadAnimationParameters.cpp

extern StringTable *LpMicroProgramStringTable;





int AnimatedUVCallback(void *data, MaterialAnimationData *animation);


void PSXShaderDescriptor::ReadAnimationParameters(QWORD **packet, ParameterBlock *block)
{
    int animates;
    int uvsAnimate;
    block->GetParameter("animates", 0, &animates);
    block->GetParameter("uvs_animate", 0, &uvsAnimate);
    if (animates || uvsAnimate)
        m_animation = new MaterialAnimationData(block);
    if (uvsAnimate && m_microProgram == LpMicroProgramStringTable->GetIndex("DiffuseTexTrans")) {
        m_animation->m_uvAnimation = new UVAnimationData(block);
        m_animation->RegisterAnimationCallbackFunction(AnimatedUVCallback, 0);
    }
}


// ---- src/MaterialAnimationData/ctor.cpp


MaterialAnimationData::MaterialAnimationData(ParameterBlock *params)
{
    int value;

    m_flags = 0;
    m_lastTime = -1;
    m_callback = 0;
    m_callbackData = 0;
    m_uvAnimation = 0;
    for (int i = 0; i < 4; i++)
        m_parameters[i] = 0;
    params->GetParameter("animatedMapInitialFrame", 0.0f, &m_frame);
    params->GetParameter("animatedMapFramesPerSecond", 1.0f, &m_framesPerSecond);
    params->GetParameter("animatedMapLooping", 1, &value);
    m_bits.m_looping = value;
    params->GetParameter("animatedMapReverseLoopOnEnd", 0, &value);
    m_bits.m_pingPong = value;
    params->GetParameter("animatedMapLoopOnEndToFrame", -1, &m_loopOnEndToFrame);
}


// ---- src/MaterialAnimationData/dtor.cpp

MaterialAnimationData::~MaterialAnimationData()
{
    if (m_uvAnimation)
        delete m_uvAnimation;
}


// ---- src/MaterialAnimationData/RegisterAnimationCallbackFunction.cpp
// RegisterAnimationCallbackFunction__21MaterialAnimationDataFPFPvP21MaterialAnimationData_iPv — setter: *(a0+0x14)=a1; *(a0+0x18)=a2.
void MaterialAnimationData::RegisterAnimationCallbackFunction(int (*callback)(void *data, MaterialAnimationData *animation),
                                                              void *data)
{
    m_callback = callback;
    m_callbackData = data;
}


// ---- src/MaterialAnimationData/UpdateAnimation.cpp
extern "C" float fmodf(float x, float y);

// What the texture map string table holds for an animated map (only the
// frame count is read here).
#ifdef SPLASHDOWN_PC
#include "AnimTextureMap/AnimTextureMap.h"
struct TextureMapEntry {
    char pad0[__builtin_offsetof(AnimTextureMap, m_numFrames)];
    int m_numFrames;                    // AnimTextureMap's (PS2 +0x20; +0x18 here)
};
#else
struct TextureMapEntry {
    char pad0[0x20];
    int m_numFrames;                    // +0x20
};
#endif


extern Game *LpGame;


int MaterialAnimationData::UpdateAnimation(int texture)
{
    if (LpGame->m_lastTime == m_lastTime || LpGame->m_paused)
        return 1;
    int run = 1;
    if (m_callback)
        run = m_callback(m_callbackData, this);
    if (run) {
        int numFrames = ((TextureMapEntry *)LpTextureMapStringTable->GetPointer(texture))->m_numFrames;
        float dt = 0.0f;
        if (m_lastTime != -1)
            dt = 0.001f * (unsigned int)(LpGame->m_lastTime - m_lastTime);
        m_lastTime = LpGame->m_lastTime;
        if (numFrames > 0) {
            dt *= m_framesPerSecond;
            if (m_bits.m_reverse)
                m_frame -= dt;
            else
                m_frame += dt;
            if (!(m_frame < numFrames)) {
                float frame = fmodf(m_frame, numFrames);
                if (m_bits.m_pingPong) {
                    m_frame = (numFrames - 1) - frame;
                    m_bits.m_reverse = 1;
                } else if (m_loopOnEndToFrame != -1) {
                    m_frame = m_loopOnEndToFrame + fmodf(frame, numFrames - m_loopOnEndToFrame);
                } else {
                    m_frame = frame;
                }
                if (!m_bits.m_looping)
                    m_bits.m_stopped = 1;
            } else if (m_frame < 0.0f) {
                m_frame = Fabs(fmodf(m_frame, numFrames));
                m_bits.m_reverse = 0;
            }
            if (m_bits.m_stopped)
                m_frame = numFrames - 1;
        }
    }
    return 1;
}


// ---- src/PSXShaderDescriptor/SetUpTextureMapAddress.cpp



extern PSXWater *GameWater;


int PSXShaderDescriptor::SetUpTextureMapAddress(PSXTextureMap *tex)
{
    int result = 0;
    PSXMipSurface *mip = tex->GetReqdMipLevel();
    unsigned long *reg = m_tex0;
    int psm;
    int cbp = 0;
    int cpsm = 0;
    int csm = 0;
    int csa = 0;
    int cld = 0;
    switch (tex->m_format) {
    case 0x22B8:
    case 0x115C:
        psm = 0;            // PSMCT32
        break;
    case 0x613:
    case 0x22B:
    case 0x235:
        psm = 2;            // PSMCT16
        break;
    case 8:
        cbp = tex->m_xF0;
        psm = 0x13;         // PSMT8
        cld = 1;
        break;
    case 4:
        cbp = tex->m_xF0;
        psm = 0x14;         // PSMT4
        cld = 1;
        break;
    default:
#line 1107 "PSXSurfaceMaterial.cpp"
        ASSERT(0, "");
        break;
    }
    *reg = (unsigned long)tex->m_xEC | ((unsigned long)mip->m_blocks << 14) | ((unsigned long)psm << 20)
         | ((unsigned long)mip->m_widthLog2 << 26) | ((unsigned long)mip->m_heightLog2 << 30)
         | ((unsigned long)m_textureColorComponent << 34) | ((unsigned long)m_textureFunction << 35) | ((unsigned long)cbp << 37)
         | ((unsigned long)cpsm << 51) | ((unsigned long)csm << 55) | ((unsigned long)csa << 56)
         | ((unsigned long)cld << 61);
    if (tex->m_compressed) {
        reg += 2;
        *reg = 0xFF000080FF;    // TEXA: TA0 = 0xFF, AEM = 1, TA1 = 0xFF
    }
    if (GameWater && tex == GameWater->m_texture)
        GameWater->UpdateTex0();
    return result;
}


// ---- src/PSXShaderDescriptor/SetUpFillTextureAddress.cpp

int PSXShaderDescriptor::SetUpFillTextureAddress()
{
    *m_tex0 = ((unsigned long)m_textureFunction << 35) | (((unsigned long)m_textureColorComponent << 34) | 0x77FF);
    return 0;
}


// ---- src/PSXShaderDescriptor/IsIdentical.cpp
extern "C" int memcmp(const void *a, const void *b, unsigned int n);




bool PSXShaderDescriptor::IsIdentical(ShaderDescriptor *other)
{
    if (m_microProgram != other->m_microProgram || m_numTextures != other->m_numTextures || m_textures[0] != other->m_textures[0] || m_textures[1] != other->m_textures[1] || m_texCoords[0] != other->m_texCoords[0]
        || m_texCoords[1] != other->m_texCoords[1] || m_animation != other->m_animation || m_fillTexture != other->m_fillTexture || m_enabled != other->m_enabled)
        return 0;
    int size = m_packet->qwc * 16;
    if (size != other->m_packet->qwc * 16)
        return 0;
    return !memcmp(m_packet->data, other->m_packet->data, size);
}


// ---- src/PSXSurfaceMaterial/ctor.cpp
PSXSurfaceMaterial::PSXSurfaceMaterial()
{
}

// ---- src/PSXSurfaceMaterial/dtor.cpp


PSXSurfaceMaterial::~PSXSurfaceMaterial()
{
}


// ---- src/PSXSurfaceMaterial/UpdateLOD.cpp
// Retail pads this function's labels, so no `#pragma alignlabel off`.





void PSXSurfaceMaterial::UpdateLOD(float *uvScales, float numTexCoords, float maxSize, float scale)
{
    static float TextureLODScalar = 1.5f;
    maxSize *= TextureLODScalar;
    for (int i = 0; i < m_numShaders; i++) {
        int j;
        ShaderDescriptor *layer = m_shaders[i];
        for (j = 0; j < layer->m_numTextures; j++) {
            BaseTextureMap *texture = (BaseTextureMap *)LpTextureMapStringTable->GetPointer(layer->m_textures[j]);
            if (texture) {
                int coord = layer->m_texCoords[j];
#line 1222 "PSXSurfaceMaterial.cpp"
                ASSERT(coord < numTexCoords, "TextureCoordinate index out of range.");
                float uvScale = scale * uvScales[coord];
                float size = uvScale * texture->m_width;
                if (size > maxSize)
                    size = maxSize;
                texture->AddUsageViaProjectedSize(size);
            }
        }
    }
}


// ---- src/PSXSurfaceMaterial/AddGeometry.cpp

extern PSXSurfaceShaderList *LpSurfaceShaderList;







void PSXSurfaceMaterial::AddGeometry(SurfaceGeometry *geometry, _sceDmaTag *head, _sceDmaTag *tail, int single)
{
    int i;
    int count;
    ShaderDescriptor *layer;
    if (single)
        count = 1;
    else
        count = m_numShaders;
    if (DebugFill) {
        int shader;
        switch (DebugFill) {
        case 0x20:
        default:     // retail's compare chain treats 0x20 as the default
            shader = 0;
            break;
        case 0x40:
            shader = 1;
            break;
        case 0x80:
            shader = 2;
            break;
        case 0x100:
            shader = 3;
            break;
        }
        BaseTextureMap *texture = (BaseTextureMap *)LpTextureMapStringTable->GetPointer(shader);
        texture->m_usage = 10.0f;
        for (i = 0; i < count; i++)
            ((PSXSurfaceShaderInstance *)LpSurfaceShaderList->m_shaders[shader])->AddGeometry(geometry, head, tail);
    } else {
        for (i = 0; i < count; i++) {
            layer = m_shaders[i];
            if (layer->m_enabled) {
                ((PSXSurfaceShaderInstance *)LpSurfaceShaderList->m_shaders[m_instances[i]])->AddGeometry(geometry, head, tail);
                if (layer->m_animation)
                    layer->m_animation->UpdateAnimation(layer->m_textures[0]);
            }
        }
    }
}


