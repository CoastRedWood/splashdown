// SkyCube: the sky box, a DrawableCube kept centred on the camera
// (0xAF0 bytes, JetskiCompetition::LoadSkyCube news it).
#ifndef SKYCUBE_H
#define SKYCUBE_H

#include "DrawableCube/DrawableCube.h"

class SkyCube : public DrawableCube {
public:
    SkyCube(int enabled);
    SkyCube *Create(char *name, float scale);

    // --- virtuals in retail slot order ---
    virtual ~SkyCube();
    virtual int Tick(float dt);
    virtual int Render3D();

    float m_heightOffset;               // +0xAE8 (Create's scale; Tick offsets the camera height by it)
};

#endif
