#ifndef D3DTYPES_H
#define D3DTYPES_H

#include "mwcc_shim.h"
#include "ps2_port.h"

// Heap-image shim: mwcc parses the
// inline member bodies below in reverse order, each in its own heap epoch, so
// MwccHeapShim is parsed just before the default constructor. Its const
// image leaves a 1 in the "has call" byte of the ctor's 0.0f node, as in
// retail: every inlined `_D3DVECTOR()` 0.0f then carries the flag
// (UIWidgetOverlayText::Create's DisplayText arguments load first). Checked
// no worse over every file that includes this header. The image's `@N`
// object goes to .mwccshim (mwcc_shim.h): the class's inline bodies are
// parsed at its end, inside the pragma, and none has a static or other data.
struct MwccD3DImage { char pad0[0x290]; char flag0; };

#pragma section mwccshim begin
// Soultree's _D3DVECTOR: Direct3D's vector with a w component, defaulting to
// the point (0, 0, 0, 1). The default constructor stores z, y, x, then w.
struct _D3DVECTOR {
    _D3DVECTOR() { x = y = z = 0.0f; w = 1.0f; }
    // Direct3D's one-float form (x = y = z = f). Each constructor declared
    // here moves the constant nodes of files that construct _D3DVECTORs in
    // mwcc's parser heap (by 0x10-0x60 bytes); the heap-image shims of the
    // files that depend on "has call" flags are calibrated with it present.
    _D3DVECTOR(float f) { x = y = z = f; w = 1.0f; }
    static void MwccHeapShim() { const MwccD3DImage image = { {0}, 1 }; }  // see MwccD3DImage
    _D3DVECTOR(float ax, float ay, float az) { x = ax; y = ay; z = az; w = 1.0f; }
    _D3DVECTOR(float ax, float ay, float az, float aw) { x = ax; y = ay; z = az; w = aw; }
    _D3DVECTOR(const float *v) { x = v[0]; y = v[1]; z = v[2]; w = 1.0f; }
    float &operator[](int i) { return (&x)[i]; }
    void operator+=(const _D3DVECTOR &v) { x += v.x; y += v.y; z += v.z; }
    void operator*=(float s) { x *= s; y *= s; z *= s; }
    int operator==(const _D3DVECTOR &v) const { return x == v.x && y == v.y && z == v.z; }
    bool operator!=(const _D3DVECTOR &v) const { return x != v.x || y != v.y || z != v.z; }
    float x, y, z, w;
};
#pragma section mwccshim end

// Free operators, as Direct3D's D3D_OVERLOADS declares them. As members they
// compile differently in some files.
inline _D3DVECTOR operator+(const _D3DVECTOR &a, const _D3DVECTOR &b)
{
    return _D3DVECTOR(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline _D3DVECTOR operator-(const _D3DVECTOR &a, const _D3DVECTOR &b)
{
    return _D3DVECTOR(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline _D3DVECTOR operator*(float s, const _D3DVECTOR &v)
{
    return _D3DVECTOR(s * v.x, s * v.y, s * v.z);
}

inline _D3DVECTOR operator*(const _D3DVECTOR &v, float s)
{
    return _D3DVECTOR(s * v.x, s * v.y, s * v.z);
}

inline _D3DVECTOR operator/(const _D3DVECTOR &v, float s)
{
    float r = 1.0f / s;
    return _D3DVECTOR(v.x * r, v.y * r, v.z * r);
}

// 16 named floats: an array member changes struct-copy codegen, and so does
// the empty default constructor (copies become lwc1/swc1 pairs). operator()
// is a compare chain, not an index.
struct _D3DMATRIX {
    _D3DMATRIX() {}
    float &operator()(int row, int column)
    {
        if (row == 0 && column == 0)
            return _11;
        if (row == 0 && column == 1)
            return _12;
        if (row == 0 && column == 2)
            return _13;
        if (row == 0 && column == 3)
            return _14;
        if (row == 1 && column == 0)
            return _21;
        if (row == 1 && column == 1)
            return _22;
        if (row == 1 && column == 2)
            return _23;
        if (row == 1 && column == 3)
            return _24;
        if (row == 2 && column == 0)
            return _31;
        if (row == 2 && column == 1)
            return _32;
        if (row == 2 && column == 2)
            return _33;
        if (row == 2 && column == 3)
            return _34;
        if (row == 3 && column == 0)
            return _41;
        if (row == 3 && column == 1)
            return _42;
        if (row == 3 && column == 2)
            return _43;
        return _44;
    }

    float _11, _12, _13, _14;
    float _21, _22, _23, _24;
    float _31, _32, _33, _34;
    float _41, _42, _43, _44;
};

// Direct3D 8's float RGBA colour (also defined, under the same guard, by
// LightEmitter/_D3DCOLORVALUE.h for files that keep their own _D3DVECTOR).
#ifndef D3DCOLORVALUE_DEFINED
#define D3DCOLORVALUE_DEFINED
struct _D3DCOLORVALUE {
    float r, g, b, a;
};
#endif

// Direct3D 8's viewport.
struct _D3DVIEWPORT8 {
    unsigned int X, Y, Width, Height;
    float MinZ, MaxZ;
};

// Direct3D 8 enums the render and texture interfaces take. Empty: only their
// mangled names matter. RenderTarget/D3DEnums.h defines them under the same
// guard for files that keep their own _D3DVECTOR.
#ifndef D3DENUMS_DEFINED
#define D3DENUMS_DEFINED
enum _D3DBLEND {};
enum _D3DTEXTURESTAGESTATETYPE {};
enum _D3DRENDERSTATETYPE {};
enum _D3DTEXTUREBLEND {};
#endif

#endif
