// _D3DCOLORVALUE alone, for files that can't include d3dtypes.h (they keep
// their own _D3DVECTOR). Same guard as the copy in include/d3dtypes.h.
#ifndef D3DCOLORVALUE_DEFINED
#define D3DCOLORVALUE_DEFINED
struct _D3DCOLORVALUE {
    float r, g, b, a;
};
#endif
