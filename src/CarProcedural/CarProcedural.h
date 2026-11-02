// CarProcedural: a spline-driven ambient car (a PhysicsBody). 0x4B0 bytes
// (`new` in Scene loading); its own data starts after PhysicsBody's 0x298.
// Members are where the ctor stores (ints, and _D3DVECTORs at +0x2C0..+0x330),
// named by the other copies. +0x2B4 is the spline (LoadVueFile,
// ComputeSplineLengths); LoadCarProceduralBinary and the dtor call it a
// CarProceduralShared. +0x4A8 is the tire array the dtor deletes.
#ifndef CARPROCEDURAL_CARPROCEDURAL_H
#define CARPROCEDURAL_CARPROCEDURAL_H

#include "PhysicsBody/PhysicsBody.h"

class CollisionObject;
class SearchNode;
class SoultreeObject;
class Spline;
class Terrain;

class CarProcedural : public PhysicsBody {
public:
    CarProcedural(int enabled);
    void AddSearchPlacement(SearchNode *a, SearchNode *b);
    void CalculateHermiteCoefficients(float t, float &h00, float &h01, float &h10, float &h11);
    float ComputeLagTime(float t, float lag);
    void ComputeSplineLengths();
    _D3DVECTOR InterpolateSpline(float t);
    void LoadCarProceduralBinary(long file);
    int LoadVueFile(char *fileName, char *objectName);
    void ResetTime(float t, int a);

    // --- virtuals in retail slot order ---
    virtual ~CarProcedural();
    virtual int Tick(float dt);
    virtual void ResetDerivatives();
    virtual int Create(char *a, char *b, Terrain *terrain, char *c, _D3DVECTOR *d, float e, int f, int g, float h,
                       float i, int j, float k, float l, float m, int n, int o);
    virtual void AddWorldForce(_D3DVECTOR force);
    virtual void AddWorldForce(_D3DVECTOR force, _D3DVECTOR position);
    virtual void AddWorldMoment(_D3DVECTOR moment);
    virtual void AddBodyForce(_D3DVECTOR force);
    virtual void AddBodyForce(_D3DVECTOR force, _D3DVECTOR position);
    virtual void AddBodyMoment(_D3DVECTOR moment);

    char pad298[0x2A0 - 0x298];
    SoultreeObject *m_model;            // +0x2A0
    int m_loaded;                       // +0x2A4
    CollisionObject *m_testCollision;   // +0x2A8
    CollisionObject *m_collision;       // +0x2AC
    CollisionObject *m_rays;            // +0x2B0, collision rays
    Spline *m_spline;                   // +0x2B4
    char pad2B8[0x2C0 - 0x2B8];
    _D3DVECTOR m_x2C0;                  // +0x2C0
    _D3DVECTOR m_x2D0;                  // +0x2D0
    _D3DVECTOR m_x2E0;                  // +0x2E0
    _D3DVECTOR m_x2F0;                  // +0x2F0
    _D3DVECTOR m_x300;                  // +0x300
    _D3DVECTOR m_up;                    // +0x310
    _D3DVECTOR m_x320;                  // +0x320
    _D3DVECTOR m_x330;                  // +0x330
    int m_x340;                         // +0x340
    int m_x344;                         // +0x344
    int m_x348;                         // +0x348
    int m_x34C;                         // +0x34C
    int m_x350;                         // +0x350
    int m_x354;                         // +0x354
    int m_x358;                         // +0x358
    unsigned int m_numTires;            // +0x35C
    unsigned int m_segment;             // +0x360
    int m_x364;                         // +0x364
    char m_carName[0x468 - 0x368];      // +0x368
    float m_x468;                       // +0x468
    float m_timeScale;                  // +0x46C
    float m_x470;                       // +0x470
    float m_x474;                       // +0x474
    float m_x478;                       // +0x478
    float m_x47C;                       // +0x47C, inverse of the front tire's height
    float m_x480;                       // +0x480
    float m_wheelAngle;                 // +0x484, tire roll
    float m_steerAngle;                 // +0x488
    float m_wheelBase;                  // +0x48C
    float m_x490;                       // +0x490
    int m_x494;                         // +0x494
    float m_x498;                       // +0x498
    float m_x49C;                       // +0x49C
    float m_x4A0;                       // +0x4A0
    Terrain *m_terrain;                 // +0x4A4
    SoultreeObject **m_tires;           // +0x4A8, new[]'d tire objects
    char pad4AC[0x4B0 - 0x4AC];
};

#endif
