// SearchPlacement: where a SearchableObject sits in the spatial search
// (its first node and search method, plus one extra SearchPlacementItem per
// portal-adjacent node it overlaps). 0x30 bytes; every placement shares the
// block allocator LpSearchPlacementAllocator (items are 0x18-byte blocks).
// Names follow SearchPlacement's own methods and retail's assert strings
// ("FirstPlacementItem.LpSearchNode").
#ifndef SEARCHPLACEMENT_SEARCHPLACEMENT_H
#define SEARCHPLACEMENT_SEARCHPLACEMENT_H

#include "d3dtypes.h"

class Portal;
class SearchableObject;
class SearchMethod;
class SearchNode;

struct SearchPlacementItem {
    SearchPlacementItem *LpNext;        // +0x00
    SearchNode *LpSearchNode;           // +0x04
    SearchMethod *LpSearchMethod;       // +0x08
    Portal *LpPortal;                   // +0x0C
    float Distance;                     // +0x10
    unsigned int PlacementCode;         // +0x14
};

class SearchPlacement {
public:
    SearchPlacement();
    ~SearchPlacement();
    void PerformInitialPlacement(SearchableObject *object, SearchNode *node, unsigned int type,
                                 _D3DVECTOR *min, _D3DVECTOR *max, float radius);
    void UpdatePlacement(SearchableObject *object, _D3DVECTOR *min, _D3DVECTOR *max, float radius);
    void UpdatePlacementInStandaloneNode(SearchableObject *object, _D3DVECTOR *min, _D3DVECTOR *max);
    void Remove(SearchableObject *object);

    SearchPlacementItem FirstPlacementItem;     // +0x00
    char pad18[0x20 - 0x18];
    _D3DVECTOR m_position;                      // +0x20, last placed position (the ctor's
                                                // 0, 0, 0, 1 is _D3DVECTOR's own)
};

#endif
