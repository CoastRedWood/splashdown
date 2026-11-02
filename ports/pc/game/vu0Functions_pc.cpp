// C versions of vu0Functions.cpp's two hand-written VU0 routines.
//
// Both transform the 8 corners of the box (center - half) .. (center + half)
// by the 4x4 matrix m, as vmulax/vmadday/vmaddaz/vmaddw do (the corner keeps
// center's w), and take each corner's 6 clip flags against |w| (vclipw.xyz:
// bit 0 x > +|w|, bit 1 x < -|w|, then y, then z). The box is outside when
// all corners share a flag: the result is 1 when the AND of the flags is not
// 0, else 0. The retail code leaves early once the AND is 0; the value is
// the same.
#include <cmath>

static unsigned ClipFlags(const float *m, const float *p)
{
    float v[4];
    for (int k = 0; k < 4; k++)
        v[k] = m[k] * p[0] + m[4 + k] * p[1] + m[8 + k] * p[2] + m[12 + k] * p[3];
    float w = std::fabs(v[3]);
    unsigned flags = 0;
    for (int k = 0; k < 3; k++) {
        if (v[k] > w)
            flags |= 1u << (2 * k);
        if (v[k] < -w)
            flags |= 2u << (2 * k);
    }
    return flags;
}

// The corner walk of the retail code: start at center - half, then step
// +x, +z, -x, +y, +x, -z, -x by the box's full size.
static void BoxFlags(const float *m, const float *center, const float *half, unsigned *all, unsigned *any)
{
    float p[4] = {center[0] - half[0], center[1] - half[1], center[2] - half[2], center[3]};
    float size[3] = {half[0] * 2.0f, half[1] * 2.0f, half[2] * 2.0f};
    static const int axis[7] = {0, 2, 0, 1, 0, 2, 0};
    static const float sign[7] = {1, 1, -1, 1, 1, -1, -1};
    unsigned f = ClipFlags(m, p);
    *all = 0x3F & f;
    *any = f;
    for (int c = 0; c < 7; c++) {
        p[axis[c]] += sign[c] * size[axis[c]];
        f = ClipFlags(m, p);
        *all &= f;
        *any |= f;
    }
}

extern "C" int Vu0BoundingBoxVisible(void *m, void *center, void *half)
{
    unsigned all, any;
    BoxFlags((const float *)m, (const float *)center, (const float *)half, &all, &any);
    return all != 0;
}

extern "C" int Vu0BoundingBoxVisibleClipCodes(void *m, void *center, void *half, unsigned int *codes)
{
    unsigned all, any;
    BoxFlags((const float *)m, (const float *)center, (const float *)half, &all, &any);
    *codes = any & 0x3F;
    return all != 0;
}
