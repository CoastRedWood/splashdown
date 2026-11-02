// UIWaterPatch: the height field behind UIWaterEffect, two buffers of
// 8-wide cells swapped each Tick. 0x70 bytes (new in
// UIWaterEffect::InternalCreate). Members from the ctor and Create (the cell
// sizes and their inverses, the origin, the buffers, the grid size).
#ifndef UIWATERPATCH_H
#define UIWATERPATCH_H

#include "GameObject/GameObject.h"
#include "d3dtypes.h"

struct WaterPatchCell {                 // 0x40 bytes, 8 grid points
    short height[8];                    // +0x00
    char pad10[0x30 - 0x10];
    short force[8];                     // +0x30
};

// Retail inherits GameObject privately (`class UIWaterPatch : GameObject`):
// its RTTI base list marks the base private (UIWater.cpp's @943, 0x18 bytes).
// The access declarations give the users what they call.
class UIWaterPatch : GameObject {
public:
    GameObject::Release;
    UIWaterPatch(int enabled);
    UIWaterPatch *Create(_D3DVECTOR position, int size, float scaleX, float scaleZ);
    void Reset();
    float GetHeight(int x, int y);
    int GetHeight(int y, float *out);    // copies one row of heights
    void ApplyExternalForce(float force, _D3DVECTOR position);

    static float HeightScale;
    static float gWakeForce;
    static short VELOCITY_SCALE __attribute__((section(".sdata")));

    // --- overrides in retail slot order ---
    virtual ~UIWaterPatch();
    virtual int Tick(float dt);

    float m_scaleX;                     // +0x30, cell width
    float m_invScaleX;                  // +0x34
    float m_scaleZ;                     // +0x38, cell depth
    float m_invScaleZ;                  // +0x3C
    _D3DVECTOR m_position;              // +0x40, origin
    int m_field50;                      // +0x50
    int m_current;                      // +0x54 (index into m_buffers)
    WaterPatchCell *m_buffers[2];       // +0x58
    int m_height;                       // +0x60
    int m_width;                        // +0x64, in cells
    float m_halfHeight;                 // +0x68
};

#endif
