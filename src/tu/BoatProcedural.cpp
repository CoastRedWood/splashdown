// BoatProcedural.cpp  @ 0x003CAE10 (text 0x3CAE10-0x3CBB50)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's BoatProcedural.cpp as one translation unit: BoatProcedural's
// functions, then Ken Perlin's noise (noise1, normalize2/3, init: its
// tables follow BoatProcedural's .bss and `start` follows gBoatMaxRollAngle
// in .data, so it is this TU), then the static initializer. `make link`
// places the object's data as well as its code (tools/link_c.py). The
// JetskiScreenText(Params) functions after init in its tu_map row are
// another TU (built with -sdatathreshold 2), and the two collision
// callbacks after them are left per-function.


#include "BoatProcedural/BoatProcedural.h"
#include "CarProcedural/CarProcedural.h"
#include "Spline/Spline.h"
#include "d3dtypes.h"
#include "GameObject/GameObject.h"
#include "SoultreeObject/SoultreeObject.h"
#include "CollisionObject/CollisionObject.h"
#include "Water/Water.h"
#include "PhysicsBody/PhysicsBody.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"

// The old Tick.cpp had inline_depth(4), and noise1.cpp
// fast_fptosi.
#pragma inline_depth(4)
#pragma fast_fptosi on

extern "C" double fmod(double x, double y);
extern "C" double sqrt(double);
extern "C" int rand();

float gBoatMaxTurnRate = 1.0f;
float gBoatMaxRollAngle = 0.1745329052f;    // 10 degrees (0x3E32B8C1)

void init();

// ---- src/BoatProcedural/ctor.cpp

BoatProcedural::BoatProcedural(int enabled) : CarProcedural(enabled)
{
    m_water = 0;
}


// ---- src/BoatProcedural/dtor.cpp

BoatProcedural::~BoatProcedural()
{
}


// ---- src/BoatProcedural/ResetTime.cpp

void BoatProcedural::ResetTime(float t, int a)
{
    if (m_spline)
        m_spline->m_time = t;
    m_x354 = a;
}


// ---- src/BoatProcedural/Tick.cpp
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

// out = m's 3x3 part * v (w ends up 1), v passed by value.
static inline void ApplyMatrixVector(_D3DVECTOR *out, _D3DMATRIX *m, _D3DVECTOR v)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x10(%1); lqc2 vf6, 0x20(%1); lqc2 vf8, 0x0(%2); vmulax.xyz ACC, vf4, vf8x; vmadday.xyz ACC, vf5, vf8y; vmaddz.xyz vf9, vf6, vf8z; vadd.xyzw vf9, vf9, vf0; sqc2 vf9, 0x0(%0)"
        : : "r"(out), "r"(m), "r"(&v) : "memory");
}

// Clamps v in place to the range between a and b (either order).
static inline void ClampRef(float &v, float a, float b)
{
    if (b > a) {
        if (v > b)
            v = b;
        else if (v < a)
            v = a;
    } else {
        if (v < b)
            v = b;
        else if (v > a)
            v = a;
    }
}

// Maps x from [x0, x1] onto [y0, y1], clamped to the output range.
static inline float MapRange(float x, float x0, float y0, float x1, float y1)
{
    float y;
    if (x1 != x0) {
        float v = y0 + (x - x0) * (y1 - y0) / (x1 - x0);
        ClampRef(v, y0, y1);
        y = v;
    } else
        y = 0.0f;
    return y;
}


// Moves the boat along its spline (looping), heading along its motion and
// sitting on the water, and pushes the water where it is.
int BoatProcedural::Tick(float dt)
{
    int result = GameObject::Tick(dt);
    static int BoatProceduralID = -1;
    _D3DVECTOR unused1;                 // constructed, never used (retail)
    _D3DVECTOR unused2;
    if (dt <= 0.0f)
        return result;
    if (m_loaded) {
        m_spline->m_rate = 1.0f / dt;
        m_spline->m_time = fmod(m_spline->m_time + dt, m_timeScale * m_spline->m_knots[m_spline->m_count - 1].m_time);
    }
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
    m_water->ApplyExternalForce(1.0f, m_x2E0);
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
    _D3DVECTOR position;
    m_model->GetPosition(0, &position);
    _D3DVECTOR surface = position;
    _D3DVECTOR up = YVector;
    if (!m_x344 && m_loaded && m_water) {
        m_water->GetGridPointIntersection(&surface, 0);
        m_spline->m_height = surface.y;
        m_x344 = 1;
    } else {
        surface.y = m_spline->m_height;
    }
    MapRange(m_xA0.y, -gBoatMaxTurnRate, -gBoatMaxRollAngle, gBoatMaxTurnRate, gBoatMaxRollAngle);  // roll: computed, unused
    m_model->SetOrientation(&m_x300, &up, 0, 1, 0);
    _D3DVECTOR place;
    place.x = m_x2E0.x + m_x2F0.x;
    place.z = m_x2E0.z + m_x2F0.z;
    place.y = surface.y;
    if (m_centerOfMass == ZeroVector) {
        m_model->SetPosition(0, &place);
    } else {
        _D3DVECTOR offset;
        ApplyMatrixVector(&m_x60, &m_model->m_worldMatrix, m_centerOfMass);
        offset = place - m_x60;
        m_model->SetPosition(0, &offset);
    }
    m_collision->UpdateCollisionInfo();
    m_model->UpdateSearchPlacement();
    return result;
}


// ---- src/global/noise1.cpp
// Ken Perlin's reference noise (noise.c), compiled as C++ with floats: the
// tables and `start` are this TU's (.bss/.data in this order, after
// BoatProcedural's). Not static in retail? Unknown; they are file globals
// here, as the old per-function files' externs named them.
#define B 0x100
#define BM 0xff
#define N 0x1000

int p[B + B + 2];
float g3[B + B + 2][3];
float g2[B + B + 2][2];
float g1[B + B + 2];
int start = 1;

#define s_curve(t) ( t * t * (3. - 2. * t) )
#define lerp(t, a, b) ( a + t * (b - a) )
#define setup(i,b0,b1,r0,r1)\
	t = vec[i] + N;\
	b0 = ((int)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (int)t;\
	r1 = r0 - 1.;

// mwcc session-state shim: the
// compiler run had seen a two-argument runtime helper before noise1. The
// whole TU still needs it (nothing before noise1 here makes such a call).
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }

float noise1(float arg)
{
	int bx0, bx1;
	float rx0, rx1, sx, t, u, v, vec[1];

	vec[0] = arg;
	if (start) {
		start = 0;
		init();
	}

	setup(0, bx0,bx1, rx0,rx1);

	sx = s_curve(rx0);

	u = rx0 * g1[ p[ bx0 ] ];
	v = rx1 * g1[ p[ bx1 ] ];

	return lerp(sx, u, v);
}


// ---- src/global/normalize2.cpp
void normalize2(float v[2])
{
    float s;
    s = sqrt(v[0] * v[0] + v[1] * v[1]);
    v[0] = v[0] / s;
    v[1] = v[1] / s;
}


// ---- src/global/normalize3.cpp
void normalize3(float v[3])
{
    float s;
    s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    v[0] = v[0] / s;
    v[1] = v[1] / s;
    v[2] = v[2] / s;
}


// ---- src/global/init.cpp
void init()
{
	int i, j, k;

	for (i = 0 ; i < B ; i++) {
		p[i] = i;

		g1[i] = (float)((rand() % (B + B)) - B) / B;

		for (j = 0 ; j < 2 ; j++)
			g2[i][j] = (float)((rand() % (B + B)) - B) / B;
		normalize2(g2[i]);

		for (j = 0 ; j < 3 ; j++)
			g3[i][j] = (float)((rand() % (B + B)) - B) / B;
		normalize3(g3[i]);
	}

	while (--i) {
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}

	for (i = 0 ; i < B + 2 ; i++) {
		p[B + i] = p[i];
		g1[B + i] = g1[i];
		for (j = 0 ; j < 2 ; j++)
			g2[B + i][j] = g2[i][j];
		for (j = 0 ; j < 3 ; j++)
			g3[B + i][j] = g3[i][j];
	}
}


// ---- src/sinit/BoatProcedural.cpp
// Static initializer of BoatProcedural.cpp: only the axis vectors from the shared header.
