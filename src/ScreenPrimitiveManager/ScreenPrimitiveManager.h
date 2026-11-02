// ScreenPrimitiveManager: queues ScreenPrimitives for one frame and renders
// them in Render3D. 0x40 bytes (new in its users); the ctor clears
// +0x30..+0x38 and sets m_queued; Create allocates at most 128 slots.
#ifndef SCREENPRIMITIVEMANAGER_SCREENPRIMITIVEMANAGER_H
#define SCREENPRIMITIVEMANAGER_SCREENPRIMITIVEMANAGER_H

#include "GameObject/GameObject.h"

class ScreenPrimitive;

class ScreenPrimitiveManager : public GameObject {
public:
    ScreenPrimitiveManager(int enabled);
    ScreenPrimitiveManager *Create(int maxPrimitives);
    void AddPrimitive(ScreenPrimitive *primitive);
    void RenderQueuedPrimitives();

    // --- overrides in retail slot order ---
    virtual ~ScreenPrimitiveManager();
    virtual int Render3D();

    int m_maxPrimitives;                // +0x30
    ScreenPrimitive **m_primitives;     // +0x34
    int m_numPrimitives;                // +0x38
    int m_queued;                       // +0x3C, 1 from the ctor
};

#endif
