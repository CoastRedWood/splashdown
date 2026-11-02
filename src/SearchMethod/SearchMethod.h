// SearchMethod: abstract spatial-search interface (QuadTree, VisibilityQuadTree,
// SearchList derive from it). A BaseObject, so the vptr is at +0x08 and the
// class is 0x30 bytes (BaseObject's +0x0C is padding; its data starts at +0x10).
// Slots 4-7 and 9-14 are pure in retail (retail vtable); their
// signatures are QuadTree's overrides.
#ifndef SEARCHMETHOD_SEARCHMETHOD_H
#define SEARCHMETHOD_SEARCHMETHOD_H

#include "GameObject/GameObject.h"
#include "ContainerList/ContainerList.h"

struct _D3DVECTOR;
class SearchableObject;

class SearchMethod : public BaseObject {
public:
    SearchMethod();
    void PrepareGeometryOnSearchListContents(SearchableObject **list, int *count);
    void Render3DOnSearchListContents(SearchableObject **list, int count);

    // --- virtuals in retail slot order (after BaseObject's four) ---
    virtual ~SearchMethod();
    virtual unsigned int CalculatePointPlacementCode(_D3DVECTOR *point, unsigned int code) = 0;
    virtual unsigned int CalculatePointPlacementCode(float x, float y, float z, unsigned int code) = 0;
    virtual unsigned int CalculateBoxPlacementCode(_D3DVECTOR *min, _D3DVECTOR *max, unsigned int code) = 0;
    virtual void InsertObject(SearchableObject *object, unsigned int code, _D3DVECTOR *min, _D3DVECTOR *max) = 0;
    virtual void UpdateExtents(SearchableObject *object, unsigned int code, _D3DVECTOR *min, _D3DVECTOR *max);
    virtual void RemoveObject(SearchableObject *object, unsigned int code) = 0;
    virtual int OpenSearch(_D3DVECTOR *center, _D3DVECTOR *extent, _D3DVECTOR *outMin, _D3DVECTOR *outMax) = 0;
    virtual int OpenSearch(_D3DVECTOR *center, _D3DVECTOR *extent, int *count, SearchableObject ***list,
                           SearchableObject **buffer) = 0;
    virtual SearchableObject *NextSearchObject() = 0;
    virtual void RewindSearch() = 0;
    virtual void CloseSearch() = 0;
    virtual void PerformPrepareGeometry();
    virtual void PerformRender3D();

    unsigned int m_type;                    // +0x10, search type (FindSearchMethod's key)
    unsigned short m_searchId;              // +0x14
    unsigned char m_searchOpen : 1;         // +0x16 bit 0
    ContainerList<GameObject *> m_objects;  // +0x18, objects collected by PrepareGeometry
    unsigned char m_soultreeObjectType;     // +0x2C, type ID of "SoultreeObject"
    unsigned char m_searchableType;         // +0x2D, type ID of "SearchableGameObject"
};

#endif
