// SpatialSearch: a search over the placements of the spatial search tree,
// node by node through each node's static and dynamic SearchMethods
// (0x3C bytes; no vtable).
#ifndef SPATIALSEARCH_SPATIALSEARCH_H
#define SPATIALSEARCH_SPATIALSEARCH_H

#include "d3dtypes.h"

class SearchableObject;
class SearchMethod;
class SearchNode;
class SearchPlacement;
struct SearchPlacementItem;

class SpatialSearch {
public:
    SpatialSearch();
    int OpenSearch(SearchPlacement *placement, unsigned int flags, _D3DVECTOR *min, _D3DVECTOR *max,
                   _D3DVECTOR *boundsMin, _D3DVECTOR *boundsMax);
    SearchableObject *NextSearchObject();
    void CloseSearch();

    _D3DVECTOR m_min;                   // +0x00
    _D3DVECTOR m_max;                   // +0x10
    unsigned int m_flags;               // +0x20: 1 static, 2 dynamic objects
    SearchNode *m_node;                 // +0x24
    SearchMethod *m_method;             // +0x28
    SearchPlacement *m_placement;       // +0x2C
    SearchPlacementItem *m_current;     // +0x30
    _D3DVECTOR *m_boundsMin;            // +0x34 (grown to each method's bounds; may be 0)
    _D3DVECTOR *m_boundsMax;            // +0x38
};

#endif
