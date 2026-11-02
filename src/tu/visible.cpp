// visible.cpp  @ 0x0029E920 (text 0x29E920-0x29EB50)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's visible.cpp as one translation unit: an unanchored TU in
// VisibilityQuadTree.cpp's tu_map row, VisibleTest's two functions, then the
// static initializer (__sinit_visible.cpp, the .ctor slot after
// VisibilityQuadTree.cpp's: the axis vectors at 0x475BC0, with the two
// profiler IDs after them). `make link` places the object's data as well as
// its code (tools/link_c.py).


#include "Camera/Camera.h"
#include "VisibleTest/VisibleTest.h"

VisibleTest GlobalVisibleTest;
VisibleTest *VisTest = &GlobalVisibleTest;
#include "sinit/AxisVectors.h"

// ---- src/VisibleTest/PointVisible.cpp


int VisibleTest::PointVisible(Camera *camera, _D3DMATRIX *m, _D3DVECTOR *point, _D3DVECTOR *screen, unsigned int *clipCodes)
{
    unsigned int clip = 0;
    if (screen) {
        asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x10(%1); lqc2 vf6, 0x20(%1); lqc2 vf7, 0x30(%1); lqc2 vf8, 0x0(%2); addi $2, $0, 0x3f; vmulax.xyzw ACC, vf4, vf8x; vmadday.xyzw ACC, vf5, vf8y; vmaddaz.xyzw ACC, vf6, vf8z; vmaddw.xyzw vf10, vf7, vf8w; vclipw.xyz vf10, vf10w; vdiv Q, vf0w, vf10w; vnop; vnop; vnop; vnop; vnop; cfc2 $3, vi18; and $2, $2, $3; vwaitq; vmulq.xyzw vf10, vf10, Q; sqc2 vf10, 0x0(%3); sw $2, 0x0(%0)"
            : : "r"(&clip), "r"(m), "r"(point), "r"(screen) : "$2", "$3", "memory");
        float halfWidth = camera->m_viewport.Width / 2.0f;
        float halfHeight = camera->m_viewport.Height / 2.0f;
        screen->x = halfWidth + screen->x * halfWidth;
        screen->y = halfHeight + screen->y * halfHeight;
    } else {
        asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x10(%1); lqc2 vf6, 0x20(%1); lqc2 vf7, 0x30(%1); lqc2 vf8, 0x0(%2); addi $2, $0, 0x3f; vmulax.xyzw ACC, vf4, vf8x; vmadday.xyzw ACC, vf5, vf8y; vmaddaz.xyzw ACC, vf6, vf8z; vmaddw.xyzw vf10, vf7, vf8w; vclipw.xyz vf10, vf10w; vnop; vnop; vnop; vnop; vnop; cfc2 $3, vi18; and $2, $2, $3; sw $2, 0x0(%0)"
            : : "r"(&clip), "r"(m), "r"(point) : "$2", "$3", "memory");
    }
    if (clipCodes)
        *clipCodes = clip;
    if (!clip)
        return 1;
    return 0;
}


// ---- src/VisibleTest/BoundingBoxVisible.cpp
extern "C" int Vu0BoundingBoxVisibleClipCodes(void *m, void *center, void *half, unsigned int *codes);
extern "C" int Vu0BoundingBoxVisible(void *m, void *center, void *half);


int VisibleTest::BoundingBoxVisible(_D3DMATRIX *m, _D3DVECTOR *center, _D3DVECTOR *half, unsigned int *clipCodes)
{
    if (clipCodes) {
        // Profiler IDs, initialized but never read (retail's names are swapped).
        static int Vu0BoundingBoxVisibleID = -1;
        return 1 - Vu0BoundingBoxVisibleClipCodes(m, center, half, clipCodes);
    }
    static int Vu0BoundingBoxVisibleClipCodesID = -1;
    return 1 - Vu0BoundingBoxVisible(m, center, half);
}


// ---- src/sinit/visible.cpp
// Static initializer of visible.cpp: only the axis vectors from the shared header.
