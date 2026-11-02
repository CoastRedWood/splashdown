// OBBOverlap.h — OBBOverlapTest's body as an inline:
// TestBoundingBoxTreesForOverlap inlines it.
#ifndef GLOBAL_OBBOVERLAP_H
#define GLOBAL_OBBOVERLAP_H

#include "d3dtypes.h"
#include "global/Vu0Inline.h"
#include "global/Vu0More.h"
#include "global/FabsInline.h"

extern _D3DVECTOR TestAxis[15];

// Do the box (center, half) and the box (center2, half2) in m's space
// overlap?  Separating-axis test over the 15 candidate axes.
static inline int OBBOverlapTestInline(_D3DVECTOR *center, _D3DVECTOR *half, _D3DVECTOR center2, _D3DVECTOR half2, _D3DMATRIX *m)
{
    Vu0TransformPoint(m, &center2, &center2);
    _D3DVECTOR a[3], b[3];
    a[0] = _D3DVECTOR(half->x, 0.0f, 0.0f);
    a[1] = _D3DVECTOR(0.0f, half->y, 0.0f);
    a[2] = _D3DVECTOR(0.0f, 0.0f, half->z);
    b[0] = _D3DVECTOR(half2.x, 0.0f, 0.0f);
    b[1] = _D3DVECTOR(0.0f, half2.y, 0.0f);
    b[2] = _D3DVECTOR(0.0f, 0.0f, half2.z);
    Vu0ApplyMatrixVector(&b[0], m, b[0]);
    Vu0ApplyMatrixVector(&b[1], m, b[1]);
    Vu0ApplyMatrixVector(&b[2], m, b[2]);
    _D3DVECTOR d = center2 - *center;
    for (int i = 0; i < 15; i++) {
        switch (i) {
        case 0:
            TestAxis[i] = _D3DVECTOR(1.0f, 0.0f, 0.0f);
            break;
        case 1:
            TestAxis[i] = _D3DVECTOR(0.0f, 1.0f, 0.0f);
            break;
        case 2:
            TestAxis[i] = _D3DVECTOR(0.0f, 0.0f, 1.0f);
            break;
        case 3:
            TestAxis[i] = _D3DVECTOR(m->_11, m->_12, m->_13);
            break;
        case 4:
            TestAxis[i] = _D3DVECTOR(m->_21, m->_22, m->_23);
            break;
        case 5:
            TestAxis[i] = _D3DVECTOR(m->_31, m->_32, m->_33);
            break;
        case 6:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[0], &TestAxis[3]);
            break;
        case 7:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[0], &TestAxis[4]);
            break;
        case 8:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[0], &TestAxis[5]);
            break;
        case 9:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[1], &TestAxis[3]);
            break;
        case 10:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[1], &TestAxis[4]);
            break;
        case 11:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[1], &TestAxis[5]);
            break;
        case 12:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[2], &TestAxis[3]);
            break;
        case 13:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[2], &TestAxis[4]);
            break;
        case 14:
            Vu0CrossProduct(&TestAxis[i], &TestAxis[2], &TestAxis[5]);
            break;
        }
        float rd = Fabs(Vu0DotProduct(&d, &TestAxis[i]));
        float ra = Fabs(Vu0DotProduct(&a[0], &TestAxis[i])) + Fabs(Vu0DotProduct(&a[1], &TestAxis[i])) + Fabs(Vu0DotProduct(&a[2], &TestAxis[i]));
        float rb = Fabs(Vu0DotProduct(&b[0], &TestAxis[i])) + Fabs(Vu0DotProduct(&b[1], &TestAxis[i])) + Fabs(Vu0DotProduct(&b[2], &TestAxis[i]));
        if (rd > ra + rb)
            return 0;
    }
    return 1;
}

#endif
