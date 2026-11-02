// RayFace.h — RayIntersectBoundingBoxTreeFace's body as an inline (round
// 33): RayBundleIntersectsBoundingBoxTree inlines it.
#ifndef GLOBAL_RAYFACE_H
#define GLOBAL_RAYFACE_H

#include "global/CollisionTree.h"
#include "global/Vu0Inline.h"
#include "global/Vu0More.h"
#include "global/FabsInline.h"

// x where the line y = c.y crosses the edge a-b (2D, in the projection plane).
static inline float CrossingX(_D3DVECTOR &a, _D3DVECTOR &b, _D3DVECTOR &c)
{
    return a.x + (b.x - a.x) * (c.y - a.y) / (b.y - a.y);
}

// Does the segment start + [0, length] * dir hit the face from its front?
// On a hit nearer than *t (or with *t negative) records t, the point and the
// face normal.
static inline int RayIntersectFace(BoundingBoxTreeFace *face, _D3DVECTOR dir, _D3DVECTOR start, float length,
                                   _D3DVECTOR *point, _D3DVECTOR *normal, float *t)
{
    _D3DVECTOR hit;
    _D3DVECTOR faceNormal(face->m_normalX, face->m_normalY, face->m_normalZ);
    _D3DVECTOR *v0 = &BoundingBoxTreeVertices[face->m_vertexIndex[0]];
    _D3DVECTOR *v1 = &BoundingBoxTreeVertices[face->m_vertexIndex[1]];
    _D3DVECTOR *v2 = &BoundingBoxTreeVertices[face->m_vertexIndex[2]];
    _D3DVECTOR absNormal, p0, p1, p2, q, c1, c2;
    float d = Vu0DotProduct(&faceNormal, &dir);
    if (d > 0.0f)
        return 0;
    d *= length;
    float s = -(face->m_distance + Vu0DotProductVf4(&start)) / d;
    if (s >= 0.0f && s <= 1.0f) {
        hit = start + length * (s * dir);
        absNormal.x = Fabs(face->m_normalX);
        absNormal.y = Fabs(face->m_normalY);
        absNormal.z = Fabs(face->m_normalZ);
        if (absNormal.z > absNormal.x && absNormal.z > absNormal.y) {
            p0.x = v0->x;
            p0.y = v0->y;
            p1.x = v1->x;
            p1.y = v1->y;
            p2.x = v2->x;
            p2.y = v2->y;
            q.x = hit.x;
            q.y = hit.y;
        } else if (absNormal.y > absNormal.x) {
            p0.x = v0->x;
            p0.y = v0->z;
            p1.x = v1->x;
            p1.y = v1->z;
            p2.x = v2->x;
            p2.y = v2->z;
            q.x = hit.x;
            q.y = hit.z;
        } else {
            p0.x = v0->y;
            p0.y = v0->z;
            p1.x = v1->y;
            p1.y = v1->z;
            p2.x = v2->y;
            p2.y = v2->z;
            q.x = hit.y;
            q.y = hit.z;
        }
        c2.y = c1.y = q.y;
        if (q.y <= p0.y && q.y >= p1.y) {
            if (q.y > p2.y) {
                c1.x = CrossingX(p0, p1, c1);
                c2.x = CrossingX(p0, p2, c2);
            } else {
                c1.x = CrossingX(p0, p1, c1);
                c2.x = CrossingX(p1, p2, c2);
            }
        } else if (q.y <= p1.y && q.y >= p0.y) {
            if (q.y > p2.y) {
                c1.x = CrossingX(p0, p1, c1);
                c2.x = CrossingX(p1, p2, c2);
            } else {
                c1.x = CrossingX(p0, p1, c1);
                c2.x = CrossingX(p0, p2, c2);
            }
        } else if (q.y <= p2.y && q.y >= p1.y) {
            if (q.y > p0.y) {
                c1.x = CrossingX(p1, p2, c1);
                c2.x = CrossingX(p0, p2, c2);
            } else {
                c1.x = CrossingX(p0, p1, c1);
                c2.x = CrossingX(p1, p2, c2);
            }
        } else if (q.y <= p1.y && q.y >= p2.y) {
            if (q.y > p0.y) {
                c1.x = CrossingX(p0, p1, c1);
                c2.x = CrossingX(p1, p2, c2);
            } else {
                c1.x = CrossingX(p0, p2, c1);
                c2.x = CrossingX(p1, p2, c2);
            }
        } else if (q.y <= p0.y && q.y >= p2.y) {
            if (q.y > p1.y) {
                c1.x = CrossingX(p0, p1, c1);
                c2.x = CrossingX(p0, p2, c2);
            } else {
                c1.x = CrossingX(p0, p2, c1);
                c2.x = CrossingX(p1, p2, c2);
            }
        } else if (q.y <= p2.y && q.y >= p0.y) {
            if (q.y > p1.y) {
                c1.x = CrossingX(p1, p2, c1);
                c2.x = CrossingX(p0, p2, c2);
            } else {
                c1.x = CrossingX(p0, p1, c1);
                c2.x = CrossingX(p0, p2, c2);
            }
        } else {
            return 0;
        }
        if ((q.x <= c1.x && q.x >= c2.x) || (q.x <= c2.x && q.x >= c1.x)) {
            if (t) {
                if (s < *t || *t < 0.0f) {
                    *t = s;
                    if (point)
                        *point = hit;
                    if (normal)
                        *normal = faceNormal;
                }
            }
            return 1;
        }
        return 0;
    }
    return 0;
}

#endif
