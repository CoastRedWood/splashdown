// SearchPlacement.cpp  @ 0x002FDDE0 (text 0x2FDDE0-0x2FEA10)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's SearchPlacement.cpp as one translation unit: SearchPlacement's
// methods, a TU without an initializer at the end of ParticleEmitter.cpp's
// tu_map row (after PSXMemoryCard.cpp). PortalLink is SearchNode.h's.



#include "BlockAllocator/BlockAllocator.h"
#include "SearchPlacement/SearchPlacement.h"
#include "GameObject/GameObject.h"
#include "SearchMethod/SearchMethod.h"
#include "soultree_assert_block.h"
#include "d3dtypes.h"
#include "Portal/Portal.h"
#include "SearchNode/SearchNode.h"

// The block allocator every placement's extra items come from (made by the
// first placement, released by the last), the placement count, and the
// placement for objects outside every node (SpatialSearch, SceneManager).
BlockAllocator *LpSearchPlacementAllocator;
int SearchPlacementCount;
SearchPlacement *LpGlobalSearchPlacement;
// ---- src/SearchPlacement/ctor.cpp


// m_position starts as _D3DVECTOR's (0, 0, 0, 1). All placements share one
// block allocator, made by the first.
SearchPlacement::SearchPlacement()
{
    if (!LpSearchPlacementAllocator)
        LpSearchPlacementAllocator = new BlockAllocator(sizeof(SearchPlacementItem), 0x1000);
    FirstPlacementItem.LpNext = 0;
    FirstPlacementItem.LpSearchNode = 0;
    FirstPlacementItem.LpSearchMethod = 0;
    FirstPlacementItem.LpPortal = 0;
    FirstPlacementItem.Distance = 0;
    FirstPlacementItem.PlacementCode = 0;
    SearchPlacementCount++;
}


// ---- src/SearchPlacement/dtor.cpp


// The last placement to go releases the shared allocator.
SearchPlacement::~SearchPlacement()
{
    SearchPlacementCount--;
    if (SearchPlacementCount == 0 && LpSearchPlacementAllocator) {
        LpSearchPlacementAllocator->Release();
        LpSearchPlacementAllocator = 0;
    }
}


// ---- src/SearchPlacement/PerformInitialPlacement.cpp


void SearchPlacement::PerformInitialPlacement(SearchableObject *object, SearchNode *node, unsigned int type,
                                              _D3DVECTOR *min, _D3DVECTOR *max, float radius)
{
#line 49 "SearchPlacement.cpp"
    ASSERT(FirstPlacementItem.LpSearchNode == 0, "PerformInitialPlacement should only be used to make the initial placement sherlock!");
    SearchMethod *method = node->FindSearchMethod(type);
#line 52 "SearchPlacement.cpp"
    ASSERT(method != 0, "PerformInitialPlacement: cannot find the appropriate search type");
    m_position = *min;
    unsigned int code = method->CalculateBoxPlacementCode(min, max, 0);
#line 60 "SearchPlacement.cpp"
    ASSERT(code != 0, "PerformInitialPlacement: placementCode not in this node");
    method->InsertObject(object, code, min, max);
    FirstPlacementItem.LpSearchNode = node;
    FirstPlacementItem.LpSearchMethod = method;
    FirstPlacementItem.PlacementCode = code;
    FirstPlacementItem.LpNext = 0;
    m_position = *min;
    // Also place the object in every node whose portal its sphere touches.
    SearchNode *other;
    PortalLink *link;
    for (link = FirstPlacementItem.LpSearchNode->m_portals; link; link = link->m_next) {
        float distance;
        if (link->m_portal->SphereIntersects(min, radius, &distance)) {
            other = link->m_node;
            method = other->FindSearchMethod(method->m_type);
            unsigned int otherCode = method->CalculateBoxPlacementCode(min, max, 0);
            if (otherCode) {
                method->InsertObject(object, otherCode, min, max);
                SearchPlacementItem *item = (SearchPlacementItem *)LpSearchPlacementAllocator->AllocBlock();
                item->PlacementCode = otherCode;
                item->LpSearchNode = other;
                item->LpSearchMethod = method;
                item->LpPortal = link->m_portal;
                item->Distance = distance;
                item->LpNext = FirstPlacementItem.LpNext;
                FirstPlacementItem.LpNext = item;
            }
        }
    }
}


// ---- src/SearchPlacement/UpdatePlacement.cpp
// UpdatePlacement__15SearchPlacementFP16SearchableObjectP10_D3DVECTORP10_D3DVECTORf  @ 0x002FE240
// Register notes (asserting function, so every variable is one graph node and the
// $s colouring follows spill cost / degree): `method`, `code` and `item` are declared
// separately in each arm of the portal test (one shared variable spanning both arms
// changes every degree), and the portal loop reuses `node` for the far-side node, as
// retail's register map shows (node and the far-side node share $s0).


// Retail's sqrt: sqrt.s into $f8, stored through a stack temporary.
static inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}


// Follows the object through any portal it crossed since the last update and
// refreshes its placement in every search method it overlaps.
void SearchPlacement::UpdatePlacement(SearchableObject *object, _D3DVECTOR *min, _D3DVECTOR *max, float radius)
{
    SearchNode *node;
    PortalLink *link;
    SearchNode *previous;
    node = FirstPlacementItem.LpSearchNode;
    if (!node)
        return;
    link = node->m_portals;
    previous = 0;
    if (!link) {
        UpdatePlacementInStandaloneNode(object, min, max);
    } else {
        previous = 0;
        while (link) {
            if (link->m_node != previous && link->m_portal->RayIntersects(&m_position, min, 0)) {
                previous = node;
                node = link->m_node;
                link = node->m_portals;
            } else {
                link = link->m_next;
            }
        }
        if (node != FirstPlacementItem.LpSearchNode) {
            SearchMethod *method = FirstPlacementItem.LpSearchMethod;
            method->RemoveObject(object, FirstPlacementItem.PlacementCode);
            SearchPlacementItem *item = FirstPlacementItem.LpNext;
            while (item) {
                item->LpSearchMethod->RemoveObject(object, item->PlacementCode);
                SearchPlacementItem *next = item->LpNext;
                LpSearchPlacementAllocator->FreeBlock(item);
                item = next;
            }
            FirstPlacementItem.LpNext = 0;
            FirstPlacementItem.LpSearchNode = node;
            method = FirstPlacementItem.LpSearchMethod = node->FindSearchMethod(method->m_type);
            FirstPlacementItem.PlacementCode = method->CalculatePointPlacementCode(min, 0);
            method->InsertObject(object, FirstPlacementItem.PlacementCode, min, max);
            for (link = FirstPlacementItem.LpSearchNode->m_portals; link; link = link->m_next) {
                float distance;
                if (link->m_portal->SphereIntersects(min, radius, &distance)) {
                    node = link->m_node;
                    method = node->FindSearchMethod(method->m_type);
                    unsigned int code = method->CalculateBoxPlacementCode(min, max, 0);
                    if (code) {
                        method->InsertObject(object, code, min, max);
                        SearchPlacementItem *added = (SearchPlacementItem *)LpSearchPlacementAllocator->AllocBlock();
                        added->PlacementCode = code;
                        added->LpSearchNode = node;
                        added->LpSearchMethod = method;
                        added->LpPortal = link->m_portal;
                        added->Distance = distance;
                        added->LpNext = FirstPlacementItem.LpNext;
                        FirstPlacementItem.LpNext = added;
                    }
                }
            }
        } else {
            _D3DVECTOR delta = *min - m_position;
            float moved = Sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
            SearchMethod *method = FirstPlacementItem.LpSearchMethod;
            unsigned int code = method->CalculateBoxPlacementCode(min, max, FirstPlacementItem.PlacementCode);
#line 246 "SearchPlacement.cpp"
            ASSERT(code != 0, "Haven't crossed a portal, but PlacementCode is out of range");
            if (code != FirstPlacementItem.PlacementCode) {
                method->RemoveObject(object, FirstPlacementItem.PlacementCode);
                method->InsertObject(object, code, min, max);
                FirstPlacementItem.PlacementCode = code;
            } else {
                method->UpdateExtents(object, code, min, max);
            }
            SearchPlacementItem *previousItem = &FirstPlacementItem;
            SearchPlacementItem *item = FirstPlacementItem.LpNext;
            while (item) {
                item->Distance += moved;
                method = item->LpSearchMethod;
                code = method->CalculateBoxPlacementCode(min, max, item->PlacementCode);
#line 266 "SearchPlacement.cpp"
                ASSERT(code != 0, "Haven't crossed a portal, but PlacementCode is out of range");
                if (code != item->PlacementCode) {
                    method->RemoveObject(object, item->PlacementCode);
                    method->InsertObject(object, code, min, max);
                    item->PlacementCode = code;
                } else {
                    method->UpdateExtents(object, code, min, max);
                }
                if (item->Distance > radius) {
                    float distance;
                    if (item->LpPortal->SphereIntersects(min, radius, &distance)) {
                        previousItem = item;
                        item->Distance = distance;
                        item = item->LpNext;
                    } else {
                        item->LpSearchMethod->RemoveObject(object, item->PlacementCode);
                        SearchPlacementItem *next = item->LpNext;
                        previousItem->LpNext = next;
                        LpSearchPlacementAllocator->FreeBlock(item);
                        item = next;
                    }
                } else {
                    previousItem = item;
                    item = item->LpNext;
                }
            }
        }
    }
    m_position = *min;
}


// ---- src/SearchPlacement/UpdatePlacementInStandaloneNode.cpp

// Move the object within its single search method after its extents changed.
void SearchPlacement::UpdatePlacementInStandaloneNode(SearchableObject *object, _D3DVECTOR *min, _D3DVECTOR *max)
{
    SearchMethod *method = FirstPlacementItem.LpSearchMethod;
    unsigned int code = method->CalculateBoxPlacementCode(min, max, FirstPlacementItem.PlacementCode);
    if (code != FirstPlacementItem.PlacementCode) {
        if (code == 0) {
            method->RemoveObject(object, FirstPlacementItem.PlacementCode);
        } else {
            method->RemoveObject(object, FirstPlacementItem.PlacementCode);
            method->InsertObject(object, code, min, max);
        }
    } else {
        FirstPlacementItem.LpSearchMethod->UpdateExtents(object, code, min, max);
    }
    FirstPlacementItem.PlacementCode = code;
}


// ---- src/SearchPlacement/Remove.cpp


// Removes the object from every node it was placed in, freeing the extra
// placement items.
void SearchPlacement::Remove(SearchableObject *object)
{
#line 413 "SearchPlacement.cpp"
    ASSERT(FirstPlacementItem.LpSearchMethod != 0, "Remove: FirstPlacementItem.LpSearchMethod==NULL");
    FirstPlacementItem.LpSearchMethod->RemoveObject(object, FirstPlacementItem.PlacementCode);
    SearchPlacementItem *item = FirstPlacementItem.LpNext;
    while (item) {
        item->LpSearchMethod->RemoveObject(object, item->PlacementCode);
        SearchPlacementItem *next = item->LpNext;
        LpSearchPlacementAllocator->FreeBlock(item);
        item = next;
    }
}

