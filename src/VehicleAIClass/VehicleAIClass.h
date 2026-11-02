// VehicleAIClass: an AI driver (SearchableObject + GameObject at +8, data
// from +0x38). 0xF40 bytes (`new` in JetskiAIVehicle::Create): data to
// +0xF3B, then its own vptr at +0xF3C (declared after the data). Its Tick /
// Render3D override GameObject's (the second base), so they get primary
// slots too. Names are the most common among the copies (mwcc's layout);
// +0x50 is the 200-point path.
#ifndef VEHICLEAICLASS_VEHICLEAICLASS_H
#define VEHICLEAICLASS_VEHICLEAICLASS_H

#include "GameObject/GameObject.h"
#include "SearchableObject/SearchableObject.h"
#include "TrackPosition_Tag/TrackPosition_Tag.h"
#include "d3dtypes.h"

class CollisionObject;
class RaceStatusClass;
class Scene;
class SearchPlacement;
class Terrain;
class VehicleAIInterface;
struct AIStateDataType_Tag;
struct AIStateSpaceType_Tag;
struct RaceStatusStruct_Tag;
class TrackType;

class VehicleAIClass : public SearchableObject, public GameObject {
public:
    VehicleAIClass(int enabled);
    GameObject *Create(VehicleAIInterface *vehicle, int *a, _D3DVECTOR position, _D3DVECTOR velocity, float speed,
                       _D3DVECTOR forward, _D3DVECTOR up, int b, int c, RaceStatusStruct_Tag *status,
                       RaceStatusClass *race, TrackType *track, _D3DVECTOR *d, _D3DVECTOR *e, int f,
                       CollisionObject **colliders, float *g, float h, Terrain *terrain, float *k, float *l, int m,
                       float *n, float *o, float *p, float *q, float *r, float *s, float *t, Scene *scene);
    int ApplyALOD(float *maxSpeed);
    int FreeStateSpace(AIStateSpaceType_Tag **root);
    int ResetAI();
    int UpdateTrackAI(float dt);
    int AvoidCollisions(int numObjects, CollisionObject **objects, _D3DVECTOR position, _D3DVECTOR velocity,
                        float time, _D3DVECTOR *direction, float *distance);
    int CheckForCollisions(_D3DVECTOR *position, float radius, int numObjects, CollisionObject **objects,
                           _D3DVECTOR velocity, _D3DVECTOR *direction);
    int GetNewState(AIStateDataType_Tag *state, AIStateDataType_Tag **newState, _D3DVECTOR position);

    unsigned int m_flags;               // +0x38
    VehicleAIInterface *m_vehicle;      // +0x3C
    float m_alodRange;                  // +0x40
    float m_alodSpeedScale;             // +0x44
    float m_x48;                        // +0x48
    float m_alodMaxScale;               // +0x4C
    _D3DVECTOR m_path[200];             // +0x50
    _D3DVECTOR m_xCD0;                  // +0xCD0
    char padCE0[0xD00 - 0xCE0];
    _D3DVECTOR m_xD00;                  // +0xD00
    _D3DVECTOR m_xD10;                  // +0xD10
    _D3DVECTOR m_xD20;                  // +0xD20
    _D3DVECTOR m_xD30;                  // +0xD30
    float m_xD40;                       // +0xD40
    char padD44[0xD50 - 0xD44];
    _D3DVECTOR m_xD50;                  // +0xD50
    _D3DVECTOR m_xD60;                  // +0xD60
    int m_mode;                         // +0xD70
    int m_xD74;                         // +0xD74
    RaceStatusStruct_Tag *m_status;     // +0xD78
    RaceStatusClass *m_raceStatus;      // +0xD7C
    TrackType *m_track;                 // +0xD80
    float m_throttle;                   // +0xD84
    float m_brake;                      // +0xD88
    char padD8C[0xD90 - 0xD8C];
    _D3DVECTOR m_steer;                 // +0xD90
    _D3DVECTOR m_xDA0;                  // +0xDA0
    _D3DVECTOR m_xDB0;                  // +0xDB0
    _D3DVECTOR m_xDC0;                  // +0xDC0
    int m_numObjects;                   // +0xDD0
    CollisionObject **m_objects;        // +0xDD4
    float m_xDD8;                       // +0xDD8
    float m_xDDC;                       // +0xDDC, time since the last stunt
    float m_xDE0;                       // +0xDE0
    float m_xDE4;                       // +0xDE4
    float m_xDE8;                       // +0xDE8
    Terrain *m_terrain;                 // +0xDEC
    float m_xDF0;                       // +0xDF0
    float m_xDF4;                       // +0xDF4
    float m_xDF8;                       // +0xDF8
    float m_xDFC;                       // +0xDFC
    int m_xE00;                         // +0xE00
    int m_xE04;                         // +0xE04
    int m_xE08;                         // +0xE08
    int m_buttons;                      // +0xE0C
    _D3DVECTOR m_xE10;                  // +0xE10
    int m_xE20;                         // +0xE20
    int m_xE24;                         // +0xE24
    float m_xE28;                       // +0xE28
    float m_xE2C;                       // +0xE2C
    int m_xE30;                         // +0xE30
    int m_xE34[3];                      // +0xE34
    AIStateDataType_Tag *m_state;       // +0xE40
    char padE44[0xE50 - 0xE44];
    _D3DVECTOR m_xE50;                  // +0xE50
    _D3DVECTOR m_xE60;                  // +0xE60
    _D3DVECTOR m_xE70;                  // +0xE70
    _D3DVECTOR m_xE80;                  // +0xE80
    float m_xE90;                       // +0xE90
    float m_xE94;                       // +0xE94
    float m_xE98;                       // +0xE98
    char padE9C[0xEA0 - 0xE9C];
    _D3DVECTOR m_xEA0;                  // +0xEA0
    SearchPlacement *m_searchPlacement; // +0xEB0
    char padEB4[0xEC0 - 0xEB4];
    _D3DVECTOR m_extents;               // +0xEC0
    float m_xED0;                       // +0xED0
    float m_xED4;                       // +0xED4
    float m_xED8;                       // +0xED8
    float m_xEDC;                       // +0xEDC
    float m_xEE0;                       // +0xEE0
    float m_xEE4;                       // +0xEE4
    float m_xEE8;                       // +0xEE8
    float m_xEEC;                       // +0xEEC
    float m_xEF0;                       // +0xEF0
    int m_xEF4;                         // +0xEF4
    float m_xEF8;                       // +0xEF8
    float m_xEFC;                       // +0xEFC
    TrackPosition_Tag m_trackPosition;  // +0xF00
    float m_random[10];                 // +0xF0C
    float m_xF34;                       // +0xF34
    float m_xF38;                       // +0xF38

    // --- own virtuals (retail vtable); vptr at +0xF3C ---
    virtual ~VehicleAIClass();
    virtual int Tick(float dt);
    virtual int Render3D();
};

#endif
