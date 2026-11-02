// ShaderDescriptor: how one material pass is drawn (microprogram, up to
// two textures, GS states), 0x24 bytes. The data is set up by
// PSXShaderDescriptor but compared through the base (IsIdentical).
#ifndef SHADERDESCRIPTOR_H
#define SHADERDESCRIPTOR_H

struct ShaderPacket;
struct _sceDmaTag;
class MaterialAnimationData;

class ShaderDescriptor {
public:
    ShaderDescriptor();

    // --- virtuals in retail slot order ---
    virtual ~ShaderDescriptor();        // vptr +0x00
    virtual int ReadFromFile(char *name) = 0;
    virtual bool IsIdentical(ShaderDescriptor *other) = 0;

    short m_microProgram;               // +0x04 (LpMicroProgramStringTable index, -1)
    unsigned char m_enabled;            // +0x06 (1)
    short m_textures[2];                // +0x08 (LpTextureMapStringTable indices, -1)
    unsigned char m_texCoords[2];       // +0x0C (texture coordinate set per texture)
    unsigned char m_numTextures;        // +0x0E
    unsigned char m_textureColorComponent;  // +0x0F (GS TEX0 TCC)
    unsigned char m_textureFunction;    // +0x10 (GS TEX0 TFX)
    unsigned char m_fillTexture;        // +0x11 (no texture: draw the fill color)
    int m_fillColor;                    // +0x14 (0xFF808080; int: Render's load order needs it)
    union {
        ShaderPacket *m_packet;         // +0x18 (render-state DMA packet: REF tag + GS data, memalign'd)
        _sceDmaTag *m_packetTag;        //       (the same packet as PSXSurfaceShaderInstance::Render sends it;
                                        //        a cast of m_packet moves Render's loads)
    };
    unsigned long *m_tex0;              // +0x1C (the packet's 64-bit TEX0 data)
    MaterialAnimationData *m_animation; // +0x20
};

#endif
