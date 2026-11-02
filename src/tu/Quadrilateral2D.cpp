// Quadrilateral2D.cpp  @ 0x0030AC90 (text 0x30AC90-0x30B7F0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's Quadrilateral2D.cpp as one translation unit: ScreenPoint2D,
// EdgeVsEdge2D and ScreenQuad2D, then the static initializer (only the axis
// vectors). SimpleThrillCam.cpp follows.




#include "ScreenPoint2D/ScreenPoint2D.h"
#include "d3dtypes.h"
#include "ScreenQuad2D/ScreenQuad2D.h"
#include "soultree_assert_block.h"
#include "sinit/AxisVectors.h"

// ---- src/ScreenPoint2D/ctor.cpp

ScreenPoint2D::ScreenPoint2D(float x, float y)
{
    this->x = x;
    this->y = y;
}


// ---- src/ScreenPoint2D/ctor_0030ACA0.cpp

ScreenPoint2D::ScreenPoint2D()
{
}


// ---- src/global/EdgeVsEdge2D.cpp

// Intersection of segment a-b with segment c-d; the hit point goes to out.
int EdgeVsEdge2D(const ScreenPoint2D &a, const ScreenPoint2D &b, const ScreenPoint2D &c,
                 const ScreenPoint2D &d, ScreenPoint2D &out)
{
    // Declaration order picked for retail register allocation.
    float aby;
    float abx;
    float cdx;
    float dx;
    float dy;
    float inv;
    float u;
    float t;
    float denom;
    float cdy;
    float num;
    dx = d.x - b.x;
    abx = a.x - b.x;
    cdx = d.x - c.x;
    aby = a.y - b.y;
    dy = d.y - b.y;
    cdy = d.y - c.y;
    denom = abx * cdy - cdx * aby;
    if (denom == 0.0f)
        return 0;
    inv = 1.0f / denom;
    num = dx * cdy - dy * cdx;
    t = num * inv;
    if (t < 0.0f || t > 1.0f)
        return 0;
    u = abx * dy - aby * dx;
    u = u * inv;
    if (u < 0.0f || u > 1.0f)
        return 0;
    out.x = b.x + abx * t;
    out.y = b.y + aby * t;
    return 1;
}


// ---- src/ScreenQuad2D/Set.cpp
void ScreenQuad2D::Set(ScreenPoint2D *points)
{
    for (int i = 0; i < 4; i++) {
        m_points[i].x = points[i].x;
        m_points[i].y = points[i].y;
    }
}


// ---- src/ScreenQuad2D/Set_0030ADF0.cpp
void ScreenQuad2D::Set(_D3DVECTOR *points)
{
    for (int i = 0; i < 4; i++) {
        m_points[i].x = points[i].x;
        m_points[i].y = points[i].y;
    }
}


// ---- src/ScreenQuad2D/Set_0030AE20.cpp
void ScreenQuad2D::Set(ScreenQuad2D &other)
{
    for (int i = 0; i < 4; i++) {
        m_points[i].x = other.m_points[i].x;
        m_points[i].y = other.m_points[i].y;
    }
}


// ---- src/ScreenQuad2D/IsConvex.cpp


// Convex if each edge turns the same way into the next.
int ScreenQuad2D::IsConvex()
{
    ScreenPoint2D normals[4];
    ScreenPoint2D edges[4];
    int i;

    for (i = 0; i < 4; i++) {
        int next;
        if (i >= 3)
            next = 0;
        else
            next = i + 1;
        edges[i].x = m_points[next].x - m_points[i].x;
        edges[i].y = m_points[next].y - m_points[i].y;
    }
    for (i = 0; i < 4; i++) {
        normals[i].x = -edges[i].y;
        normals[i].y = edges[i].x;
        int next;
        if (i >= 3)
            next = 0;
        else
            next = i + 1;
        if (!(normals[i].y * edges[next].y + normals[i].x * edges[next].x <= 0.0f))
            return 0;
    }
    return 1;
}


// ---- src/ScreenQuad2D/MinMaxIntersect.cpp


int EdgeVsEdge2D(const ScreenPoint2D &a0, const ScreenPoint2D &a1, const ScreenPoint2D &b0, const ScreenPoint2D &b1,
                 ScreenPoint2D &hit);


// Intersects two quads into `out` (or their bounding box when the overlap
// isn't a convex quad).
int ScreenQuad2D::MinMaxIntersect(ScreenQuad2D &a, ScreenQuad2D &b, ScreenQuad2D &out)
{
    ScreenPoint2D points[24];
    ScreenPoint2D unused[4];
    ScreenPoint2D hit;
    int count = 0;
    int inA = 0;
    int inB = 0;
    int i;
    for (i = 0; i < 4; i++) {
        if (a.PointInQuad(b.m_points[i])) {
            points[count++] = b.m_points[i];
            inA++;
        }
    }
    if (inA == 4) {
        out.Set(b.m_points);
        return 1;
    }
    for (i = 0; i < 4; i++) {
        if (b.PointInQuad(a.m_points[i])) {
            points[count++] = a.m_points[i];
            inB++;
        }
    }
    if (inB == 4) {
        out.Set(a.m_points);
        return 1;
    }
    // The edge loops reuse the inA/inB counters, as retail does.
    for (inA = 0; inA < 4; inA++) {
        for (inB = 0; inB < 4; inB++) {
            int nextA;
            if (inA >= 3)
                nextA = 0;
            else
                nextA = inA + 1;
            int nextB;
            if (inB >= 3)
                nextB = 0;
            else
                nextB = inB + 1;
            if (EdgeVsEdge2D(a.m_points[inA], a.m_points[nextA], b.m_points[inB], b.m_points[nextB], hit))
                points[count++] = hit;
        }
    }
#line 197 "Quadrilateral2D.cpp"
    ASSERT(count < 25, "Intersection Array Overflowed");
    if (count == 0)
        return 0;
    if (count == 4) {
        out.Set(points);
        out.Orient();
        if (!out.IsConvex())
            out.SetToMinMax(points, count);
    } else {
        out.SetToMinMax(points, count);
    }
    return 1;
}


// ---- src/ScreenQuad2D/SetToMinMax.cpp


// The quad becomes the bounding rectangle of the points.
void ScreenQuad2D::SetToMinMax(ScreenPoint2D *points, int count)
{
    if (count > 0) {
        float minX, maxX, maxY, minY;
        minX = maxX = points[0].x;
        maxY = minY = points[0].y;
        for (int i = 1; i < count; i++) {
            if (points[i].x < minX)
                minX = points[i].x;
            if (points[i].x > maxX)
                maxX = points[i].x;
            if (points[i].y < minY)
                minY = points[i].y;
            if (points[i].y > maxY)
                maxY = points[i].y;
        }
        m_points[0] = ScreenPoint2D(minX, maxY);
        m_points[1] = ScreenPoint2D(maxX, maxY);
        m_points[2] = ScreenPoint2D(maxX, minY);
        m_points[3] = ScreenPoint2D(minX, minY);
    }
}


// ---- src/ScreenQuad2D/PointInQuad.cpp


// The point is inside if it is on the inner side of all four edges.
int ScreenQuad2D::PointInQuad(ScreenPoint2D p)
{
    ScreenPoint2D normal;
    ScreenPoint2D offset;
    for (int i = 0; i < 4; i++) {
        int next;
        if (i >= 3)
            next = 0;
        else
            next = i + 1;
        normal.x = m_points[i].y - m_points[next].y;
        normal.y = m_points[next].x - m_points[i].x;
        offset.x = p.x - m_points[i].x;
        offset.y = p.y - m_points[i].y;
        if (!(normal.x * offset.x + normal.y * offset.y <= 0.0f))
            return 0;
    }
    return 1;
}


// ---- src/ScreenQuad2D/Orient.cpp

// True if a, b, c wind clockwise on screen.
static inline int Clockwise(ScreenPoint2D &a, ScreenPoint2D &b, ScreenPoint2D &c)
{
    return a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y) < 0.0f;
}


// Reorder the corners so that they wind consistently.
void ScreenQuad2D::Orient()
{
    ScreenPoint2D p[4];
    for (int i = 0; i < 4; i++)
        p[i] = m_points[i];
    if (Clockwise(m_points[0], m_points[1], m_points[2])) {
        if (!Clockwise(m_points[2], m_points[3], m_points[0])) {
            if (Clockwise(m_points[1], m_points[3], m_points[2])) {
                m_points[2] = p[3];
                m_points[3] = p[2];
            } else {
                m_points[1] = p[3];
                m_points[2] = p[1];
                m_points[3] = p[2];
            }
        }
    } else if (Clockwise(m_points[0], m_points[3], m_points[2])) {
        m_points[1] = p[3];
        m_points[3] = p[1];
    } else if (Clockwise(m_points[1], m_points[3], m_points[0])) {
        m_points[1] = p[2];
        m_points[2] = p[1];
    } else {
        m_points[1] = p[2];
        m_points[2] = p[3];
        m_points[3] = p[1];
    }
}

