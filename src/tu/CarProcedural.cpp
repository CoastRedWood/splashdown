// CarProcedural.cpp  @ 0x002155C0 (text 0x2155C0-0x2185D0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's CarProcedural.cpp as one translation unit: its functions in retail
// order, then its static initializer. `make link` places the object's data as
// well as its code (tools/link_c.py). PhysicsBody's four setters at the end
// are defined here too. The old files' views map onto the shared headers:
// +0x2B4 is the Spline (Spline.h; the dtor and LoadCarProceduralBinary called
// it a CarProceduralShared), +0x4A8 the tire array.




// inline_depth is TU-wide (set once): 5 and 6 both match; 4 leaves
// InterpolateSpline's _D3DVECTOR(float, float, float) out of line.
#pragma inline_depth(5)

#include "GameObject/GameObject.h"
#include "d3dtypes.h"
#include "CarProcedural/CarProcedural.h"
#include "Spline/Spline.h"
#include "DataAccess/DataAccess.h"
#include "soultree_assert_block.h"
#include "PSXRenderableObject/PSXRenderableObject.h"
#include "CollisionObject/CollisionObject.h"
#include "VertexRayToFaceIntersectionData/VertexRayToFaceIntersectionData.h"
#include "PhysicsBody/PhysicsBody.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"
#include "SoultreeObject/SoultreeObject.h"
#include "Terrain/Terrain.h"
#include "TriggerObject/Vu0Orient.h"

extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" int sscanf(const char *s, const char *fmt, ...);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);
extern "C" int strncmp(const char *a, const char *b, unsigned int n);
extern "C" char *strstr(const char *s, const char *find);
extern "C" double fmod(double x, double y);
void PSXprintf(const char *fmt, ...);
float FastAtan2(float y, float x);
void FastSinCos(float angle, float *s, float *c);
void GetOrientationVectors(_D3DVECTOR *front, _D3DVECTOR *up, float yaw, float pitch, float roll);
_D3DMATRIX IdentityMatrix();

extern DataAccess *LpDataAccess;
extern char CollisionContentDirectory[];
extern char AnimContentDirectory[];
extern unsigned int WORLDOBJECT_PHYSICSBODY;

// One knot as LoadCarProceduralBinary reads it (32 bytes, no constructor:
// retail's new[] there has no array header or constructor loop).
struct CarProceduralPoint {
    float m_values[8];
};

static inline float RSqrt(float x)
{
    float r;
    asm("rsqrt.s $f8, %0, %1; swc1 $f8, 0(%2)" : : "f"(1.0f), "f"(x), "r"(&r) : "$f8", "memory");
    return r;
}

static inline float Length(const _D3DVECTOR &v)
{
    float len2 = v.x * v.x + v.y * v.y + v.z * v.z;
    if (len2 == 0.0f)
        return 0.0f;
    return 1.0f / RSqrt(len2);
}

static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
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

// VU0 cross product, out = a x b.
static inline void CrossProduct(_D3DVECTOR *out, _D3DVECTOR *a, _D3DVECTOR *b)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vopmula.xyz ACC, vf4, vf5; vopmsub.xyz vf6, vf5, vf4; vsub.w vf6, vf6, vf6; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(a), "r"(b) : "memory");
}

// VU0 xyz dot product.
static inline float DotProduct(_D3DVECTOR *a, _D3DVECTOR *b)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2 $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(a), "r"(b) : "$2");
    return r;
}

// out = m's 3x3 part * v (w ends up 1), v passed by value.
static inline void ApplyMatrixVector(_D3DVECTOR *out, _D3DMATRIX *m, _D3DVECTOR v)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x10(%1); lqc2 vf6, 0x20(%1); lqc2 vf8, 0x0(%2); vmulax.xyz ACC, vf4, vf8x; vmadday.xyz ACC, vf5, vf8y; vmaddz.xyz vf9, vf6, vf8z; vadd.xyzw vf9, vf9, vf0; sqc2 vf9, 0x0(%0)"
        : : "r"(out), "r"(m), "r"(&v) : "memory");
}

// out = transpose(rotation of m) * v: a world direction into m's space.
static inline void RotateWorldToLocal(_D3DVECTOR *out, _D3DMATRIX *m, _D3DVECTOR v)
{
    asm("lqc2 vf1, 0x0(%1); lqc2 vf2, 0x10(%1); lqc2 vf3, 0x20(%1); lqc2 vf4, 0x30(%1); vmulw.w vf21, vf0, vf0w; vsub.xyzw vf30, vf30, vf30; vadd.xyzw vf5, vf1, vf30; vadd.xyzw vf6, vf2, vf30; vadd.xyzw vf7, vf3, vf30; vadd.xyzw vf8, vf4, vf30; vaddy.x vf6, vf0, vf1y; vaddz.x vf7, vf0, vf1z; vaddz.y vf7, vf0, vf2z; vaddx.y vf5, vf0, vf2x; vaddx.z vf5, vf0, vf3x; vaddy.z vf6, vf0, vf3y; lqc2 vf20, 0x0(%2); vmulax.xyz ACC, vf5, vf20x; vmadday.xyz ACC, vf6, vf20y; vmaddz.xyz vf21, vf7, vf20z; sqc2 vf21, 0x0(%0)"
        : : "r"(out), "r"(m), "r"(&v) : "memory");
}

// An inlined strrchr(name, '.').
static inline char *FindExtension(char *s)
{
    char *start = s;
    while (*s++)
        ;
    do {
        s--;
    } while (s != start && *s != '.');
    if (*s == '.')
        return s;
    return 0;
}

// Replaces name's extension, or appends one.
static inline void SetExtension(char *name, char *extension)
{
    char *ext;
    if ((ext = FindExtension(name)))
        strcpy(ext, extension);
    else
        strcat(name, extension);
}

void CarProceduralSomethingHitMeCallback(CollisionObject *self, CollisionObject *other)
{
}

CarProcedural::CarProcedural(int enabled) : PhysicsBody(enabled)
{
    m_x358 = 1;
    m_x34C = 0;
    m_loaded = 0;
    m_x340 = 0;
    m_x344 = 0;
    m_x348 = 1;
    m_x350 = 0;
    m_numTires = 0;
    m_segment = 0;
    m_x364 = 0;
    m_x468 = 120.0f;
    m_x470 = 0;
    m_x474 = 0;
    m_x478 = 0;
    m_x498 = 0.5f;
    m_x49C = 0.2f;
    m_wheelAngle = 0.0f;
    m_steerAngle = 0.0f;
    m_wheelBase = 0.0f;
    m_x490 = 0;
    m_x494 = 0;
    m_x4A0 = -3.4028235e38f;            // -FLT_MAX
    m_x2C0 = ZeroVector;
    m_x2D0 = ZeroVector;
    m_x2E0 = ZeroVector;
    m_x2F0 = ZeroVector;
    m_x300 = ZeroVector;
    m_up = YVector;
    m_terrain = 0;
    m_model = 0;
    m_tires = 0;
    m_testCollision = 0;
    m_collision = 0;
    m_rays = 0;
    m_spline = 0;
    m_x354 = 0;
    SetMass(10000.0f);
}

CarProcedural::~CarProcedural()
{
    if (m_tires)
        delete[] m_tires;
    if (m_spline) {
        m_spline->m_refCount--;
        if (m_spline->m_refCount == 0) {
            LpDataAccess->DetachObject(m_spline);
            delete[] (char *)m_spline->m_knots;
            delete m_spline;
        }
    }
}

// Loads the car's model (and its tires), its collision objects and its path
// (.vue). Returns 0 if the path won't load.
int CarProcedural::Create(char *name, char *collisionFile, Terrain *terrain, char *vueFile, _D3DVECTOR *offset,
                          float period, int f, int g, float h, float i, int numTires, float k, float l, float m,
                          int n, int rays)
{
    _D3DVECTOR min;
    _D3DVECTOR max;
    char collisionPath[0x110];
    char vuePath[0x110];
    SoultreeObject *body;

    m_x358 = n;
    GameObject::Create();
    sprintf(collisionPath, "%s%s", CollisionContentDirectory, collisionFile);
    if (!LpDataAccess->FileExists(collisionPath)) {
        PSXprintf("Animated object (%s) has no .col file!!.\n", name);
        *collisionFile = 0;
    }
    sprintf(vuePath, "%s%s", AnimContentDirectory, vueFile);
#line 145 "CarProcedural.cpp"
    STRCPY(m_carName, name);
    PSXRenderableObject *object = new PSXRenderableObject(1);
    object->Create(name, 0, 1);
    AddChild(object, 0);
    m_model = object;
    if (!(body = m_model->GetNamedObject("Body")))
        body = m_model;
    m_object = m_model;
    m_x2F0 = *offset;
    m_x490 = h;
    m_x300 = ZeroVector;
    m_wheelAngle = 0.0f;
    m_x340 = f;
    m_x348 = g;
    body->GetPosition(&m_centerOfMass);
    body->GetBoundingBox(&min, &max);
    _D3DMATRIX inertia = IdentityMatrix();
    inertia._11 = m_mass / 12.0f * (max.y * max.y + max.z * max.z);
    inertia._22 = m_mass / 12.0f * (max.x * max.x + max.z * max.z);
    inertia._33 = m_mass / 12.0f * (max.x * max.x + max.y * max.y);
    SetInertia(inertia);
    m_numTires = numTires;
    if (m_numTires) {
        m_tires = new SoultreeObject *[m_numTires];
        if (m_numTires > 0)
            m_tires[0] = m_model->GetNamedObject("TireFrontL");
        if (m_numTires > 1)
            m_tires[1] = m_model->GetNamedObject("TireFrontR");
        if (m_numTires > 2)
            m_tires[2] = m_model->GetNamedObject("TireBackL");
        if (m_numTires > 3)
            m_tires[3] = m_model->GetNamedObject("TireBackR");
        if (m_numTires > 4)
            m_tires[4] = m_model->GetNamedObject("TireMidL");
        if (m_numTires > 5)
            m_tires[5] = m_model->GetNamedObject("TireMidR");
        m_tires[0]->GetBoundingBox(&min, &max);
        if (max.y > 0.0f)
            m_x47C = 1.0f / max.y;
        else
            m_x47C = 1.0f;
    }
    m_terrain = terrain;
    m_testCollision = new CollisionObject(1);
    m_testCollision->Create(3);
    m_testCollision->m_ignoreVegetation = 1;
    AddChild(m_testCollision, -1);
    m_x480 = 5.0f;
    m_collision = new CollisionObject(1);
    m_collision->Create(5);
    m_collision->m_owner = this;
    m_collision->m_ownerData = this;
    m_collision->m_ownerType = WORLDOBJECT_PHYSICSBODY;
    m_collision->m_ignoreVegetation = 1;
    AddChild(m_collision, -1);
    if (collisionFile && *collisionFile) {
        if (rays) {
            m_rays = new CollisionObject(1);
            m_rays->Create(3);
            m_rays->m_ownerType = WORLDOBJECT_PHYSICSBODY;
            m_rays->SetTestListType((CollisionObjectTestListType)1);
            m_rays->SetSoultreeCollisionRays(m_object, collisionFile, 0);
            m_rays->m_otherCallback = CarProceduralSomethingHitMeCallback;
            m_rays->m_owner = m_object;
            m_rays->m_ownerData = this;
            AddChild(m_rays, 2);
        }
        m_collision->SetSoultreeCollision(m_model, collisionFile);
#line 261 "CarProcedural.cpp"
        ASSERTF(m_collision->m_collisionType != 1, ("CAR PROCEDURAL USING HIERARCHY COLLISION OBJECT FROM FILE!!!   - %s", collisionFile));
    } else {
#line 267 "CarProcedural.cpp"
        ASSERTF(0, ("No Collision File for %s!", name));
    }
    *(int *)m_collision->m_collisionData = 1;
    m_testCollision->AddToTestList(m_collision);
    m_x468 = period;
    if (period > 0.0f)
        m_timeScale = 1.0f / m_x468;
    m_x470 = k;
    m_x474 = l;
    m_x478 = m;
    if (!LoadVueFile(vuePath, m_model->m_name)) {
        Release();
        return 0;
    }
    return (int)this;
}


// Loads the car's path: the binary .cpb if there is one, else the frames of
// the named object from the .vue animation export.
int CarProcedural::LoadVueFile(char *fileName, char *objectName)
{
    char line[0x400];
    char quoted[0x100];
    char keyword[0x400];
    char command[0x100];
    char name[0x100];
    float matrix[9];
    long handle;
    void *data;
    unsigned int frame;
    int index;

    sprintf(quoted, "\"%s\"", objectName);
    SetExtension(fileName, ".cpb");
    handle = -1;
    data = 0;
    if (LpDataAccess->GetDataOrFileHandle(fileName, "rb", &handle, (void **)&data, 1)) {  // (cast: retail argument order)
        if (data) {
            m_spline = (Spline *)data;
            m_spline->m_refCount++;
            goto success;
        }
        LoadCarProceduralBinary(handle);
        LpDataAccess->AttachObject(handle, m_spline);
        LpDataAccess->FClose(handle);
        m_loaded = 1;
        goto success;
    }
    SetExtension(fileName, ".vue");
    handle = -1;
    data = 0;
    if (LpDataAccess->GetDataOrFileHandle(fileName, "r", &handle, (void **)&data, 1)) {  // (cast: retail argument order)
        if (data) {
            m_spline = (Spline *)data;
            m_spline->m_refCount++;
            goto success;
        }
        m_spline = new Spline;
        m_spline->m_count = 0;
        m_spline->m_height = 0;
        m_spline->m_time = 0;
        while (!LpDataAccess->FEof(handle)) {
            LpDataAccess->FGets(handle, line, 0x400);
            if (!strncmp(line, "FRAME", strlen("FRAME")))
                m_spline->m_count++;
        }
        LpDataAccess->FSeek(handle, 0, 0);
        m_spline->m_knots = new SplineKnot[m_spline->m_count];
        index = -1;
        while (!LpDataAccess->FEof(handle)) {
            LpDataAccess->FGets(handle, line, 0x400);
            if (!strncmp(line, "FRAME", strlen("FRAME"))) {
                index++;
                sscanf(line, "%s %d", keyword, &frame);
                m_spline->m_knots[index].m_time = frame;
            } else if (!strncmp(line, "transform", strlen("transform"))) {
                _D3DVECTOR position;
                sscanf(line, "%s %s %f %f %f %f %f %f %f %f %f %f %f %f", command, name, &matrix[0], &matrix[1],
                       &matrix[2], &matrix[3], &matrix[4], &matrix[5], &matrix[6], &matrix[7], &matrix[8],
                       &position.x, &position.z, &position.y);
                if (strstr(name, quoted) || strstr(name, "Body"))
                    m_spline->m_knots[index].m_position = position;
            }
        }
        ComputeSplineLengths();
        m_spline->m_refCount = 1;
        LpDataAccess->AttachObject(handle, m_spline);
        LpDataAccess->FClose(handle);
        m_loaded = 1;
        goto success;
    }
    PSXprintf("Could not find procedural cpb or vue file %s.\n", fileName);
#line 442 "CarProcedural.cpp"
    ASSERT(0, "");
    return 0;
success:
    return 1;
}


// Approximates each spline segment's arc length with 10 chords.
void CarProcedural::ComputeSplineLengths()
{
    float step;
    unsigned int i;
    unsigned int j;
    unsigned int steps;
    steps = 10;
    step = 1.0f / steps;

    for (i = 0; i < m_spline->m_count - 1; i++) {
        SplineKnot *k = &m_spline->m_knots[i];
        float t1;
        float t0;
        float scale;
        float start;
        float span;
        scale = m_timeScale;
        t0 = k[0].m_time;
        t1 = k[1].m_time;
        start = scale * t0;
        k->m_length = 0;
        span = scale * t1 - start;
        _D3DVECTOR prev = InterpolateSpline(start);
        _D3DVECTOR cur;
        for (j = 1; j <= steps; j++) {
            cur = InterpolateSpline(start + span * (j * step));
            m_spline->m_knots[i].m_length += Length(cur - prev);
            prev = cur;
        }
    }
    m_spline->m_knots[m_spline->m_count - 1].m_length = 0;
}


void CarProcedural::AddSearchPlacement(SearchNode *a, SearchNode *b)
{
    m_model->AddSearchPlacement(a, 2);
    m_collision->AddSearchPlacement(b, 2);
}

void CarProcedural::ResetTime(float t, int a)
{
    if (m_spline)
        m_spline->m_time = t;
    m_x354 = a;
}

// Moves the car along its spline: position on the ground (terrain or a
// ray), heading along its motion, body tilted to the ground normal, and
// the tires rolled and the front ones steered.
int CarProcedural::Tick(float dt)
{
    static int carProcMtr = -1;
    int result = GameObject::Tick(dt);
    _D3DVECTOR front;
    _D3DVECTOR up;
    if (dt <= 0.0f)
        return result;
    // A switch, not an if: the empty case gives retail's block-optimizer
    // event.
    switch (m_loaded) {
    case 0:
        break;
    default:
        m_spline->m_rate = 1.0f / dt;
        m_spline->m_time = fmod(m_spline->m_time + dt, m_timeScale * m_spline->m_knots[m_spline->m_count - 1].m_time);
        break;
    }
    if (!m_x34C) {
        _D3DVECTOR rays[2];
        rays[0] = ZeroVector;
        rays[1] = ZeroVector;
        rays[0].y += m_x480;
        rays[1].y -= m_x480;
        m_testCollision->SetRayCollision(1, rays, 0);
        _D3DVECTOR tire;
        for (unsigned int i = 0; i < m_numTires; i++) {
            m_tires[i]->GetPosition(&tire);
            if (tire.z > 0.0f)
                m_wheelBase = tire.z;
        }
        m_x34C = 1;
    }
    static int meter3 = -1;
    float t;
    if (m_x490 > 0.0f) {
        t = ComputeLagTime(m_spline->m_time, m_x490);
    } else {
        m_x494 = 0;
        t = m_spline->m_time;
    }
    m_x2E0 = InterpolateSpline(t);
    _D3DVECTOR motion = m_x2E0 - m_x2D0;
    m_x2D0 = m_x2E0;
    motion.y = 0.0f;
    m_velocity = m_spline->m_rate * motion;
    if (m_numTires) {
        _D3DVECTOR local;
        RotateWorldToLocal(&local, &m_model->m_worldMatrix, motion);
        m_wheelAngle += dt * (local.z * m_x47C);
        m_wheelAngle = fmod(m_wheelAngle, 6.2831855f);
    }
    _D3DVECTOR heading = motion;
    float len2 = heading.x * heading.x + heading.y * heading.y + heading.z * heading.z;
    if (len2 == 0.0f)
        heading = ZeroVector;
    else
        heading *= RSqrt(len2);
    CrossProduct(&m_xA0, &m_x300, &heading);
    m_xA0 *= m_spline->m_rate;
    m_xB0 = m_model->RotateVectorWorldToLocal(&m_xA0);
    if (m_x354)
        m_x300 = heading;
    else
        m_x300 = m_x498 * m_x300 + (1.0f - m_x498) * heading;
    if (m_x348 && m_numTires) {
        _D3DVECTOR ahead = InterpolateSpline(ComputeLagTime(t, -m_wheelBase)) - m_x2E0;
        _D3DVECTOR localAhead;
        RotateWorldToLocal(&localAhead, &m_model->m_worldMatrix, ahead);
        m_steerAngle = FastAtan2(localAhead.x, localAhead.z);
        m_steerAngle = Fmin(Fmax(m_steerAngle, -0.52359873f), 0.52359873f);
    }
    static int meter2 = -1;
    _D3DVECTOR position;
    m_model->GetPosition(0, &position);
    _D3DVECTOR ground = position;
    _D3DVECTOR normal;
    if (!m_x344 && (!m_x340 || m_loaded)) {
        if (m_terrain)
            m_terrain->GetGridPointIntersection(&ground, &normal, 0, 0);
        else
            ground.y = m_x2E0.y;
        if (m_x358) {
            _D3DMATRIX matrix;
            Vu0OrientMatrix(&matrix, &position, (_D3DVECTOR &)ZVector, (_D3DVECTOR &)YVector, 1, 1);
            m_testCollision->UpdateTransform(&matrix);
            m_testCollision->CheckForCollision(0);
            CollisionObject *test = m_testCollision;
            if (test->m_collided) {
                VertexRayToFaceIntersectionData *data = test->m_intersectionData;
                _D3DVECTOR hit = position + (2.0f * (1.0f - data->m_x00.y) * m_x480 - m_x480) * YVector;
                if (hit.y > ground.y) {
                    ground = hit;
                    normal = data->m_normal;
                }
            }
        }
        if (m_loaded) {
            m_spline->m_height = ground.y;
            if (m_spline->m_height == m_x4A0) {
                if (m_x340)
                    m_x344 = 1;
            } else {
                m_x4A0 = m_spline->m_height;
            }
        }
    } else {
        ground.y = m_spline->m_height;
        normal = YVector;
    }
    static int meter6 = -1;
    if (m_x340) {
        m_model->SetOrientation(&m_x300, &normal, 0, 1, 0);
    } else {
        if (m_x354)
            m_up = normal;
        else if (DotProduct(&m_up, &normal) > 0.707f)
            m_up = m_x49C * m_up + (1.0f - m_x49C) * normal;
        _D3DVECTOR along = DotProduct(&m_x300, &m_up) * m_up;
        front = m_x300 - along;
        m_model->SetOrientation(&front, &m_up, 0, 1, 0);
    }
    _D3DVECTOR place;
    place.x = m_x2E0.x + m_x2F0.x;
    place.z = m_x2E0.z + m_x2F0.z;
    place.y = ground.y;
    _D3DVECTOR offset;
    if (m_centerOfMass == ZeroVector) {
        m_model->SetPosition(0, &place);
    } else {
        ApplyMatrixVector(&m_x60, &m_model->m_worldMatrix, m_centerOfMass);
        offset = place - m_x60;
        m_model->SetPosition(0, &offset);
    }
    static int meter7 = -1;
    for (unsigned int i = 0; i < m_numTires; i++) {
        if (m_x348 && i < 2) {
            GetOrientationVectors(&front, &up, m_steerAngle, -m_wheelAngle, 0.0f);
        } else {
            front.x = 0.0f;
            static int meter9 = -1;
            FastSinCos(-m_wheelAngle, &front.y, &front.z);
            up.x = 0.0f;
            up.y = front.z;
            up.z = -front.y;
        }
        m_tires[i]->SetOrientation(&front, &up, 0, 1, 0);
    }
    static int meter4 = -1;
    m_collision->UpdateCollisionInfo();
    if (m_rays) {
        m_rays->UpdateCollisionInfo();
        m_rays->CheckForCollision(0);
    }
    static int meter5 = -1;
    m_model->UpdateSearchPlacement();
    return result;
}


// Notes: `d *= length` as its own statement, the else branch's knot reads through
// m_segment with `fwd = m_segment` after them, and a block-scoped `r` before the
// m_timeScale multiply give retail's mul.s operand orders and delay slots.
// Spline time `lag` units of arc length behind (lag > 0) or ahead of the
// position at time t.
float CarProcedural::ComputeLagTime(float t, float lag)
{
    float pos;
    unsigned int next;
    float result;
    int back;
    unsigned int fwd;
    float d;
    float frac;
    float start;
    float end;
    float span;
    pos = t * m_x468;
    next = m_segment + 1;
    while (next < m_spline->m_count && !(pos <= m_spline->m_knots[next].m_time)) {
        m_segment++;
        next++;
    }
    while (m_segment != 0 && pos < m_spline->m_knots[m_segment].m_time) {
        m_segment--;
        next--;
    }
    if (!(lag <= 0.0f)) {
        d = (pos - m_spline->m_knots[m_segment].m_time) / (m_spline->m_knots[next].m_time - m_spline->m_knots[m_segment].m_time);
        d *= m_spline->m_knots[m_segment].m_length;
        back = m_segment;
        while (d < lag) {
            back--;
            if (back < 0)
                back += m_spline->m_count;
            d += m_spline->m_knots[back].m_length;
        }
        d -= lag;
        result = 0.0f;
        if (m_spline->m_knots[back].m_length != result) {
            frac = d / m_spline->m_knots[back].m_length;
            {
                float r = m_spline->m_knots[back].m_time +
                          frac * (m_spline->m_knots[(back + 1) % m_spline->m_count].m_time - m_spline->m_knots[back].m_time);
                result = r * m_timeScale;
            }
        } else {
            PSXprintf("CarProcedural::THIS SHOULD NEVER HAPPEN.\n");
            return result;
        }
    } else {
        start = m_spline->m_knots[m_segment].m_time;
        span = m_spline->m_knots[next].m_time - start;
        d = (m_spline->m_knots[next].m_time - pos) / span * m_spline->m_knots[m_segment].m_length;
        fwd = m_segment;
        while (d < Fabs(lag)) {
            fwd = (fwd + 1) % m_spline->m_count;
            d += m_spline->m_knots[fwd].m_length;
        }
        d += lag;
        result = 0.0f;
        if (m_spline->m_knots[fwd].m_length != result) {
            frac = d / m_spline->m_knots[fwd].m_length;
            end = m_spline->m_knots[(fwd + 1) % m_spline->m_count].m_time;
            // Retail adds the remainder to the far knot's time.
            {
                float r = end + (1.0f - frac) * (end - m_spline->m_knots[fwd].m_time);
                result = r * m_timeScale;
            }
        } else {
            PSXprintf("CarProcedural::THIS SHOULD NEVER HAPPEN.\n");
        }
    }
    return result;
}


// Hermite (Catmull-Rom style) position on the spline at time t, which wraps
// around the spline's length.
_D3DVECTOR CarProcedural::InterpolateSpline(float t)
{
    SplineKnot *knots;
    SplineKnot *k1;
    SplineKnot *k0;
    _D3DVECTOR *p0;
    Spline *spline;     // declaration order gives retail's register choice
    _D3DVECTOR *p1;
    float h00, h01, h10, h11;
    t = fmod(t, m_timeScale * m_spline->m_knots[m_spline->m_count - 1].m_time);
    if (t < 0.0f)
        t += m_timeScale * m_spline->m_knots[m_spline->m_count - 1].m_time;
    if (m_spline->m_count) {
        float pos = t * m_x468;
        unsigned int next = m_segment + 1;
        while (next < m_spline->m_count && !(pos <= m_spline->m_knots[next].m_time)) {
            m_segment++;
            next++;
        }
        while (m_segment != 0 && pos < m_spline->m_knots[m_segment].m_time) {
            m_segment--;
            next--;
        }
        CalculateHermiteCoefficients((pos - m_spline->m_knots[m_segment].m_time) /
                                     (m_spline->m_knots[next].m_time - m_spline->m_knots[m_segment].m_time),
                                     h00, h01, h10, h11);

        spline = m_spline;
        knots = spline->m_knots;
        k1 = &knots[next];
        k0 = &knots[m_segment];
        _D3DVECTOR d = k1->m_position - k0->m_position;
        p0 = &k0->m_position;
        p1 = &k1->m_position;
        _D3DVECTOR m0;
        _D3DVECTOR m1;
        if (m_segment == 0) {
            m0 = d;
        } else {
            m0 = (k1->m_time - k0->m_time) / (k1->m_time - knots[m_segment - 1].m_time) *
                 (*p0 - knots[m_segment - 1].m_position + d);
        }
        if (next == spline->m_count - 1) {
            m1 = d;
        } else {
            float s = (k1->m_time - k0->m_time) / (knots[next + 1].m_time - k0->m_time);
            SplineKnot *k2 = &knots[next + 1];
            m1 = s * (d + (k2->m_position - *p1));
        }
        return h00 * *p0 + h01 * *p1 + h10 * m0 + h11 * m1;
    }
    return ZeroVector;
}


void CarProcedural::CalculateHermiteCoefficients(float t, float &h00, float &h01, float &h10, float &h11)
{
    float t2 = t * t;
    float t3 = t * t2;
    float h = 3.0f * t2 - t3 - t3;   // 3t^2 - 2t^3, spelled so it subtracts twice like retail
    h00 = 1.0f - h;
    h01 = h;
    float g = t3 - t2;
    h10 = t + (g - t2);
    h11 = g;
}

void CarProcedural::LoadCarProceduralBinary(long file)
{
    m_spline = new Spline;
    LpDataAccess->FRead(file, m_spline, 4, 1);
    m_spline->m_knots = (SplineKnot *)new CarProceduralPoint[m_spline->m_count];
    LpDataAccess->FRead(file, m_spline->m_knots, sizeof(CarProceduralPoint), m_spline->m_count);
    m_spline->m_refCount = 1;
    m_spline->m_height = 0;
    m_spline->m_time = 0;
}

// Not used by a procedural car (its motion comes from the spline).
void CarProcedural::ResetDerivatives() {}
void CarProcedural::AddWorldForce(_D3DVECTOR, _D3DVECTOR) {}
void CarProcedural::AddWorldForce(_D3DVECTOR) {}
void CarProcedural::AddWorldMoment(_D3DVECTOR) {}
void CarProcedural::AddBodyForce(_D3DVECTOR, _D3DVECTOR) {}
void CarProcedural::AddBodyForce(_D3DVECTOR) {}
void CarProcedural::AddBodyMoment(_D3DVECTOR) {}

void PhysicsBody::SetElectricCharge(float charge)
{
    m_electricCharge = charge;
}


// ---- src/PhysicsBody/SetMagneticDipole.cpp


void PhysicsBody::SetMagneticDipole(_D3DVECTOR dipole)
{
    m_magneticDipole = dipole;
}


// ---- src/PhysicsBody/SetInitialVelocity.cpp


void PhysicsBody::SetInitialVelocity(_D3DVECTOR &v)
{
    m_initialVelocity.x = v.x;
    m_initialVelocity.y = v.y;
    m_initialVelocity.z = v.z;
    m_initialVelocity.w = v.w;
}


// ---- src/PhysicsBody/SetInitialBodyAngularRate.cpp


void PhysicsBody::SetInitialBodyAngularRate(_D3DVECTOR &v)
{
    m_initialBodyAngularRate.x = v.x;
    m_initialBodyAngularRate.y = v.y;
    m_initialBodyAngularRate.z = v.z;
    m_initialBodyAngularRate.w = v.w;
}

