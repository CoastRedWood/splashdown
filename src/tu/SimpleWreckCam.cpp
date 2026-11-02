// SimpleWreckCam.cpp  @ 0x00338CE0 (text 0x338CE0-0x33B930)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's SimpleWreckCam.cpp as one translation unit: SimpleWreckCamera,
// then the static initializer (only the axis vectors). Its .rodata labels
// run @960-@1927 (RTTI SimpleWreckCamera, CameraBehavior);
// JetskiWreckCam.cpp restarts at @873.


#include "d3dtypes.h"
#include "sinit/AxisVectors.h"
#include "SimpleWreckCamera/SimpleWreckCamera.h"
#include "CameraBehavior/CameraBehavior.h"
#include "CameraFilter/CameraFilter.h"
#include "DebugOverlay/DebugOverlay.h"
#include "Game/Game.h"
#include "CameraVector/CameraVector.h"
#include "CameraStateParams/CameraStateParams.h"
#include "PhysicsBody/PhysicsBody.h"
#include "SoultreeObject/SoultreeObject.h"

// ---- src/SimpleWreckCamera/ctor.cpp





SimpleWreckCamera::SimpleWreckCamera()
{
    m_fov = 80.0f;
    m_zoom = 0;
    m_bodies = 0;
    m_objects = 0;
    m_lastPositions = 0;
    m_lookFilters[0] = 0;
    m_lookFilters[1] = 0;
    m_lookFilters[2] = 0;
    m_positionFilters[0] = 0;
    m_positionFilters[1] = 0;
    m_positionFilters[2] = 0;
    m_fovFilter = 0;
    m_distance = 0;
    m_distanceZoom = 0;
    m_x08 = 0;
    m_x0C = 0;
    m_x1F0 = 0;
    m_numBodies = 0;
    m_numTargets = 0;
    m_resetFilters = 0;
    m_watchingBodies = 1;
    m_copyFilter = 1;
    m_onlyFirst = 0;
    for (int i = 0; i < 10; i++)
        m_ignore[i] = 0;
}


// ---- src/SimpleWreckCamera/dtor.cpp
extern "C" void free(void *);






SimpleWreckCamera::~SimpleWreckCamera()
{
    if (m_lookFilters[0])
        delete m_lookFilters[0];
    if (m_lookFilters[1])
        delete m_lookFilters[1];
    if (m_lookFilters[2])
        delete m_lookFilters[2];
    if (m_positionFilters[0])
        delete m_positionFilters[0];
    if (m_positionFilters[1])
        delete m_positionFilters[1];
    if (m_positionFilters[2])
        delete m_positionFilters[2];
    if (m_fovFilter)
        delete m_fovFilter;
    if (m_objects)
        free(m_objects);
    if (m_bodies)
        free(m_bodies);
    if (m_lastPositions) {
        for (int i = 0; i < m_numBodies; i++) {
            if (m_lastPositions[i])
                delete m_lastPositions[i];
        }
    }
    if (m_lastPositions)
        free(m_lastPositions);
}


// ---- src/SimpleWreckCamera/Initialize.cpp
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);


struct WreckCameraParams {
    float m_x00[8];
};

// A smoothed value with optional limits.



int SimpleWreckCamera::Initialize(float fovRate, WreckCameraParams params, float lookRate, float positionRate, float x54)
{
    m_x54 = x54;
    m_fovFilter = new CameraFilter(m_fov, fovRate);
    m_lookFilters[0] = new CameraFilter(0, lookRate);
    m_lookFilters[1] = new CameraFilter(0, lookRate);
    m_lookFilters[2] = new CameraFilter(0, lookRate);
    m_positionFilters[0] = new CameraFilter(0, positionRate);
    m_positionFilters[1] = new CameraFilter(0, positionRate);
    m_positionFilters[2] = new CameraFilter(0, positionRate);
    _D3DVECTOR zero(0, 0, 0);
    ResetLookFilters(zero);
    ResetPositionFilters(zero);
    memcpy(&m_x2D0, &params, sizeof(WreckCameraParams));
    return 1;
}


// ---- src/SimpleWreckCamera/AddPhysicsBody.cpp
void PSXprintf(const char *fmt, ...);
extern "C" void *realloc(void *p, unsigned int size);

class PhysicsBody;


void SimpleWreckCamera::AddPhysicsBody(PhysicsBody *body)
{
    if (!m_watchingBodies && m_numBodies > 0) {
        PSXprintf("Already watching 1 or more SoultreeObjects, can't watch PhysicsBodies!\n");
        return;
    }
    m_bodies = (PhysicsBody **)realloc(m_bodies, (m_numBodies + 1) * sizeof(PhysicsBody *));
    m_bodies[m_numBodies] = body;
    m_numBodies++;
    m_numTargets = m_numBodies;
    m_watchingBodies = 1;
}


// ---- src/SimpleWreckCamera/RemoveAllPhysicsBodies.cpp
void PSXprintf(const char *fmt, ...);
extern "C" void free(void *p);

class PhysicsBody;
class SoultreeObject;


int SimpleWreckCamera::RemoveAllPhysicsBodies()
{
    if (!m_watchingBodies) {
        PSXprintf("Can't remove all PhysicsBodies, we're watching SoultreeObjects with this camera!\n");
    } else {
        if (m_bodies)
            free(m_bodies);
        m_bodies = 0;
        m_numBodies = 0;
        m_numTargets = m_numBodies;
        return 1;
    }
    return 0;
}


// ---- src/SimpleWreckCamera/RemoveAllSoultreeObjects.cpp
void PSXprintf(const char *fmt, ...);
extern "C" void free(void *p);

class PhysicsBody;
class SoultreeObject;


int SimpleWreckCamera::RemoveAllSoultreeObjects()
{
    if (m_watchingBodies) {
        PSXprintf("Can't remove all SoultreeObjects, we're watching PhysicsBodies with this camera!\n");
    } else {
        if (m_objects)
            free(m_objects);
        m_objects = 0;
        m_numBodies = 0;
        m_numTargets = m_numBodies;
        return 1;
    }
    return 0;
}


// ---- src/SimpleWreckCamera/ComputeCameraData.cpp
#pragma inline_depth(4)

// Per-file copies, as in retail (every file has its own ZeroVector).


extern Game *LpGame;
int GDbgOverlayID = -1;

int RotateAroundAxis(float angle, _D3DVECTOR axis, _D3DVECTOR v, _D3DVECTOR *out);

static inline float RSqrt(float x)
{
    float r;
    asm("rsqrt.s $f8, %0, %1; swc1 $f8, 0(%2)" : : "f"(1.0f), "f"(x), "r"(&r) : "$f8", "memory");
    return r;
}

// VU0 cross product, out = a x b.
static inline void CrossProduct(_D3DVECTOR *out, _D3DVECTOR *a, _D3DVECTOR *b)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vopmula.xyz ACC, vf4, vf5; vopmsub.xyz vf6, vf5, vf4; vsub.w vf6, vf6, vf6; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(a), "r"(b) : "memory");
}

void SimpleWreckCamera::ComputeCameraData(float dt, int &state, _D3DVECTOR *position, _D3DVECTOR *lookPoint,
                                          float *fov, _D3DVECTOR *a, _D3DVECTOR *b, float *c)
{
    _D3DVECTOR pos;
    _D3DVECTOR look;
    _D3DVECTOR dir;
    _D3DVECTOR velocity(0.0f, 0.0f, 0.0f);
    _D3DVECTOR target(0.0f, 0.0f, 0.0f);
    int moving;
    if (m_copyFilter && m_x1F0 > 0.0f)
        m_x1F0 -= dt;
    UpdateOffsets();
    float f = ComputeFOV();
    ComputeNetPositionAndVelocity(dt, &target, &velocity);
    if (velocity.x != 0.0f && velocity.y != 0.0f && velocity.z != 0.0f) {
        moving = 1;
        m_x2C0 = velocity;
    } else {
        if (m_x2C0 == ZeroVector)
            m_x2C0 = *a;
        velocity = m_x2C0;
        moving = 0;
    }
    float zoom = f / m_maxFOV;
    RotateAngleXAndY();
    _D3DVECTOR heading = velocity;
    heading.y = 0.0f;
    float len2 = heading.x * heading.x + heading.y * heading.y + heading.z * heading.z;
    if (len2 == 0.0f)
        heading = ZeroVector;
    else
        heading *= RSqrt(len2);
    _D3DVECTOR side;
    CrossProduct(&side, &heading, &YVector);
    RotateAroundAxis(m_x0C, side, heading, &pos);
    RotateAroundAxis(m_x08, YVector, pos, &dir);
    _D3DVECTOR step = dir - m_direction;
    step *= 0.1f;
    dir += step;
    m_direction = dir;
    look = ComputeLookPoint(target, velocity, zoom, moving);
    pos = ComputePosition(dir, target, velocity, look);
    FilterCameraParameters(&pos, &look, &f, dt);
    if (position)
        *position = pos;
    if (lookPoint)
        *lookPoint = look;
    if (fov)
        *fov = f;
    DebugOverlay *overlay = LpGame->m_debugOverlay;
    if (overlay) {
        if (GDbgOverlayID < 0) {
            // Spelled out (not m_numPages++): retail's registers need the named page.
            int page = overlay->m_numPages;
            overlay->m_numPages = page + 1;
            GDbgOverlayID = page;
        }
        LpGame->m_debugOverlay->PrintfHeading(GDbgOverlayID, "Wreck Camera Info");
        if (m_watchingBodies)
            LpGame->m_debugOverlay->PrintfDetail(GDbgOverlayID, "Viewing PhysicsBodies");
        else
            LpGame->m_debugOverlay->PrintfDetail(GDbgOverlayID, "Viewing SoultreeObjects");
        LpGame->m_debugOverlay->PrintfDetail(GDbgOverlayID, "AngleY: %f = %f degrees", m_x08, 57.29578f * m_x08);
        LpGame->m_debugOverlay->PrintfDetail(GDbgOverlayID, "AngleX: %f = %f degrees", m_x0C, 57.29578f * m_x0C);
        LpGame->m_debugOverlay->PrintfDetail(GDbgOverlayID, "ViewingDistance: %f", m_distance);
        LpGame->m_debugOverlay->PrintfDetail(GDbgOverlayID, "FOV: %f", f);
    }
}


// ---- src/SimpleWreckCamera/SwitchOn.cpp


static inline float Sign(float x)
{
    return x < 0.0f ? -1.0f : 1.0f;
}

void SimpleWreckCamera::SwitchOn(CameraStateParams params, int copyFilter)
{
    if (copyFilter) {
        m_fov = params.f34;
        m_x1F0 = 2.0f;
    } else {
        m_fov = m_minFOV;
    }
    m_x0C = params.f38;
    m_x08 = params.f3C;
    if (Sign(m_x08) != Sign(m_x2D0))
        m_x08 = -m_x08;
    m_copyFilter = copyFilter;
    if (!copyFilter || !CopyFilterData(params))
        m_resetFilters = 1;
    BeginWreck(params);
}


// ---- src/SimpleWreckCamera/SwitchOff.cpp



// Clears the ignore list and hands the filters on. The members are read
// before the loop (retail loads them all first).
CameraStateParams SimpleWreckCamera::SwitchOff()
{
    CameraStateParams params;
    int i;
    float f38 = m_x0C;
    float f3C = m_x08;
    CameraFilter *p40 = m_positionFilters[0];
    CameraFilter *p44 = m_positionFilters[1];
    CameraFilter *p48 = m_positionFilters[2];
    CameraFilter *p4C = m_lookFilters[0];
    CameraFilter *p50 = m_lookFilters[1];
    CameraFilter *p54 = m_lookFilters[2];
    CameraFilter *p58 = m_fovFilter;
    float fov;  // never set: retail stores whatever is in $f2 at +0x30/+0x34
    for (i = 0; i < m_numBodies; i++)
        m_ignore[i] = 0;
    m_numTargets = m_numBodies;
    params.f30 = fov;
    params.f34 = fov;
    params.f38 = f38;
    params.f3C = f3C;
    params.p40 = (float *)p40;
    params.p44 = (float *)p44;
    params.p48 = (float *)p48;
    params.p4C = (float *)p4C;
    params.p50 = (float *)p50;
    params.p54 = (float *)p54;
    params.p58 = (float *)p58;
    return params;
}


// ---- src/SimpleWreckCamera/CopyFilterData.cpp




// Copies each filter's value/target from the saved state; returns 0 if any
// filter is missing on either side.
int SimpleWreckCamera::CopyFilterData(CameraStateParams params)
{
    int ok = 1;
    if (m_positionFilters[0] && params.p40) {
        m_positionFilters[0]->m_value = params.p40[0];
        m_positionFilters[0]->m_scale = params.p40[2];
    } else
        ok = 0;
    if (m_positionFilters[1] && params.p44) {
        m_positionFilters[1]->m_value = params.p44[0];
        m_positionFilters[1]->m_scale = params.p44[2];
    } else
        ok = 0;
    if (m_positionFilters[2] && params.p48) {
        m_positionFilters[2]->m_value = params.p48[0];
        m_positionFilters[2]->m_scale = params.p48[2];
    } else
        ok = 0;
    if (m_lookFilters[0] && params.p4C) {
        m_lookFilters[0]->m_value = params.p4C[0];
        m_lookFilters[0]->m_scale = params.p4C[2];
    } else
        ok = 0;
    if (m_lookFilters[1] && params.p50) {
        m_lookFilters[1]->m_value = params.p50[0];
        m_lookFilters[1]->m_scale = params.p50[2];
    } else
        ok = 0;
    if (m_lookFilters[2] && params.p54) {
        m_lookFilters[2]->m_value = params.p54[0];
        m_lookFilters[2]->m_scale = params.p54[2];
    } else
        ok = 0;
    if (m_fovFilter && params.p58) {
        m_fovFilter->m_value = params.p58[0];
        m_fovFilter->m_scale = params.p58[2];
    } else
        ok = 0;
    return ok;
}


// ---- src/SimpleWreckCamera/BeginWreck.cpp

// Camera state handed from one camera behavior to the next (see
// src/global/SwitchOff.cpp).

static inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

// libvu0 sceVu0Normalize.
static inline void Normalize(_D3DVECTOR *out, _D3DVECTOR *in)
{
    asm("lqc2 vf4, 0x0(%1); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(in) : "memory");
}



// Start watching a wreck: aim from the previous camera position at the
// centroid of the wrecking objects.
void SimpleWreckCamera::BeginWreck(CameraStateParams params)
{
    _D3DVECTOR center(0.0f, 0.0f, 0.0f);
    ComputeDistancesBetweenObjects();
    for (int i = 0; i < m_numBodies; i++) {
        center.x += m_positions[i].x;
        center.y += m_positions[i].y;
        center.z += m_positions[i].z;
    }
    float scale = 1.0f / (float)m_numBodies;
    center.x *= scale;
    center.y *= scale;
    center.z *= scale;
    float dx = center.x - params.position.x;
    float dy = center.y - params.position.y;
    float dz = center.z - params.position.z;
    m_distance = Sqrt(dx * dx + dy * dy + dz * dz);
    m_direction = _D3DVECTOR(params.position.x - center.x, params.position.y - center.y, params.position.z - center.z);
    Normalize(&m_direction, &m_direction);
    m_lookPoint.x = m_lookFilters[0]->m_value;
    m_lookPoint.y = m_lookFilters[1]->m_value;
    m_lookPoint.z = m_lookFilters[2]->m_value;
    m_distanceZoom = 0;
    m_zoom = 0;
}


// ---- src/SimpleWreckCamera/ComputeNetPositionAndVelocity.cpp
#pragma inline_depth(6)


static inline float Fmax(float a, float b)
{
    float r;
    asm("max.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}



// Averages the bodies' positions (weighted toward the first body by how close
// they are to it) and velocities, skipping ignored and far-away bodies.
void SimpleWreckCamera::ComputeNetPositionAndVelocity(float dt, _D3DVECTOR *position, _D3DVECTOR *velocity)
{
    int skip[10];
    _D3DVECTOR v;
    int i;
    m_numTargets = 0;
    for (i = 0; i < m_numBodies; i++) {
        if (m_ignore[i]) {
            skip[i] = 1;
        } else if (i != 0) {
            float frac = 1.0f - (float)i / (float)(m_numBodies + 1);
            float weight = Fmax(1.0f - m_distances[0][i] / m_x54, 0.0f);
            if (weight <= 0.0f) {
                skip[i] = 1;
            } else {
                skip[i] = 0;
                m_numTargets++;
                _D3DVECTOR offset = m_positions[i] - m_positions[0];
                *position += m_positions[0] + (frac * weight) * offset;
            }
        } else {
            *position += m_positions[i];
            skip[i] = 0;
            m_numTargets++;
        }
        float speed;
        if (m_watchingBodies) {
            v = m_bodies[i]->m_velocity;
            speed = m_bodies[i]->m_speed;
        } else {
            if (!m_lastPositions[i]) {
                m_lastPositions[i] = new _D3DVECTOR;
                *m_lastPositions[i] = m_positions[i];
            }
            _D3DVECTOR *last = m_lastPositions[i];
            v = (m_positions[i] - *last) / dt;
            speed = Sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
            *m_lastPositions[i] = m_positions[i];
        }
        if (!skip[i]) {
            if (speed < 20.0f)
                v.y = 0;
            *velocity += v;
        }
    }
    *position *= 1.0f / m_numTargets;
    *velocity *= 1.0f / m_numTargets;
    m_onlyFirst = 1;
    for (i = 1; i < m_numBodies; i++) {
        if (!skip[i]) {
            m_onlyFirst = 0;
            break;
        }
    }
}


// ---- src/SimpleWreckCamera/ComputeDistancesBetweenObjects.cpp





void SimpleWreckCamera::ComputeDistancesBetweenObjects()
{
    int i;
    int j;
    for (i = 0; i < m_numBodies; i++) {
        if (m_watchingBodies)
            m_bodies[i]->m_object->GetPosition(0, &m_positions[i]);
        else
            m_objects[i]->GetPosition(0, &m_positions[i]);
        for (j = 0; j < m_numBodies; j++)
            m_distances[i][j] = 0.0f;
    }
    for (i = 0; i < m_numBodies; i++) {
        for (j = 1; j < m_numBodies; j++) {
            if (m_ignore[i])
                continue;
            if (m_ignore[j])
                continue;
            if (m_distances[j][i] != 0.0f) {
                m_distances[i][j] = m_distances[j][i];
                continue;
            }
            if (i == j)
                continue;
            float dx = m_positions[j].x - m_positions[i].x;
            float dy = m_positions[j].y - m_positions[i].y;
            float dz = m_positions[j].z - m_positions[i].z;
            float d = Sqrt(dx * dx + dy * dy + dz * dz);
            m_distances[i][j] = d;
            m_distances[j][i] = m_distances[i][j];
        }
    }
}


// ---- src/SimpleWreckCamera/UpdateOffsets.cpp
// UpdateOffsets__17SimpleWreckCameraFv  @ 0x0033A7F0
// The asm helpers turn the global optimizer off, so FP registers come from
// the creation order of values: `dist`, `range`, `ratio` and `half` as named
// locals and the Ratio helper (its arguments are evaluated right to left, so
// the denominator first) reproduce retail's colouring, with `growth` in $f6
// after the zoom temporaries. The dead then-arm gives retail's
// `c.le.s growth,1; bc1t body; b end` shape.

static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

float FastTan(float x);

static inline float Ratio(float n, float d) { return n / d; }


void SimpleWreckCamera::UpdateOffsets()
{
    int i;
    int j;
    float zoomDelta = 0;
    float distanceDelta = 0;
    float blend = 0;
    if (m_copyFilter) {
        zoomDelta = 0.2f * (m_x2DC - (m_fov + m_zoom));
        distanceDelta = 0.03f * (m_x2D8 - m_distance);
        blend = m_x1F0 / 2.0f;
        if (blend < 0.0f) {
            blend = 0.0f;
            m_x1F0 = 0;
        }
    }
    float oldSpread = 0;
    float spread = 0;
    if (m_numTargets > 1) {
        for (i = 0; i < m_numBodies; i++) {
            for (j = 1; j < m_numBodies; j++) {
                if (m_ignore[i])
                    continue;
                if (m_ignore[j])
                    continue;
                if (!(m_distances[i][j] <= oldSpread))
                    oldSpread = m_distances[i][j];
            }
        }
    }
    ComputeDistancesBetweenObjects();
    float growth;
    if (m_numTargets > 1) {
        for (i = 0; i < m_numBodies; i++) {
            for (j = 1; j < m_numBodies; j++) {
                if (m_ignore[i])
                    continue;
                if (m_ignore[j])
                    continue;
                if (!(m_distances[i][j] <= spread))
                    spread = m_distances[i][j];
            }
        }
        growth = spread - oldSpread;
    } else {
        spread = 1.5f * m_minDistance;
        growth = 0;
    }
    float zoomChange = (1.0f - blend) * (growth / 10.0f) + blend * zoomDelta;
    if (Fabs(zoomChange) < 0.1f && !(m_zoom <= 0.0f)) {
        float dist = m_minDistance + m_distanceZoom;
        float range = m_maxFOV - m_minFOV;
        float ratio = Ratio(m_maxDistance - dist, m_maxDistance - m_minDistance);
        m_zoom = m_minFOV + range * ratio - m_fov;
    } else {
        m_zoom += zoomChange;
    }
    float delta = (1.0f - blend) * (growth / 10.0f) + blend * distanceDelta;
    if (growth > 1.0f) {
        int unused = 1;     // dead arm, kept for retail's branch shape
    } else {
        float fov = Fmax(m_fov + m_zoom - 20.0f, m_minFOV);
        float half = 0.5f * fov;
        float error = spread / (2.0f * FastTan(0.017453291f * half)) - (m_minDistance + m_distanceZoom);
        if (!(error <= 0.0f) || -error < m_distanceZoom)
            delta += 0.05f * error;
    }
    m_distanceZoom += delta;
}


// ---- src/SimpleWreckCamera/ResetLookFilters.cpp

void SimpleWreckCamera::ResetLookFilters(_D3DVECTOR &v)
{
    if (m_lookFilters[0])
        m_lookFilters[0]->m_value = v.x;
    if (m_lookFilters[1])
        m_lookFilters[1]->m_value = v.y;
    if (m_lookFilters[2])
        m_lookFilters[2]->m_value = v.z;
}


// ---- src/SimpleWreckCamera/ResetPositionFilters.cpp

void SimpleWreckCamera::ResetPositionFilters(_D3DVECTOR &v)
{
    if (m_positionFilters[0])
        m_positionFilters[0]->m_value = v.x;
    if (m_positionFilters[1])
        m_positionFilters[1]->m_value = v.y;
    if (m_positionFilters[2])
        m_positionFilters[2]->m_value = v.z;
}

// (From BuoyancyModel/Create.cpp, a cross-TU stub.)
int SimpleWreckCamera::ConstrainCamera(_D3DVECTOR *position, int a)
{
    return 1;
}


// ---- src/SimpleWreckCamera/ComputeFOV.cpp

float SimpleWreckCamera::ComputeFOV()
{
    float fov = m_fov + m_zoom;
    if (fov > m_maxFOV) {
        fov = m_maxFOV;
        m_zoom = m_maxFOV - m_minFOV;
    } else {
        float minFOV = m_minFOV;
        if (fov < minFOV) {
            m_zoom = 0;
            fov = minFOV;
        }
    }
    return fov;
}


// ---- src/SimpleWreckCamera/RotateAngleXAndY.cpp

void SimpleWreckCamera::RotateAngleXAndY()
{
    float c = 0.05f;
    m_x0C += c * (m_x2D4 - m_x0C);
    m_x08 += c * (m_x2D0 - m_x08);
}


// ---- src/SimpleWreckCamera/ComputeLookPoint.cpp
#pragma inline_depth(6)



_D3DVECTOR SimpleWreckCamera::ComputeLookPoint(_D3DVECTOR position, _D3DVECTOR velocity, float dt, int moving)
{
    _D3DVECTOR lookPoint;
    _D3DVECTOR offset(0.0f, 0.0f, 0.0f);
    if (m_onlyFirst && m_numTargets > 1) {
        _D3DVECTOR between = m_positions[1] - m_positions[0];
        offset = (1.0f - m_x54 / m_distances[0][1]) * between;
    }
    if (m_copyFilter) {
        float blend = m_x1F0 / 2.0f;
        blend = Fmax(0.0f, blend);
        _D3DVECTOR lead = 0.2f * (m_lookPoint - position);
        if (moving) {
            if (m_numTargets > 1)
                lookPoint = position + velocity + blend * lead + offset;
            else
                lookPoint = position + 0.76f * velocity + blend * lead + offset;
        } else {
            lookPoint = position + blend * lead + offset;
        }
    } else if (moving) {
        lookPoint = position + 0.2f * velocity + offset;
    } else {
        lookPoint = position + offset;
    }
    m_lookPoint = lookPoint;
    return lookPoint;
}


// ---- src/SimpleWreckCamera/ComputePosition.cpp
// The chained `target + m_distance * direction` needs a deeper inline depth
// than mwcc's default to keep the _D3DVECTOR ctor inline.
#pragma inline_depth(4)


// libvu0 sceVu0Normalize.


// The camera position: back from target along direction (bent towards the
// line between the first two targets) by the clamped zoomed distance.
_D3DVECTOR SimpleWreckCamera::ComputePosition(_D3DVECTOR direction, _D3DVECTOR target, _D3DVECTOR, _D3DVECTOR)
{
    _D3DVECTOR position;
    _D3DVECTOR offset(0.0f, 0.0f, 0.0f);
    if (m_onlyFirst && m_numTargets > 1) {
        _D3DVECTOR between = m_positions[0] - m_positions[1];
        offset = (1.0f - m_x54 / m_distances[0][1]) * between;
    }
    m_distance = m_minDistance + m_distanceZoom;
    if (m_distance < m_minDistance) {
        m_distance = m_minDistance;
        m_distanceZoom = 0;
    } else if (!(m_distance <= m_maxDistance)) {
        m_distance = m_maxDistance;
        m_distanceZoom = m_maxDistance - m_minDistance;
    }
    direction += offset;
    Normalize(&direction, &direction);
    position = target + m_distance * direction;
    return position;
}


// ---- src/SimpleWreckCamera/FilterCameraParameters.cpp

static inline float Fmin(float a, float b)
{
    float r;
    asm("min.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

// A smoothed value with optional limits.

struct CameraStateParams;
struct CameraJoystickInput;





// CameraFilter's inline update, as the per-function file's view declared it.
static inline float UpdateFilter(CameraFilter *f, float dt, float target)
{
    f->m_scale = Fmin(dt, f->m_rate) / f->m_rate;
    f->m_value = f->m_value + f->m_scale * (target - f->m_value);
    return f->m_value;
}

void SimpleWreckCamera::FilterCameraParameters(_D3DVECTOR *position, _D3DVECTOR *look, float *fov, float dt)
{
    _D3DVECTOR newLook;
    _D3DVECTOR newPosition;

    if (!m_resetFilters) {
        newLook.x = UpdateFilter(m_lookFilters[0], dt, look->x);
        newLook.y = UpdateFilter(m_lookFilters[1], dt, look->y);
        newLook.z = UpdateFilter(m_lookFilters[2], dt, look->z);
        newPosition.x = UpdateFilter(m_positionFilters[0], dt, position->x);
        newPosition.y = UpdateFilter(m_positionFilters[1], dt, position->y);
        newPosition.z = UpdateFilter(m_positionFilters[2], dt, position->z);
        float newFov = UpdateFilter(m_fovFilter, dt, *fov);
        ConstrainCamera(&newPosition, 1);
        *position = newPosition;
        *look = newLook;
        *fov = newFov;
    } else {
        ConstrainCamera(position, 1);
        ResetLookFilters(*look);
        ResetPositionFilters(*position);
        m_resetFilters = 0;
    }
}

