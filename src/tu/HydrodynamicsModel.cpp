// HydrodynamicsModel.cpp  @ 0x002F2110 (text 0x2F2110-0x2F4620)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's HydrodynamicsModel.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the object's
// data as well as its code (tools/link_c.py).



#include "d3dtypes.h"
#include "GameObject/GameObject.h"
#include "HydrodynamicsModel/HydrodynamicsModel.h"
#include "PhysicsBody/PhysicsBody.h"
#include "SoultreeObject/SoultreeObject.h"
#include "Water/Water.h"
#include "PSXWater/PSXWater.h"
#include "sinit/AxisVectors.h"

void PSXprintf(const char *fmt, ...);

// ---- src/HydrodynamicsModel/ctor.cpp


HydrodynamicsModel::HydrodynamicsModel(int id) : GameObject(id)
{
    m_body = 0;
    m_hullPoints = 0;
    m_lastHullPoint = 0;
    m_wakePoints = 0;
    m_lastWakePoint = 0;
    m_frothPoints = 0;
    m_lastFrothPoint = 0;
    m_numHullPoints = m_maxHullPoints = 0;
    m_numWakePoints = m_maxWakePoints = 0;
    m_numFrothPoints = m_maxFrothPoints = 0;
    m_x38 = 0.0f;
    m_x3C = -1.0f;
    m_x48 = 10.0f;
    m_x4C = 20.0f;
    m_x50 = 0.1f;
    m_x44 = 2.0f;
    m_x40 = 0.0f;
    m_x54 = 350.0f;
    m_x58 = 0.0f;
    m_x5C = 1.0f;
    m_x60 = 1.0f;
    m_x6C = m_x5C * m_x60;
    m_x64 = 0.1f;
    m_x68 = 0.05f;
    m_x70 = 0.5f;
    m_x74 = 0.1f;
    m_x78 = 0.5f;
    m_x7C = 0.1f;
    m_x80 = 0.1f;
    m_x84 = 20.0f;
    m_x88 = 1.0f;
    m_xA0 = 0.0f;
    m_xA4 = 0.0f;
    m_xA8 = 2.0f;
    m_xB0 = 0.0f;  // retail sets m_xB0 twice
    m_xB8 = 2.0f;
    m_xBC = 20.0f;
    m_xAC = 1.0f;
    m_xB0 = 1.0f;
    m_xB4 = 1.0f;
    m_xC0 = 0.1f;
    m_xC4 = -50.0f;
    m_xC8 = -150.0f;
    m_xCC = -50.0f;
    m_wakeStrength = 0.0f;
    m_x90 = ZeroVector;
    m_xF0 = ZeroVector;
    m_force = ZeroVector;
    m_torque = ZeroVector;
    m_xD8 = 0;
    m_x30 = 0.0f;
}


// ---- src/HydrodynamicsModel/Create.cpp


HydrodynamicsModel *HydrodynamicsModel::Create(PSXWater *water, PhysicsBody *body, long maxHullPoints,
                                               long maxWakePoints, long maxFrothPoints)
{
    if (!water) {
        PSXprintf("HydrodynamicsModel::Create FAILED. You must supply a valid water.\n");
        return 0;
    }
    m_water = water;
    if (!body) {
        PSXprintf("HydrodynamicsModel::Create FAILED. You must supply a valid physics body.\n");
        return 0;
    }
    m_body = body;
    m_maxHullPoints = maxHullPoints;
    m_hullPoints = new _D3DVECTOR[(int)m_maxHullPoints];
    m_maxWakePoints = maxWakePoints;
    m_wakePoints = new HydroPoint[(int)m_maxWakePoints];
    m_maxFrothPoints = maxFrothPoints;
    m_frothPoints = new HydroPoint[(int)m_maxFrothPoints];
    // The redundant assignments reproduce retail's unfilled delay slot.
    HydrodynamicsModel *ret = this;
    if (!GameObject::Create())
        ret = 0;
    else
        ret = this;
    return ret;
}


// ---- src/HydrodynamicsModel/dtor.cpp
HydrodynamicsModel::~HydrodynamicsModel()
{
    if (m_hullPoints)
        delete[] m_hullPoints;
    if (m_wakePoints)
        delete[] m_wakePoints;
}


// ---- src/HydrodynamicsModel/Tick.cpp


int HydrodynamicsModel::Tick(float dt)
{
    if (!m_x30)
        SimulationTick(dt);
    return GameObject::Tick(dt);
}


// ---- src/HydrodynamicsModel/ComputeForceTick.cpp
// Water forces on the body for one tick: a spring/damper push along the local
// water normal at every submerged hull point, then drag, planing friction, lift,
// pitch and aligning torques interpolated between the air and water values by
// the hull's depth (m_x40), deck drag, anti-skid and angular damping.
// Asm helpers (Sqrt, Fmin, Fmax, Fabs, VU0 cross/normalize): optimizer off.
// Helper result slots follow inline depth: SquareRoot -> Sqrt puts the damping
// root after the level-1 helpers, Length -> SquareRoot -> Sqrt the speed root
// after that. inline_depth(6) keeps the vector operators inline.
// All 27 function-level vectors are declared first: retail default-constructs
// them in the prologue in address order.
#pragma inline_depth(6)


float gMinWakeSpeed = 10.0f;
float gMaxWakeSpeed = 100.0f;
float gMaxWakeAngularRate = 3.0f;
float gMaxAngularRateScale = 0.2f;
float FastAtan2(float y, float x);

static inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

static inline float Fmin(float a, float b)
{
    float r;
    asm("min.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

static inline float Fmax(float a, float b)
{
    float r;
    asm("max.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

static inline float SquareRoot(float x)
{
    return Sqrt(x);
}

static inline float Length(_D3DVECTOR &v)
{
    return SquareRoot(v.x * v.x + v.y * v.y + v.z * v.z);
}

// libvu0 sceVu0OuterProduct: a x b.
static inline _D3DVECTOR CrossProduct(_D3DVECTOR &a, _D3DVECTOR &b)
{
    _D3DVECTOR out;
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vopmula.xyz ACC, vf4, vf5; vopmsub.xyz vf6, vf5, vf4; vsub.w vf6, vf6, vf6; sqc2 vf6, 0x0(%0)"
        : : "r"(&out), "r"(&a), "r"(&b) : "memory");
    return out;
}

static inline void CrossProduct(_D3DVECTOR *out, _D3DVECTOR *a, _D3DVECTOR *b)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vopmula.xyz ACC, vf4, vf5; vopmsub.xyz vf6, vf5, vf4; vsub.w vf6, vf6, vf6; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(a), "r"(b) : "memory");
}

// libvu0 sceVu0Normalize, in place.
static inline void Normalize(_D3DVECTOR *v)
{
    asm("lqc2 vf4, 0x0(%0); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(v) : "memory");
}

static inline _D3DVECTOR Normalize(_D3DVECTOR &v)
{
    _D3DVECTOR out;
    asm("lqc2 vf4, 0x0(%1); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(&out), "r"(&v) : "memory");
    return out;
}

inline _D3DVECTOR operator-(const _D3DVECTOR &v)
{
    return _D3DVECTOR(-v.x, -v.y, -v.z);
}

// y0 at x0 to y1 at x1, clamped to the y range; 0 when x0 == x1.
static inline float Interpolate(float x, float x0, float x1, float y0, float y1)
{
    if (x1 != x0) {
        float y = y0 + (x - x0) * (y1 - y0) / (x1 - x0);
        if (y1 > y0) {
            if (y > y1)
                y = y1;
            else if (y < y0)
                y = y0;
        } else {
            if (y < y1)
                y = y1;
            else if (y > y0)
                y = y0;
        }
        return y;
    }
    return 0.0f;   // (the early return inside the if gives retail's join blocks)
}

int HydrodynamicsModel::ComputeForceTick(float dt)
{
    _D3DVECTOR surface, waterVelocity, normal, up, velocity, probe1, probe2, probe3, probe4, edge1, edge2,
        edge3, edge4, normal1, normal2, normal3, normal4, point, arm, pointVelocity, relative, force,
        localForce, lever, torque, wake, froth;
    SoultreeObject *object = m_body->m_object;

    m_force = ZeroVector;
    m_torque = ZeroVector;
    up.x = 0.0f;
    up.y = m_x3C;
    up.z = 0.0f;
    _D3DVECTOR center = object->TransformVectorLocalToWorld(&up);
    surface = center;
    m_xF0 = _D3DVECTOR();
    m_water->GetGridPointIntersection(&surface, 0);
    m_x120 = surface.y;
    m_x40 = surface.y - center.y;
    float bottom = -m_x3C;
    velocity = m_body->m_velocity - m_xF0;
    float speed = Length(velocity);
    float speedSq = speed * speed;
    if (m_x40 > 0.0f) {
        probe1.x = center.x;
        probe1.y = center.y;
        probe1.z = center.z + 1.0f;
        probe2.x = center.x + 1.0f;
        probe2.y = center.y;
        probe2.z = center.z;
        probe3.x = center.x;
        probe3.y = center.y;
        probe3.z = center.z - 1.0f;
        probe4.x = center.x - 1.0f;
        probe4.y = center.y;
        probe4.z = center.z;
        m_water->GetGridPointIntersection(&probe1, 0);
        m_water->GetGridPointIntersection(&probe2, 0);
        m_water->GetGridPointIntersection(&probe3, 0);
        m_water->GetGridPointIntersection(&probe4, 0);
        edge1 = probe1 - surface;
        edge2 = probe2 - surface;
        edge3 = probe3 - surface;
        edge4 = probe4 - surface;
        CrossProduct(&normal1, &edge1, &edge2);
        CrossProduct(&normal2, &edge2, &edge3);
        CrossProduct(&normal3, &edge3, &edge4);
        CrossProduct(&normal4, &edge4, &edge1);
        Normalize(&normal1);
        Normalize(&normal2);
        Normalize(&normal3);
        Normalize(&normal4);
        normal = normal1 + normal2 + normal3 + normal4;
        float surf = Interpolate(speed, m_x48, m_x4C, 0.0f, 1.0f);
        normal.x *= surf;
        normal.z *= surf;
        Normalize(&normal);
    } else {
        normal.x = 0.0f;
        normal.z = 0.0f;
        normal.y = 1.0f;
    }
    float spring;
    if (m_xD8)
        spring = Interpolate(speed, 0.0f, m_x5C, m_x54, m_x60);
    else
        spring = m_x54;
    float damping = 2.0f * m_x64 * SquareRoot(m_body->m_mass * spring);
    m_inWater = 0;
    m_outOfWater = 1;
    for (_D3DVECTOR *hull = m_hullPoints; hull <= m_lastHullPoint; hull++) {
        point = object->TransformVectorLocalToWorld(hull);
        arm = CrossProduct(m_body->m_xB0, *hull);
        pointVelocity = m_body->m_velocity + object->RotateVectorLocalToWorld(&arm);
        surface = point;
        m_water->GetGridPointIntersection(&surface, 0);
        float depth = surface.y - point.y;
        depth = Fmin(depth, m_x44);     // one variable: the difference and the result share a register
        if (depth > 0.0f) {
            m_inWater = 1;
            relative = pointVelocity - waterVelocity;
            float rise = relative.y;
            if (m_x38 > 0.0f && depth > m_x58 && rise >= 0.0f)
                m_xD8 = 1;
            else if (rise < 0.0f)
                m_xD8 = 0;
            float push = spring * depth - damping * rise;
            push = Fmax(push, 0.0f);
            force = push * normal;
            localForce = object->RotateVectorWorldToLocal(&force);
            lever.x = hull->x - m_body->m_centerOfMass.x;
            lever.y = 0.0f;
            lever.z = hull->z - m_body->m_centerOfMass.z;
            CrossProduct(&torque, &lever, &localForce);
            m_force += force;
            m_torque += torque;
        } else {
            m_outOfWater = 0;
        }
    }
    if (m_inWater) {
        HydroPoint *p = m_wakePoints;
        m_wakeStrength = Interpolate(speed, gMinWakeSpeed, gMaxWakeSpeed, 0.0f, 1.0f);
        float rate = Fabs(m_body->m_xA0.y);
        float turn = Interpolate(rate, 0.0f, gMaxWakeAngularRate, 0.0f, 1.0f);
        m_wakeStrength = m_wakeStrength + Fmin(turn, gMaxAngularRateScale);
        m_wakeStrength = Fmin(Fmax(m_wakeStrength, 0.0f), 1.0f);
        for (; p <= m_lastWakePoint; p++) {
            wake = object->TransformVectorLocalToWorld(&p->m_position);
            m_water->ApplyExternalForce(m_wakeStrength * p->m_strength, wake);
        }
        for (HydroPoint *q = m_frothPoints; q <= m_lastFrothPoint; q++) {
            froth = object->TransformVectorLocalToWorld(&q->m_position);
            m_water->ApplyWakeFroth(froth, q->m_strength, m_body->m_velocity);
        }
    }
    _D3DVECTOR localVelocity = object->RotateVectorWorldToLocal(&velocity);
    float drag = Interpolate(m_x40, 0.0f, bottom, m_x74, m_x70);
    float turnDrag = Interpolate(m_x40, 0.0f, bottom, m_x7C, m_x78);
    _D3DVECTOR direction = -Normalize(velocity);
    float planing;
    if (m_x40 < 0.0f)
        planing = 0.0f;     // reuses the compare's 0.0 register: an empty arm, as in retail
    else
        planing = Fmin(Fmax(0.5f * speed, -1.0f), 1.0f);
    _D3DVECTOR friction = 32.2f * (m_body->m_mass * (planing * m_x68)) * direction;
    m_force += friction;
    float lift = Interpolate(m_x40, 0.0f, bottom, m_x88, m_x84);
    float angle = FastAtan2(-localVelocity.y, localVelocity.z);
    angle = Fmin(Fmax(angle, -m_x80), m_x80);
    float pitch = Interpolate(m_x40, 0.0f, bottom, m_xA4, m_xA0);
    _D3DVECTOR deck = object->TransformVectorLocalToWorld(&m_x90);
    _D3DVECTOR deckSurface = deck;
    int hit;
    m_water->GetGridPointIntersection(&deckSurface, &hit);
    if (hit > 0) {
        float deckDepth = Fmax(Fmin(deckSurface.y - deck.y, m_xB8), 0.0f);
        drag += deckDepth * m_xB4;
    }
    _D3DVECTOR liftForce, liftWorld, dragForce;
    liftForce.x = 0.0f;
    liftForce.y = speedSq * (lift * angle);
    liftForce.z = 0.0f;
    dragForce = speedSq * (drag + turnDrag * Fabs(localVelocity.x)) * direction;
    liftWorld = object->RotateVectorLocalToWorld(&liftForce);
    m_force += liftWorld;
    m_force += dragForce;
    _D3DVECTOR pitchTorque;
    pitchTorque.x = speedSq * (pitch + m_xA8 * angle);
    pitchTorque.y = 0.0f;
    pitchTorque.z = 0.0f;
    m_torque += pitchTorque;
    _D3DVECTOR skid, skidWorld;
    if (!m_body->m_x30) {
        skid.x = -m_xC0 * localVelocity.x * Fmin(speedSq, 5300.0f);
        skidWorld = object->RotateVectorLocalToWorld(&skid);
        m_force += skidWorld;
    }
    _D3DVECTOR aligning;
    float moment = Interpolate(m_x40, 0.0f, bottom, 0.0f, m_aligningMoment);
    aligning.y = speedSq * (moment * localVelocity.x);
    m_torque += aligning;
    if (!m_body->m_x30) {
        _D3DVECTOR damp;
        damp.x = m_xC8 * m_body->m_xB0.x;
        damp.y = m_xCC * m_body->m_xB0.y;
        damp.z = m_xC4 * m_body->m_xB0.z;
        m_torque += damp;
    }
    return 1;
}


// ---- src/HydrodynamicsModel/SetForceTick.cpp
// (Was `#pragma inline_depth(reset)`, which mwcc rejects as an illegal
// pragma and ignores: inline_depth is effectively TU-wide)

int HydrodynamicsModel::SetForceTick(float dt)
{
    m_body->AddForce(m_force);      // by value: retail's stack copy
    m_body->AddTorque(m_torque);
    return 1;
}


// ---- src/HydrodynamicsModel/UpdateWater.cpp
#pragma inline_depth(4)  // Length -> Sqrt, CrossProduct


float OneOverWakeSpeedDelta = 0.028571429f;

static inline _D3DVECTOR LocalToWorld(SoultreeObject *object, _D3DVECTOR *v)
{
    return object->TransformVectorLocalToWorld(v);
}

// Finds which hull points are under the water surface and, when any is,
// pushes the water with the wake points (scaled by speed) and adds froth.
int HydrodynamicsModel::UpdateWater(float dt)
{
    _D3DVECTOR surface;
    _D3DVECTOR unused1;                 // constructed, never used (retail)
    _D3DVECTOR unused2;
    _D3DVECTOR up;
    _D3DVECTOR velocity;
    _D3DVECTOR point;
    _D3DVECTOR arm;
    _D3DVECTOR pointVelocity;
    _D3DVECTOR wake;
    _D3DVECTOR froth;
    SoultreeObject *object = m_body->m_object;

    up = _D3DVECTOR(0.0f, m_x3C, 0.0f);
    _D3DVECTOR center = object->TransformVectorLocalToWorld(&up);
    surface = center;
    m_xF0 = ZeroVector;
    m_water->GetGridPointIntersection(&surface, 0);
    velocity = m_body->m_velocity - m_xF0;
    float speed = Length(velocity);
    m_inWater = 0;
    m_outOfWater = 1;
    _D3DVECTOR *hull = m_hullPoints;
    do {                                // no initial test in retail
        point = object->TransformVectorLocalToWorld(hull);
        arm = CrossProduct(m_body->m_xB0, *hull);
        pointVelocity = m_body->m_velocity + object->RotateVectorLocalToWorld(&arm);
        surface = point;
        m_water->GetGridPointIntersection(&surface, 0);
        float depth = surface.y;
        depth -= point.y;               // in two steps: retail's FP registers
        depth = Fmin(depth, m_x44);
        if (depth > 0.0f)
            m_inWater = 1;
        else
            m_outOfWater = 0;
        hull++;
    } while (hull <= m_lastHullPoint);
    if (m_inWater) {
        HydroPoint *p = m_wakePoints;
        m_wakeStrength = (speed - gMinWakeSpeed) * OneOverWakeSpeedDelta;
        if (m_wakeStrength < 0.0f)
            m_wakeStrength = 0.0f;
        else if (m_wakeStrength > 1.0f)
            m_wakeStrength = 1.0f;
        do {
            wake = object->TransformVectorLocalToWorld(&p->m_position);
            m_water->ApplyExternalForce(m_wakeStrength * p->m_strength, wake);
            p++;
        } while (p <= m_lastWakePoint);
        for (HydroPoint *q = m_frothPoints; q <= m_lastFrothPoint; q++) {
            froth = object->TransformVectorLocalToWorld(&q->m_position);
            m_water->ApplyWakeFroth(froth, q->m_strength, m_body->m_velocity);
        }
    }
    return 1;
}


// ---- src/HydrodynamicsModel/ResetHullPoints.cpp holds the three Reset*
// setters (and JetskiAirControlModel::SetBody, now in that TU); retail
// interleaves them with the Add* functions.
// (Was `#pragma inline_depth(reset)`, which mwcc rejects as an illegal
// pragma and ignores: inline_depth is effectively TU-wide)

void HydrodynamicsModel::ResetHullPoints()
{
    m_numHullPoints = 0;
}


// ---- src/HydrodynamicsModel/AddHullPoint.cpp


void HydrodynamicsModel::AddHullPoint(_D3DVECTOR point)
{
    if (m_numHullPoints < m_maxHullPoints) {
        m_hullPoints[m_numHullPoints] = point;
        m_lastHullPoint = &m_hullPoints[m_numHullPoints];
        m_numHullPoints++;
    } else {
        PSXprintf("HydrodynamicsModel::AddHullPoint FAILED. Not enough hull points allocated during Create.\n");
    }
}


void HydrodynamicsModel::ResetWakePoints()
{
    m_numWakePoints = 0;
}


// ---- src/HydrodynamicsModel/AddWakePoint.cpp


void HydrodynamicsModel::AddWakePoint(_D3DVECTOR point, float strength)
{
    if (m_numWakePoints < m_maxWakePoints) {
        m_wakePoints[m_numWakePoints].m_position = point;
        m_wakePoints[m_numWakePoints].m_strength = strength;
        m_lastWakePoint = &m_wakePoints[m_numWakePoints];
        m_numWakePoints++;
    } else {
        PSXprintf("HydrodynamicsModel::AddWakePoint FAILED. Not enough wake points allocated during Create.\n");
    }
}


void HydrodynamicsModel::ResetFrothPoints()
{
    m_numFrothPoints = 0;
}


// ---- src/HydrodynamicsModel/AddFrothPoint.cpp


void HydrodynamicsModel::AddFrothPoint(_D3DVECTOR point, float strength)
{
    if (m_numFrothPoints < m_maxFrothPoints) {
        m_frothPoints[m_numFrothPoints].m_position = point;
        m_frothPoints[m_numFrothPoints].m_strength = strength;
        m_lastFrothPoint = &m_frothPoints[m_numFrothPoints];
        m_numFrothPoints++;
    } else {
        PSXprintf("HydrodynamicsModel::AddFrothPoint FAILED. Not enough froth points allocated during Create.\n");
    }
}


// ---- src/HydrodynamicsModel/Render3D.cpp


int HydrodynamicsModel::Render3D()
{
    return GameObject::Render3D();
}


// ---- src/sinit/HydrodynamicsModel.cpp
// Static initializer of HydrodynamicsModel.cpp: only the axis vectors from the shared header.

