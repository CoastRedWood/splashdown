// SoultreeObject: the engine's scene-graph node (a SearchableObject and a
// GameObject). 0x1B0 bytes: data to +0x1A8, then its own vptr at +0x1A8
// (the GameObject subobject at +0x08 keeps its own), rounded up to 8.
// Names follow SoultreeObject's own methods (src/SoultreeObject/).
#ifndef SOULTREEOBJECT_SOULTREEOBJECT_H
#define SOULTREEOBJECT_SOULTREEOBJECT_H

#include "GameObject/GameObject.h"

extern "C" int strcasecmp(const char *a, const char *b);

#include "d3dtypes.h"

struct Quaternion;
class SearchNode;

_D3DMATRIX IdentityMatrix();
class SearchableObject;

// SoultreeObject's first base: 8 bytes in front of its GameObject subobject.
#include "SearchableObject/SearchableObject.h"

class SearchPlacement;
class ParameterBlock;

class SoultreeObject : public SearchableObject, public GameObject {
public:
    using SearchableObject::m_hidden;   // +0x03 bit 2, GameObject has its own m_hidden
    SoultreeObject(int);
    void Hide();
    void AddSoultreeChild(SoultreeObject *child);
    void AddSoultreeSibling(SoultreeObject *sibling);
    void RemoveSoultreeChild(SoultreeObject *child);
    int NumberOfObjectsInHierarchy();
    void SetUpdateGeometry();
    void InvalidateWorldMatrix();
    void InvalidateChildWorldMatrix();
    void RemoveFromSearch();
    void SetPosition(_D3DVECTOR position);
    void SetPosition(_D3DVECTOR *position);
    void GetPosition(_D3DVECTOR *position);
    void SetRotation(_D3DVECTOR axis, float angle);
    void SetRotation(float x, float y, float z, float angle);
    void GetMatrix(_D3DMATRIX *matrix);
    void Translate(_D3DVECTOR offset);
    void UpdateSearchPlacement();
    void GetCenterHalf(_D3DVECTOR *center, _D3DVECTOR *half);
    void FillObjectList(int *count, SoultreeObject **list);
    SoultreeObject *GetNamedObject(char *name);
    void GetBoundingBox(_D3DVECTOR *min, _D3DVECTOR *max);
    void ValidateWorldMatrix();
    void GetPosition(SoultreeObject *relativeTo, _D3DVECTOR *position);
    _D3DVECTOR GetPosition(SoultreeObject *relativeTo);
    _D3DVECTOR TransformVectorWorldToLocal(_D3DVECTOR *v);
    _D3DVECTOR TransformVectorLocalToWorld(_D3DVECTOR *v);
    _D3DVECTOR RotateVectorLocalToWorld(_D3DVECTOR *v);
    _D3DVECTOR RotateVectorWorldToLocal(_D3DVECTOR *v);
    void SetScale(float, float, float);
    void AddScale(float, float, float);
    void AddRotation(_D3DVECTOR axis, float angle);
    void SetPosition(float x, float y, float z);
    void SetPosition(SoultreeObject *relativeTo, _D3DVECTOR position);
    void SetPosition(SoultreeObject *relativeTo, _D3DVECTOR *position);
    void Translate(_D3DVECTOR *offset);
    void Translate(SoultreeObject *relativeTo, _D3DVECTOR *offset);
    void GetMatrix(SoultreeObject *relativeTo, _D3DMATRIX *matrix);
    void SetMatrix(_D3DMATRIX matrix);
    void SetMatrix(_D3DMATRIX *matrix);
    void SetMatrix(SoultreeObject *relativeTo, _D3DMATRIX *matrix);
    void GetOrientation(_D3DVECTOR *front, _D3DVECTOR *up);
    void GetOrientation(Quaternion *q);
    void GetOrientation(SoultreeObject *relativeTo, _D3DVECTOR *front, _D3DVECTOR *up);
    void SetOrientation(_D3DVECTOR *front, _D3DVECTOR *up, int, int, int);
    void SetOrientation(Quaternion *q);
    void SetOrientation(SoultreeObject *relativeTo, _D3DVECTOR *front, _D3DVECTOR *up, int, int, int);
    void ResetMatrices();
    void Unhide();
    void UpdateBoundingBox();
    void GetHierarchyBoundingBox(_D3DVECTOR *min, _D3DVECTOR *max);
    void AccumulateExtents(_D3DVECTOR *v, float *extents);
    void AccumulateExtents(int *count, _D3DVECTOR *min, _D3DVECTOR *max, _D3DMATRIX *matrix);
    void AddSearchPlacement(SearchNode *node, unsigned int flags);
    int ReadHierarchyFromFile(long file, SoultreeObject **objects);

    char m_name[0x40];                  // +0x38 (length not yet known)
    char pad78[0xC0 - 0x78];
    _D3DMATRIX m_localMatrix;           // +0xC0, _41/_42/_43 is the position
    _D3DMATRIX m_worldMatrix;           // +0x100
    _D3DVECTOR m_boundingBoxMin;        // +0x140
    _D3DVECTOR m_boundingBoxMax;        // +0x150
    _D3DVECTOR m_localCenter;           // +0x160, bounding box center (local)
    _D3DVECTOR m_localHalf;             // +0x170, bounding box half size (local)
    SoultreeObject *m_parent;           // +0x180
    SoultreeObject *m_child;            // +0x184
    SoultreeObject *m_sibling;          // +0x188
    SoultreeObject *m_prevSibling;      // +0x18C
    signed char m_worldMatrixValid : 1; // +0x190 bit 0
    signed char m_hiddenWhenEnabled : 1;// +0x190 bit 1, SearchableObject::m_hidden saved by Disable
    signed char m_x190_bit2 : 1;
    signed char m_x190_bit3 : 1;
    signed char m_x190_bit4 : 1;
    signed char m_boundingBoxValid : 1; // +0x190 bit 5
    signed char m_x190_bit6 : 1;
    signed char m_updateGeometry : 1;   // +0x190 bit 7
    signed char m_visible : 1;          // +0x191 bit 0
    signed char m_x191_bit1 : 1;        // +0x191 bit 1
    char pad192[0x194 - 0x192];
    float m_boundingRadius;             // +0x194
    float m_searchRadius;               // +0x198
    SearchPlacement *m_searchPlacement; // +0x19C
    int m_numObjects;                   // +0x1A0, objects in the hierarchy file
    ParameterBlock *m_paramBlock;       // +0x1A4

    // Own virtuals: vptr at +0x1A8 (the GameObject subobject keeps its own).
    virtual ~SoultreeObject();
    virtual void ReadFromBinaryFile(long);
    virtual void ReadFromParameterBlockFile();
    virtual void CreateNewObject(SoultreeObject **);
    virtual void CalculateBoundingBox();
    virtual void ResetTransforms();
    virtual void Clone(SoultreeObject *);
    virtual void CloneObject(SoultreeObject *, SoultreeObject *);
    virtual void Enable();
    virtual void Disable();
};

#endif
