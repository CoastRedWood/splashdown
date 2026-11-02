// Portal.cpp  @ 0x002FEA10 (text 0x2FEA10-0x300010)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's Portal.cpp as one translation unit: Portal's methods, then the
// static initializer (the axis vectors and Portal::WholeScreen's corners). Its
// .rodata ends with the Portal RTTI (0x4302A8). The rest of the tu_map row:
// SpatialSearch.cpp (0x300010-0x300670, src/tu, no initializer; labels restart
// at @482), then SimpleFollowCam.cpp, JetskiFollowCam.cpp and the others
// (per-function).



#include "GameObject/GameObject.h"
#include "d3dtypes.h"
#include "ScreenPoint2D/ScreenPoint2D.h"
#include "ScreenQuad2D/ScreenQuad2D.h"
#include "Portal/Portal.h"
#include "Camera/Camera.h"
#include "RenderTarget/RenderTarget.h"
#include "sinit/AxisVectors.h"

// inline_depth is TU-wide (RayIntersects').
#pragma inline_depth(5)

ScreenQuad2D Portal::WholeScreen;

void TransformVertices(_D3DVECTOR *dst, _D3DVECTOR *src, _D3DMATRIX *matrix, int count, int dstStride, int srcStride);


// xyz dot product on VU0.
static inline float DotProduct(_D3DVECTOR *a, _D3DVECTOR *b)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2.ni $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(a), "r"(b) : "$2");
    return r;
}

// The same with the previous DotProduct's first vector still in vf4.
static inline float DotProductNext(_D3DVECTOR *b)
{
    float r;
    asm("lqc2 vf5, 0x0(%1); vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2.ni $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(b) : "$2");
    return r;
}

// libvu0 sceVu0Normalize.
static inline void Normalize(_D3DVECTOR *out, _D3DVECTOR *in)
{
    asm("lqc2 vf4, 0x0(%1); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(in) : "memory");
}

// libvu0 sceVu0OuterProduct: out = a x b.
static inline void CrossProduct(_D3DVECTOR *out, _D3DVECTOR *a, _D3DVECTOR *b)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x0(%2); vopmula.xyz ACC, vf4, vf5; vopmsub.xyz vf6, vf5, vf4; vsub.w vf6, vf6, vf6; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(a), "r"(b) : "memory");
}

// ---- src/Portal/ctor.cpp

  // __construct_array steps 16 bytes per element


Portal::Portal() : GameObject(1)
{
}


// ---- src/Portal/dtor.cpp


Portal::~Portal()
{
}


// ---- src/Portal/Create.cpp


Portal *Portal::Create(_D3DVECTOR *corners)
{
    for (int i = 0; i < 4; i++)
        m_corners[i] = corners[i];
    ScreenPoint2D screen[4];
    screen[0] = ScreenPoint2D(-1.0f, -1.0f);
    screen[1] = ScreenPoint2D(-1.0f, 1.0f);
    screen[2] = ScreenPoint2D(1.0f, 1.0f);
    screen[3] = ScreenPoint2D(1.0f, -1.0f);
    WholeScreen.Set(screen);
    WholeScreen.Orient();
    SetDiagonal();
    SetNormals();
    return this;
}


// ---- src/Portal/SetDiagonal.cpp

// |v|^2 on VU0 (xyz).
static inline float LengthSquared(_D3DVECTOR *v)
{
    float r;
    asm("lqc2 vf4, 0x0(%1); vmove.xyzw vf5, vf4; vmul.xyz vf5, vf4, vf5; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; qmfc2.ni $2, vf5; mtc1 $2, %0"
        : "=f"(r) : "r"(v) : "$2");
    return r;
}

static inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}



// The longer of the two diagonals across the portal's corners.
void Portal::SetDiagonal()
{
    _D3DVECTOR d1;
    _D3DVECTOR d2;
    d1 = _D3DVECTOR(m_corners[2].x - m_corners[0].x, m_corners[2].y - m_corners[0].y, m_corners[2].z - m_corners[0].z);
    d2 = _D3DVECTOR(m_corners[3].x - m_corners[1].x, m_corners[3].y - m_corners[1].y, m_corners[3].z - m_corners[1].z);
    m_diagonal = Max(Sqrt(LengthSquared(&d1)), Sqrt(LengthSquared(&d2)));
}


// ---- src/Portal/SetNormals.cpp


// The portal's plane normal, then each edge's normal within the plane.
void Portal::SetNormals()
{
    _D3DVECTOR edge = _D3DVECTOR(m_corners[1].x - m_corners[0].x, m_corners[1].y - m_corners[0].y, m_corners[1].z - m_corners[0].z);
    _D3DVECTOR diagonal = _D3DVECTOR(m_corners[2].x - m_corners[0].x, m_corners[2].y - m_corners[0].y, m_corners[2].z - m_corners[0].z);
    CrossProduct(&m_normal, &edge, &diagonal);
    Normalize(&m_normal, &m_normal);
    for (int i = 0; i < 4; i++) {
        edge = _D3DVECTOR(m_corners[(i + 1) % 4].x - m_corners[i].x, m_corners[(i + 1) % 4].y - m_corners[i].y, m_corners[(i + 1) % 4].z - m_corners[i].z);
        CrossProduct(&m_edgeNormals[i], &edge, &m_normal);
        Normalize(&m_edgeNormals[i], &m_edgeNormals[i]);
    }
}


// ---- src/Portal/ProjectToScreen.cpp


void TransformVertices(_D3DVECTOR *dst, _D3DVECTOR *src, _D3DMATRIX *matrix, int count, int dstStride, int srcStride);


int Portal::ProjectToScreen(ScreenQuad2D &quad)
{
    _D3DVECTOR verts[4];
    _D3DVECTOR eye = LpRenderTarget->m_camera->m_position;
    float reach = 0.3f * m_diagonal;
    _D3DVECTOR ahead(eye.x + reach * LpRenderTarget->m_camera->m_lookAt.x,
                     eye.y + reach * LpRenderTarget->m_camera->m_lookAt.y,
                     eye.z + reach * LpRenderTarget->m_camera->m_lookAt.z);
    if (RayIntersects(&eye, &ahead, 0)) {
        quad.Set(WholeScreen);
        quad.Orient();
        return 1;
    }
    TransformVertices(verts, m_corners, &LpRenderTarget->m_camera->m_viewProjection, 4, 16, 16);
    int clipped[4];
    for (int i = 0; i < 4; i++) {
        float inv = 1.0f / verts[i].w;
        verts[i].x = inv * verts[i].x;
        verts[i].y = inv * verts[i].y;
        verts[i].z = inv * verts[i].z;
        if (verts[i].z < -1.0001f) {
            clipped[i] = 1;
            if (verts[i].x < 0.0f)
                verts[i].x = -1.1f;
            else
                verts[i].x = 1.1f;
            if (verts[i].y < 0.0f)
                verts[i].y = -1.1f;
            else
                verts[i].y = 1.1f;
        } else if (!(verts[i].z <= 1.0001f)) {
            clipped[i] = 1;
            if (verts[i].x < 0.0f)
                verts[i].x = 1.1f;
            else
                verts[i].x = -1.1f;
            if (verts[i].y < 0.0f)
                verts[i].y = 1.1f;
            else
                verts[i].y = -1.1f;
        } else {
            clipped[i] = 0;
        }
    }
    if (clipped[0] && clipped[1] && clipped[2] && clipped[3])
        return 0;
    quad.Set(verts);
    quad.Orient();
    return 1;
}


// ---- src/Portal/RayIntersects.cpp

// Is p (on the triangle's plane) inside triangle abc? Tested at b and c.
static inline int InsideTriangle(_D3DVECTOR &p, _D3DVECTOR &a, _D3DVECTOR &b, _D3DVECTOR &c, _D3DVECTOR *normal)
{
    _D3DVECTOR edge1, edge2, toPoint;
    edge1 = c - b;
    edge2 = a - b;
    toPoint = p - b;
    _D3DVECTOR n;
    CrossProduct(&n, &edge1, &toPoint);
    Normalize(&n, &n);
    if (DotProduct(normal, &n) < 0.0f)
        return 0;
    CrossProduct(&n, &toPoint, &edge2);
    Normalize(&n, &n);
    if (DotProduct(normal, &n) < 0.0f)
        return 0;
    edge1 = a - c;
    edge2 = b - c;
    toPoint = p - c;
    CrossProduct(&n, &edge1, &toPoint);
    Normalize(&n, &n);
    if (DotProduct(normal, &n) < 0.0f)
        return 0;
    CrossProduct(&n, &toPoint, &edge2);
    Normalize(&n, &n);
    if (DotProduct(normal, &n) < 0.0f)
        return 0;
    return 1;
}

// Does the segment a-b cross the portal? Optionally returns the crossing.
int Portal::RayIntersects(_D3DVECTOR *a, _D3DVECTOR *b, _D3DVECTOR *hit)
{
    _D3DVECTOR point, toA, toB;
    toA = *a - m_corners[0];
    toB = *b - m_corners[0];
    if (!(DotProduct(&m_normal, &toA) * DotProductNext(&toB) <= 0.0f))
        return 0;
    _D3DVECTOR ray = *b - *a;
    float d = -DotProduct(&m_corners[0], &m_normal);
    float t = -(d + DotProduct(&m_normal, a)) / DotProductNext(&ray);
    if (t >= 0.0f && t <= 1.0f) {
        point = *a + t * ray;
        if (hit)
            *hit = point;
        if (InsideTriangle(point, m_corners[0], m_corners[1], m_corners[2], &m_normal))
            return 1;
        if (InsideTriangle(point, m_corners[0], m_corners[2], m_corners[3], &m_normal))
            return 1;
    }
    return 0;
}


// ---- src/Portal/SphereIntersects.cpp


static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}


// Does the sphere touch the portal: within radius of its plane and not
// beyond any edge? Optionally returns the distance from the plane. (The
// "!(a <= b)" tests are how retail compares: c.le.s then bc1f.)
int Portal::SphereIntersects(_D3DVECTOR *center, float radius, float *distance)
{
    _D3DVECTOR d = _D3DVECTOR(center->x - m_corners[0].x, center->y - m_corners[0].y, center->z - m_corners[0].z);
    float planeDistance = DotProduct(&d, &m_normal);
    if (distance)
        *distance = Fabs(planeDistance);
    if (!(planeDistance <= radius) || planeDistance < -radius)
        return 0;
    if (!(DotProduct(&d, &m_edgeNormals[0]) <= radius))
        return 0;
    for (int i = 1; i < 4; i++) {
        d = _D3DVECTOR(center->x - m_corners[i].x, center->y - m_corners[i].y, center->z - m_corners[i].z);
        if (!(DotProduct(&d, &m_edgeNormals[i]) <= radius))
            return 0;
    }
    return 1;
}


// ---- src/Portal/Render3D_00300000.cpp

int Portal::Render3D()
{
    return 1;
}


