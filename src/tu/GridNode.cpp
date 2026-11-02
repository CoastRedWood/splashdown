// GridNode.cpp  @ 0x0023F480 (text 0x23F480-0x241BE0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's GridNode.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the
// object's data as well as its code (tools/link_c.py).



#include "d3dtypes.h"
#include "GridNodeGeometryBase/GridNodeGeometryBase.h"
#include "DrawableGridNode/DrawableGridNode.h"
#include "Terrain/Terrain.h"
#include "sinit/AxisVectors.h"

// ---- src/GridNode/dtor.cpp
GridNode::~GridNode()
{
    if (m_childOffsets)
        delete[] m_childOffsets;
    if (m_children) {
        for (int i = 0; i < 256; i++) {
            if (m_children[i])
                delete m_children[i];
        }
        delete[] m_children;
    }
}


// ---- src/GridNode/GetPatch.cpp

// Child-table row offsets for a 16x16 block (SizeBy[y] + x). Neither table
// is const: retail has them in .data, between Griddraw.cpp's and HiResMeter.cpp's.
static int SizeBy[17] = {
    0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80,
    0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0, 0x100,
};

// Vertex-table row offsets for a 17x17 grid (SizeVerticesBy[y] + x).
static int SizeVerticesBy[18] = {
    0x000, 0x011, 0x022, 0x033, 0x044, 0x055, 0x066, 0x077, 0x088,
    0x099, 0x0AA, 0x0BB, 0x0CC, 0x0DD, 0x0EE, 0x0FF, 0x110, 0x121,
};

// Small helpers: written inline, the products' FP operand order differs from retail.
static inline float Scale(float a, float b) { return a * b; }
static inline float Unpack(signed char c, float scale) { return c * scale; }

// Walks down to the leaf holding (x, y), then reads the 2x2 patch of
// heights, normals and flags at (x, y) and (x + step, y + step).
int GridNode::GetPatch(int x, int y, float *heights, _D3DVECTOR *normals, unsigned char *flags, int step)
{
//PB
    GridNode *node;
    int i;
    int levels;
    int shift;
//PE
    node = this;
    levels = m_level;
    shift = levels * 4;
    for (i = 0; i < levels; i++) {
        if (node->m_children) {
            int cx = x >> shift;
            int mask = ~(-1 << shift);
            int cy = y >> shift;
            if (cx >= 16 || cy >= 16 || !(node = node->m_children[SizeBy[cy] + cx]))
                return 0;
            shift -= 4;
            x &= mask;
            y &= mask;
        } else {
            return 0;
        }
    }
    if (node) {
    float scale = node->m_x10;
    if (node->m_heightScale == 1.0f) {
        heights[0] = scale * node->m_lod->GetHeight(x, y);
        heights[1] = scale * node->m_lod->GetHeight(x + step, y);
        heights[3] = scale * node->m_lod->GetHeight(x + step, y + step);
        heights[2] = scale * node->m_lod->GetHeight(x, y + step);
    } else {
        {
            int base = node->m_lod->m_baseHeight;
            heights[0] = scale * (base + node->m_heightScale * (node->m_lod->GetHeight(x, y) - base));
        }
        {
            int base = node->m_lod->m_baseHeight;
            heights[1] = scale * (base + node->m_heightScale * (node->m_lod->GetHeight(x + step, y) - base));
        }
        {
            int base = node->m_lod->m_baseHeight;
            heights[3] = scale * (base + node->m_heightScale * (node->m_lod->GetHeight(x + step, y + step) - base));
        }
        {
            int base = node->m_lod->m_baseHeight;
            heights[2] = scale * (base + node->m_heightScale * (node->m_lod->GetHeight(x, y + step) - base));
        }
    }
    if (normals) {
        float normalScale = Scale(node->m_heightScale, 1.0f / 127.0f);
        ByteVector *n;
        n = node->m_lod->GetNormal(x, y);
        normals[0].x = Unpack(n->x, normalScale);
        normals[0].y = Unpack(n->y, 1.0f / 127.0f);
        normals[0].z = Unpack(n->z, normalScale);
        n = node->m_lod->GetNormal(x + step, y);
        normals[1].x = Unpack(n->x, normalScale);
        normals[1].y = Unpack(n->y, 1.0f / 127.0f);
        normals[1].z = Unpack(n->z, normalScale);
        n = node->m_lod->GetNormal(x + step, y + step);
        normals[3].x = Unpack(n->x, normalScale);
        normals[3].y = Unpack(n->y, 1.0f / 127.0f);
        normals[3].z = Unpack(n->z, normalScale);
        n = node->m_lod->GetNormal(x, y + step);
        normals[2].x = Unpack(n->x, normalScale);
        normals[2].y = Unpack(n->y, 1.0f / 127.0f);
        normals[2].z = Unpack(n->z, normalScale);
    }
    if (flags) {
        flags[0] = node->m_lod->GetUserFlagData(x, y);
        flags[1] = node->m_lod->GetUserFlagData(x + step, y);
        flags[3] = node->m_lod->GetUserFlagData(x + step, y + step);
        flags[2] = node->m_lod->GetUserFlagData(x, y + step);
    }
    } else {
        return 0;
    }
    return 1;
}


// ---- src/GridNode/RayIntersects.cpp
// RayIntersects__8GridNodeFR10_D3DVECTORR10_D3DVECTORR10_D3DVECTORPP8GridNodePiPi  @ 0x0023FB00

#pragma fast_fptosi on


void RayIntersectPlane(_D3DVECTOR &start, _D3DVECTOR &end, _D3DVECTOR &a, _D3DVECTOR &b, _D3DVECTOR &c, _D3DVECTOR &hit);

// Retail's fmin/fmax: min.s/max.s into $f8, stored through a stack temporary.
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

// World height of vertex i of this leaf's 17x17 grid.
inline float GridNode::VertexHeight(int i)
{
    return m_x10 * ((float)m_lod->m_baseHeight +
                    m_heightScale * (float)(m_lod->GetHeight(i) - m_lod->m_baseHeight));
}

// Finds where the segment start..end (node-local) first hits the terrain under
// this node: the points where it crosses child-cell lines in x and in y are
// merged in ray order (as in RayIntersectsNodes), then each piece is handed to
// the child it lies over or, in a leaf, tested against the cell's two
// triangles. On a hit, stores the world position, the leaf and the rounded
// local x/y.
//
// Codegen notes: the Fmin/Fmax asm turns the global optimizer off, so local
// declaration and statement order set the registers. i and j count the x and
// y crossings and are then reused as the cell coordinates of the last loops
// (retail keeps them in $s0/$s1 throughout). The children loop's index is its
// own block local (a volatile register), the leaf loop's the function's
// ($s4, live across the height calls). dx0/dy0/dx1/dy1 are the offsets to the
// two diagonal corners that split the cell, named per split direction.
int GridNode::RayIntersects(_D3DVECTOR &start, _D3DVECTOR &end, _D3DVECTOR &hit, GridNode **nodes, int *x, int *z)
{
    _D3DVECTOR xs[17];
    _D3DVECTOR ys[17];
    _D3DVECTOR pts[34];
    _D3DVECTOR h;                       // hit on the triangle's plane
    _D3DVECTOR v0;                      // triangle corners
    _D3DVECTOR v1;
    _D3DVECTOR v2;
    int i;
    int j;
    int total;
    int k;
    int nx;
    int ny;
    int index;
    float dx = end.x - start.x;
    float dy = end.y - start.y;
    float dz = end.z - start.z;
    i = 0;
    float cell = (float)(1 << m_shift);
    j = 0;
    if (dx != 0.0f) {
        float step = dx < 0.0f ? -cell : cell;
        float slopeZ = dz / dx;
        float slopeY = dy / dx;
        float z = start.z;
        float y = start.y;
        float x = step * (int)(start.x / step);
        if (step < 0.0f) x -= step;
        float incY = step * slopeY;
        float incZ = step * slopeZ;
        if (x == start.x) {
            xs[i].x = start.x;
            xs[i].y = start.y;
            xs[i].z = start.z;
            i++;
        } else {
            x += step;
            if ((dx > 0.0f && x <= end.x) || (dx < 0.0f && x >= end.x)) {
                xs[i].x = x;
                y += slopeY * (x - start.x);
                xs[i].y = y;
                z += slopeZ * (x - start.x);
                xs[i].z = z;
                i++;
            }
        }
        if (dx > 0.0f) {
            while ((x += step) <= end.x) {
                y += incY;
                z += incZ;
                xs[i].x = x;
                xs[i].y = y;
                xs[i].z = z;
                i++;
            }
        } else if (dx < 0.0f) {
            while ((x += step) >= end.x) {
                y += incY;
                z += incZ;
                xs[i].x = x;
                xs[i].y = y;
                xs[i].z = z;
                i++;
            }
        }
    }
    if (dy != 0.0f) {
        float step = dy < 0.0f ? -cell : cell;
        float slopeZ = dz / dy;
        float slopeX = dx / dy;
        float z = start.z;
        float x = start.x;
        float y = step * (int)(start.y / step);
        if (step < 0.0f) y -= step;
        float incX = step * slopeX;
        float incZ = step * slopeZ;
        if (y == start.y) {
            ys[j].x = start.x;
            ys[j].y = start.y;
            ys[j].z = start.z;
            j++;
        } else {
            y += step;
            if ((dy > 0.0f && y <= end.y) || (dy < 0.0f && y >= end.y)) {
                x += slopeX * (y - start.y);
                ys[j].x = x;
                ys[j].y = y;
                z += slopeZ * (y - start.y);
                ys[j].z = z;
                j++;
            }
        }
        if (dy > 0.0f) {
            while ((y += step) <= end.y) {
                x += incX;
                z += incZ;
                ys[j].x = x;
                ys[j].y = y;
                ys[j].z = z;
                j++;
            }
        } else if (dy < 0.0f) {
            while ((y += step) >= end.y) {
                x += incX;
                z += incZ;
                ys[j].x = x;
                ys[j].y = y;
                ys[j].z = z;
                j++;
            }
        }
    }
    total = i + j;
    nx = i;
    ny = j;
    i = 0;
    j = 0;
    k = 0;
    if (!((xs[0].x == start.x && xs[0].y == start.y) || (ys[0].x == start.x && ys[0].y == start.y))) {
        pts[k].x = start.x;
        pts[k].y = start.y;
        pts[k].z = start.z;
        k++;
        total++;
    }
    if (dx == 0.0f) {
        for (; k < total; k++) {
            pts[k].x = ys[j].x;
            pts[k].y = ys[j].y;
            pts[k].z = ys[j].z;
            j++;
        }
    } else {
        for (; k < total; k++) {
            if (i < nx) {
                if (j < ny) {
                    if ((dx > 0.0f && xs[i].x < ys[j].x) || (dx < 0.0f && xs[i].x > ys[j].x)) {
                        pts[k].x = xs[i].x;
                        pts[k].y = xs[i].y;
                        pts[k].z = xs[i].z;
                        i++;
                        if (pts[k].x == ys[j].x || pts[k].y == ys[j].y) {
                            j++;
                            total--;
                        }
                    } else {
                        pts[k].x = ys[j].x;
                        pts[k].y = ys[j].y;
                        pts[k].z = ys[j].z;
                        j++;
                        if (pts[k].x == xs[i].x || pts[k].y == xs[i].y) {
                            i++;
                            total--;
                        }
                    }
                } else {
                    if (!k || pts[k - 1].x != xs[i].x || pts[k - 1].y != xs[i].y) {
                        pts[k].x = xs[i].x;
                        pts[k].y = xs[i].y;
                        pts[k].z = xs[i].z;
                    } else {
                        total--;
                    }
                    i++;
                }
            } else {
                if (!k || pts[k - 1].x != ys[j].x || pts[k - 1].y != ys[j].y) {
                    pts[k].x = ys[j].x;
                    pts[k].y = ys[j].y;
                    pts[k].z = ys[j].z;
                } else {
                    total--;
                }
                j++;
            }
        }
    }
    if (!(pts[total - 1].x == end.x && pts[total - 1].y == end.y)) {
        pts[total].x = end.x;
        pts[total].y = end.y;
        pts[total].z = end.z;
        total++;
    }
    if (m_children) {
        for (k = 0; k < total - 1; k++) {
            float px = (pts[k].x + pts[k + 1].x) / (2.0f * cell);
            float py = (pts[k].y + pts[k + 1].y) / (2.0f * cell);
            i = (int)px;
            j = (int)py;
            if (i == 16)
                i = 15;
            if (j == 16)
                j = 15;
            int index = i + SizeBy[j];
            GridNode *child = m_children[index];
            if (!child)
                continue;
            if ((pts[k].z < child->m_minHeight && pts[k + 1].z < child->m_minHeight)
                || (pts[k].z > child->m_maxHeight && pts[k + 1].z > child->m_maxHeight))
                continue;
            _D3DVECTOR a = pts[k] - _D3DVECTOR(i * cell, j * cell, 0.0f);
            _D3DVECTOR b = pts[k + 1] - _D3DVECTOR(i * cell, j * cell, 0.0f);
            if (m_children[index]->RayIntersects(a, b, hit, nodes, x, z))
                return 1;
        }
    } else {
        for (k = 0; k < total - 1; k++) {
            float px = (pts[k].x + pts[k + 1].x) / (2.0f * cell);
            float py = (pts[k].y + pts[k + 1].y) / (2.0f * cell);
            i = (int)px;
            j = (int)py;
            index = i + SizeVerticesBy[j];
            float h00 = VertexHeight(index);
            float h10 = VertexHeight(index + 1);
            float h01 = VertexHeight(index + 0x11);
            float h11 = VertexHeight(index + 0x12);
            float lo = Fmin(Fmin(h00, h10), Fmin(h01, h11));
            float hi = Fmax(Fmax(h00, h10), Fmax(h01, h11));
            if ((pts[k].z < lo && pts[k + 1].z < lo) || (pts[k].z > hi && pts[k + 1].z > hi))
                continue;
            float dx0, dy0, dx1, dy1;
            float fx = i;
            float fy = j;
            float fx1 = 1.0f + fx;
            float fy1 = 1.0f + fy;
            if ((i & 1) != (j & 1)) {
                v0 = _D3DVECTOR(fx, fy, h00);
                v1 = _D3DVECTOR(fx1, fy, h10);
                v2 = _D3DVECTOR(fx, fy1, h01);
                RayIntersectPlane(pts[k], pts[k + 1], v0, v1, v2, h);
                if (h.x <= fx1 && h.x >= fx && h.y <= fy1 && h.y >= fy) {
                    dx0 = h.x - fx;
                    dy0 = h.y - fy;
                    dx1 = h.x - fx1;
                    dy1 = h.y - fy1;
                    if (dx0 * dx0 + dy0 * dy0 <= dx1 * dx1 + dy1 * dy1)
                        goto found;
                }
                v0 = _D3DVECTOR(fx1, fy, h10);
                v1 = _D3DVECTOR(fx1, fy1, h11);
                v2 = _D3DVECTOR(fx, fy1, h01);
                RayIntersectPlane(pts[k], pts[k + 1], v0, v1, v2, h);
                if (h.x <= fx1 && h.x >= fx && h.y <= fy1 && h.y >= fy) {
                    dx0 = h.x - fx;
                    dy0 = h.y - fy;
                    dx1 = h.x - fx1;
                    dy1 = h.y - fy1;
                    if (dx1 * dx1 + dy1 * dy1 <= dx0 * dx0 + dy0 * dy0)
                        goto found;
                }
            } else {
                v0 = _D3DVECTOR(fx, fy, h00);
                v1 = _D3DVECTOR(fx1, fy, h10);
                v2 = _D3DVECTOR(fx1, fy1, h11);
                RayIntersectPlane(pts[k], pts[k + 1], v0, v1, v2, h);
                if (h.x <= fx1 && h.x >= fx && h.y <= fy1 && h.y >= fy) {
                    dx0 = h.x - fx1;
                    dy0 = h.y - fy;
                    dx1 = h.x - fx;
                    dy1 = h.y - fy1;
                    if (dx0 * dx0 + dy0 * dy0 <= dx1 * dx1 + dy1 * dy1)
                        goto found;
                }
                v0 = _D3DVECTOR(fx, fy, h00);
                v1 = _D3DVECTOR(fx1, fy1, h11);
                v2 = _D3DVECTOR(fx, fy1, h01);
                RayIntersectPlane(pts[k], pts[k + 1], v0, v1, v2, h);
                if (h.x <= fx1 && h.x >= fx && h.y <= fy1 && h.y >= fy) {
                    dx0 = h.x - fx1;
                    dy0 = h.y - fy;
                    dx1 = h.x - fx;
                    dy1 = h.y - fy1;
                    if (dx1 * dx1 + dy1 * dy1 <= dx0 * dx0 + dy0 * dy0)
                        goto found;
                }
            }
        }
    }
    return 0;

found:
    if ((h.x < start.x && h.x < end.x) || (h.x > start.x && h.x > end.x)
        || (h.y < start.y && h.y < end.y) || (h.y > start.y && h.y > end.y)
        || (h.z < start.z && h.z < end.z) || (h.z > start.z && h.z > end.z))
        return 0;
    hit.x = h.x + (float)(m_x << (m_level * 4));
    hit.y = h.y + (float)(m_y << (m_level * 4));
    hit.z = h.z;
    if (nodes)
        *nodes = this;
    if (x)
        *x = (int)(h.x + 0.5);
    if (z)
        *z = (int)(h.y + 0.5);
    return 1;
}


// ---- src/GridNode/RayIntersectsNodes.cpp
// RayIntersectsNodes__8GridNodeFR10_D3DVECTORR10_D3DVECTORPP8GridNodePii  @ 0x00241080
// Codegen notes: statement and declaration orders are search optima (the x/y
// branch statement order, `float dx;` declared apart, `j = (int)...` in the y
// branch). The merge loop writes through a pointer `p` declared before `i`
// and stepped in a guarded do-while (a named pointer colours before i, which
// retail keeps in $a2 with the pointer in $a1). The last loop's cell origin is
// two `_D3DVECTOR(cx * cell, cy * cell, 0.0f)` temporaries, which converts cy
// before cx as retail does.

// Collects the leaf nodes the segment start..end passes over: the points
// where it crosses child-cell lines in x and in y, merged in ray order, then
// each piece between two points is handed to the child it lies in.
int GridNode::RayIntersectsNodes(_D3DVECTOR &start, _D3DVECTOR &end, GridNode **nodes, int *count, int max)
{
    _D3DVECTOR xs[17];
    _D3DVECTOR ys[17];
    _D3DVECTOR pts[34];
    if (m_children) {
        int nx;
        int ny;
        int k;
        int cx;
        int total;
        int cy;
        int index;
        GridNode *child;
        int j;
        _D3DVECTOR *p;
        int i;
        float dx;
        dx = end.x - start.x;
        float dy = end.y - start.y;
        float dz = end.z - start.z;
        nx = 0;
        float cell = (float)(1 << m_shift);
        ny = 0;
        if (dx != 0.0f) {
            float z = start.z;
            float y = start.y;
            float step = dx < 0.0f ? -cell : cell;
            float slopeZ = dz / dx;
            float slopeY = dy / dx;
            i = (int)(start.x / step);
            float x = step * i;
            if (step < 0.0f) x -= step;
            float incY = step * slopeY;
            float incZ = step * slopeZ;
            if (x == start.x) {
                xs[nx].x = start.x;
                xs[nx].y = start.y;
                xs[nx].z = start.z;
                nx++;
            } else {
                x += step;
                if ((dx > 0.0f && x <= end.x) || (dx < 0.0f && x >= end.x)) {
                    xs[nx].x = x;
                    y += slopeY * (x - start.x);
                    xs[nx].y = y;
                    z += slopeZ * (x - start.x);
                    xs[nx].z = z;
                    nx++;
                }
            }
            if (dx > 0.0f) {
                while ((x += step) <= end.x) {
                    y += incY;
                    z += incZ;
                    xs[nx].x = x;
                    xs[nx].y = y;
                    xs[nx].z = z;
                    nx++;
                }
            } else if (dx < 0.0f) {
                while ((x += step) >= end.x) {
                    y += incY;
                    z += incZ;
                    xs[nx].x = x;
                    xs[nx].y = y;
                    xs[nx].z = z;
                    nx++;
                }
            }
        }
        if (dy != 0.0f) {
            float step = dy < 0.0f ? -cell : cell;
            float slopeZ = dz / dy;
            float slopeX = dx / dy;
            float z = start.z;
            float x = start.x;
            j = (int)(start.y / step);
            float y = step * j;
            if (step < 0.0f) y -= step;
            float incX = step * slopeX;
            float incZ = step * slopeZ;
            if (y == start.y) {
                ys[ny].x = start.x;
                ys[ny].y = start.y;
                ys[ny].z = start.z;
                ny++;
            } else {
                y += step;
                if ((dy > 0.0f && y <= end.y) || (dy < 0.0f && y >= end.y)) {
                    x += slopeX * (y - start.y);
                    ys[ny].x = x;
                    ys[ny].y = y;
                    z += slopeZ * (y - start.y);
                    ys[ny].z = z;
                    ny++;
                }
            }
            if (dy > 0.0f) {
                while ((y += step) <= end.y) {
                    x += incX;
                    z += incZ;
                    ys[ny].x = x;
                    ys[ny].y = y;
                    ys[ny].z = z;
                    ny++;
                }
            } else if (dy < 0.0f) {
                while ((y += step) >= end.y) {
                    x += incX;
                    z += incZ;
                    ys[ny].x = x;
                    ys[ny].y = y;
                    ys[ny].z = z;
                    ny++;
                }
            }
        }
        total = nx + ny;
        i = 0;
        j = 0;
        k = 0;
        if (!((xs[0].x == start.x && xs[0].y == start.y) || (ys[0].x == start.x && ys[0].y == start.y))) {
            pts[k].x = start.x;
            pts[k].y = start.y;
            pts[k].z = start.z;
            k++;
            total++;
        }
        if (dx == 0.0f) {
            for (; k < total; k++) {
                pts[k].x = ys[j].x;
                pts[k].y = ys[j].y;
                pts[k].z = ys[j].z;
                j++;
            }
        } else {
            if (k < total) {
                p = &pts[k];
                do {
                    if (i < nx) {
                        if (j < ny) {
                            if ((dx > 0.0f && xs[i].x < ys[j].x) || (dx < 0.0f && xs[i].x > ys[j].x)) {
                                p->x = xs[i].x;
                                p->y = xs[i].y;
                                p->z = xs[i].z;
                                i++;
                                if (p->x == ys[j].x || p->y == ys[j].y) {
                                    j++;
                                    total--;
                                }
                            } else {
                                p->x = ys[j].x;
                                p->y = ys[j].y;
                                p->z = ys[j].z;
                                j++;
                                if (p->x == xs[i].x || p->y == xs[i].y) {
                                    i++;
                                    total--;
                                }
                            }
                        } else {
                            if (!k || p[-1].x != xs[i].x || p[-1].y != xs[i].y) {
                                p->x = xs[i].x;
                                p->y = xs[i].y;
                                p->z = xs[i].z;
                            } else {
                                total--;
                            }
                            i++;
                        }
                    } else {
                        if (!k || p[-1].x != ys[j].x || p[-1].y != ys[j].y) {
                            p->x = ys[j].x;
                            p->y = ys[j].y;
                            p->z = ys[j].z;
                        } else {
                            total--;
                        }
                        j++;
                    }
                    k++;
                    p++;
                } while (k < total);
            }
        }
        if (!(pts[total - 1].x == end.x && pts[total - 1].y == end.y)) {
            pts[total].x = end.x;
            pts[total].y = end.y;
            pts[total].z = end.z;
            total++;
        }
        for (i = 0; i < total - 1; i++) {
            cx = (int)((pts[i].x + pts[i + 1].x) / (2.0f * cell));
            cy = (int)((pts[i].y + pts[i + 1].y) / (2.0f * cell));
            if (cx == 16)
                cx = 15;
            if (cy == 16)
                cy = 15;
            index = cx + SizeBy[cy];
            child = m_children[index];
            if (!child)
                continue;
            if ((pts[i].z < child->m_minHeight && pts[i + 1].z < child->m_minHeight)
                || (pts[i].z > child->m_maxHeight && pts[i + 1].z > child->m_maxHeight))
                continue;
            _D3DVECTOR a = pts[i] - _D3DVECTOR(cx * cell, cy * cell, 0.0f);
            _D3DVECTOR b = pts[i + 1] - _D3DVECTOR(cx * cell, cy * cell, 0.0f);
            if (m_children[index]->RayIntersectsNodes(a, b, nodes, count, max) && *count == max)
                return 1;
        }
    } else {
        if (*count < max)
            nodes[(*count)++] = this;
        return 1;
    }
    return count == 0 ? 0 : 1;
}


// ---- src/GridNode/SubstituteGeometry.cpp
#pragma fast_fptosi reset

void GridNode::SubstituteGeometry(GridNodeGeometryBase *oldGeometry, GridNodeGeometryBase *newGeometry)
{
    if (m_children) {
        for (int i = 0; i < 256; i++) {
            if (m_children[i])
                m_children[i]->SubstituteGeometry(oldGeometry, newGeometry);
        }
    }
    if (m_lod == oldGeometry)
        m_lod = newGeometry;
}


// ---- src/GroundFogableObject/ctor_00241BD0.cpp
GroundFogableObject::GroundFogableObject()
{
    m_groundFog = 0;
}


// ---- src/sinit/GridNode.cpp
// Static initializer of GridNode.cpp: only the axis vectors from the shared header.

