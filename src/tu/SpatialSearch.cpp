// SpatialSearch.cpp  @ 0x00300010 (text 0x300010-0x300670)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's SpatialSearch.cpp (its ASSERTs' name) as one translation unit, a
// TU without an initializer inside Portal.cpp's tu_map row: SpatialSearch.
// Its .rodata (0x4302B0-0x43032C) restarts the labels at @482 after
// Portal.cpp's RTTI.


#include "d3dtypes.h"
#include "SpatialSearch/SpatialSearch.h"
#include "SearchPlacement/SearchPlacement.h"
#include "SearchNode/SearchNode.h"
#include "SearchMethod/SearchMethod.h"
#include "soultree_assert_block.h"

extern SearchPlacement *LpGlobalSearchPlacement;

// ---- src/SpatialSearch/ctor.cpp


SpatialSearch::SpatialSearch()
{
    m_min = _D3DVECTOR(-3.40282347e+38f, -3.40282347e+38f, -3.40282347e+38f);
    m_max = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_flags = 0;
    m_node = 0;
    m_method = 0;
    m_boundsMin = 0;
    m_boundsMax = 0;
}


// ---- src/SpatialSearch/OpenSearch.cpp


int SpatialSearch::OpenSearch(SearchPlacement *placement, unsigned int flags, _D3DVECTOR *min, _D3DVECTOR *max,
                              _D3DVECTOR *boundsMin, _D3DVECTOR *boundsMax)
{
    if (!placement)
        placement = LpGlobalSearchPlacement;
    m_placement = placement;
    m_current = &m_placement->FirstPlacementItem;
    m_min = *min;
    m_max = *max;
    m_boundsMin = boundsMin;
    m_boundsMax = boundsMax;
    m_flags = flags;
    m_node = m_current->LpSearchNode;
    m_method = 0;
    if (m_flags & 1)
        m_method = m_node->m_staticMethod;
    if (!m_method && (m_flags & 2))
        m_method = m_node->m_dynamicMethod;
#line 51 "SpatialSearch.cpp"
    ASSERT(m_method != 0, "OpenSearch: LpSearchMethod==NULL");
    return m_method->OpenSearch(&m_min, &m_max, m_boundsMin, m_boundsMax);
}


// ---- src/SpatialSearch/NextSearchObject.cpp


// Returns the next object of the current method; when it runs dry, moves on
// to the dynamic method of the node and then to the next placement.
SearchableObject *SpatialSearch::NextSearchObject()
{
#line 61 "SpatialSearch.cpp"
    ASSERT(m_method != 0, "NextSearchObject: LpSearchMethod==NULL");
    SearchableObject *obj = m_method->NextSearchObject();
    if (obj)
        return obj;
    if (m_method == m_node->m_staticMethod) {
        if ((m_flags & 2) && m_node->m_dynamicMethod != m_node->m_staticMethod) {
            m_method = m_node->m_dynamicMethod;
        } else {
            if (!m_current)
                return 0;
            m_current = m_current->LpNext;
            if (m_current) {
                m_node = m_current->LpSearchNode;
                if (m_flags & 1)
                    m_method = m_node->m_staticMethod;
                if (!m_method && (m_flags & 2))
                    m_method = m_node->m_dynamicMethod;
#line 88 "SpatialSearch.cpp"
                ASSERT(m_method != 0, "NextSearchObject: LpSearchMethod==NULL");
            } else {
                return 0;
            }
        }
    } else {
        if (!m_current)
            return 0;
        m_current = m_current->LpNext;
        if (m_current) {
            m_node = m_current->LpSearchNode;
            if (m_flags & 1)
                m_method = m_node->m_staticMethod;
            if (!m_method && (m_flags & 2))
                m_method = m_node->m_dynamicMethod;
#line 109 "SpatialSearch.cpp"
            ASSERT(m_method != 0, "NextSearchObject: LpSearchMethod==NULL");
        } else {
            return 0;
        }
    }
    if (m_boundsMin) {
        _D3DVECTOR min;
        _D3DVECTOR max;
        m_method->OpenSearch(&m_min, &m_max, &min, &max);
        if (min.x < m_boundsMin->x)
            m_boundsMin->x = min.x;
        if (min.y < m_boundsMin->y)
            m_boundsMin->y = min.y;
        if (min.z < m_boundsMin->z)
            m_boundsMin->z = min.z;
        if (max.x > m_boundsMax->x)
            m_boundsMax->x = max.x;
        if (max.y > m_boundsMax->y)
            m_boundsMax->y = max.y;
        if (max.z > m_boundsMax->z)
            m_boundsMax->z = max.z;
    } else {
        m_method->OpenSearch(&m_min, &m_max, 0, 0);
    }
    return m_method->NextSearchObject();
}


// ---- src/SpatialSearch/CloseSearch.cpp


void SpatialSearch::CloseSearch()
{
    if (m_node->m_staticMethod)
        m_node->m_staticMethod->CloseSearch();
    if (m_node->m_dynamicMethod && m_node->m_dynamicMethod != m_node->m_staticMethod)
        m_node->m_dynamicMethod->CloseSearch();
}

