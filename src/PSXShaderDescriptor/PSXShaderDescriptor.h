// PSXShaderDescriptor: reads a material pass from its parameter block and
// builds the GS render-state packet (0x24 bytes).
#ifndef PSXSHADERDESCRIPTOR_H
#define PSXSHADERDESCRIPTOR_H

#include "ShaderDescriptor/ShaderDescriptor.h"

struct QWORD;
class ParameterBlock;
class PSXTextureMap;

class PSXShaderDescriptor : public ShaderDescriptor {
public:
    PSXShaderDescriptor();
    int ReadFromFile(char *path, ParameterBlock *block, int pass, int firstPass);
    int CountShaderParameters(ParameterBlock *block);
    int ReadGSRegisterStates(QWORD **packet, ParameterBlock *block, int alphaBlend);
    void ReadAnimationParameters(QWORD **packet, ParameterBlock *block);
    void ReadShaderParameters(QWORD **packet, ParameterBlock *block);
    int SetUpFillTextureAddress();
    int SetUpTextureMapAddress(PSXTextureMap *texture);

    // --- virtuals in retail slot order ---
    virtual ~PSXShaderDescriptor();
    virtual int ReadFromFile(char *name);
    virtual bool IsIdentical(ShaderDescriptor *other);
};

#endif
