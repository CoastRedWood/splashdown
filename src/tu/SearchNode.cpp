// SearchNode.cpp  @ 0x002FC730 (text 0x2FC730-0x2FCF90)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's SearchNode.cpp as one translation unit: SearchNode's methods,
// a TU without an initializer in ParticleEmitter.cpp's tu_map row (its
// RTTI follows SearchMethod.cpp's). The C-mode ctor, AddNeighbor and
// FindSearchMethod are members again. The local views' slots are
// SearchMethod.h's PerformPrepareGeometry/PerformRender3D/CloseSearch and
// GameObject's Render3D (Portal's "DebugDraw").



#include "GameObject/GameObject.h"
#include "SearchMethod/SearchMethod.h"
#include "SearchNode/SearchNode.h"
#include "Portal/Portal.h"
#include "ScreenPoint2D/ScreenPoint2D.h"
#include "ScreenQuad2D/ScreenQuad2D.h"
#include "SearchList/SearchList.h"
#include "ControlInterface/ControlTypes.h"

extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);

int KEYDOWN_HANDLER_TEST(unsigned int key, DeviceType device, ProcessNode *node, unsigned int flags);

// The environment's node, drawn after every visible node (PrepareGeometry, Render3D).
SearchNode *LpEnvironmentSearchNode;
int SearchNode::DebugDrawPortals;
unsigned short SearchNode::CurrentVisTimeStamp;

// ---- src/SearchNode/ctor.cpp

SearchNode::SearchNode(int enabled) : GameObject(enabled)
{
    m_portals = 0;
    m_staticMethod = 0;
    m_dynamicMethod = 0;
    m_visible = 0;
    m_drawEnvironment = m_x48 = 0;
}


// ---- src/SearchNode/dtor.cpp

SearchNode::~SearchNode()
{
    PortalLink *link = m_portals;
    while (link) {
        PortalLink *next = link->m_next;
        free(link);
        link = next;
    }
    if (m_staticMethod)
        m_staticMethod->Release();
    if (m_dynamicMethod)
        m_dynamicMethod->Release();
}


// ---- src/SearchNode/KeyDownHandler.cpp

int SearchNode::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (KEYDOWN_HANDLER_TEST(0x13, (DeviceType)0, node, 0x80)) {
        DebugDrawPortals = !DebugDrawPortals;
        return 1;
    }
    return GameObject::KeyDownHandler(node, key);
}


// ---- src/SearchNode/AddNeighbor.cpp

void SearchNode::AddNeighbor(SearchNode *node, Portal *portal)
{
    PortalLink *link = (PortalLink *)malloc(sizeof(PortalLink));
    link->m_next = m_portals;
    link->m_node = node;
    link->m_portal = portal;
    m_portals = link;
}


// ---- src/SearchNode/SetStaticSearchMethod.cpp

int SearchNode::SetStaticSearchMethod(SearchMethod *method)
{
    if (m_staticMethod)
        m_staticMethod->Release();
    method->m_type = 1;
    m_staticMethod = method;
    m_staticMethod->AddRef();
    return 0;
}


// ---- src/SearchNode/SetDynamicSearchMethod.cpp

int SearchNode::SetDynamicSearchMethod(SearchMethod *method)
{
    if (m_dynamicMethod)
        m_dynamicMethod->Release();
    method->m_type = 2;
    m_dynamicMethod = method;
    m_dynamicMethod->AddRef();
    return 0;
}


// ---- src/SearchNode/FindSearchMethod.cpp

SearchMethod *SearchNode::FindSearchMethod(unsigned int type)
{
    if (type & 1)
        return m_staticMethod;
    if (type & 2)
        return m_dynamicMethod;
    return 0;
}


// ---- src/SearchNode/PrepareGeometry.cpp

// Prepares geometry for this node and, through each visible portal, the nodes
// beyond it (once per visibility pass).
int SearchNode::PrepareGeometry()
{
    PortalLink *link = m_portals;
    ScreenQuad2D quad;
    ScreenQuad2D clipped;
    CurrentVisTimeStamp++;
    m_visible = 1;
    if (m_visTimeStamp == CurrentVisTimeStamp)
        return 0;
    m_visTimeStamp = CurrentVisTimeStamp;
    for (; link; link = link->m_next) {
        if (link->m_portal->ProjectToScreen(quad)) {
            if (ScreenQuad2D::MinMaxIntersect(Portal::WholeScreen, quad, clipped)) {
                if (DebugDrawPortals)
                    link->m_portal->Render3D();
                link->m_node->PrepareGeometryThroughPortal(clipped);
            } else {
                link->m_node->m_visible = 0;
            }
        } else {
            link->m_node->m_visible = 0;
        }
    }
    if (m_staticMethod)
        m_staticMethod->PerformPrepareGeometry();
    if (m_dynamicMethod && m_dynamicMethod != m_staticMethod)
        m_dynamicMethod->PerformPrepareGeometry();
    if (m_drawEnvironment && LpEnvironmentSearchNode && !LpEnvironmentSearchNode->m_visible) {
        LpEnvironmentSearchNode->m_staticMethod->PerformPrepareGeometry();
        LpEnvironmentSearchNode->m_visible = 1;
    }
    return 1;
}


// ---- src/SearchNode/PrepareGeometryThroughPortal.cpp

// As PrepareGeometry, but seen through a portal: clip each further portal
// against the one we came through, and do not start a new visibility pass.
int SearchNode::PrepareGeometryThroughPortal(ScreenQuad2D &view)
{
    PortalLink *link = m_portals;
    ScreenQuad2D quad;
    ScreenQuad2D clipped;
    m_visible = 1;
    if (m_visTimeStamp == CurrentVisTimeStamp)
        return 0;
    m_visTimeStamp = CurrentVisTimeStamp;
    for (; link; link = link->m_next) {
        if (link->m_portal->ProjectToScreen(quad)) {
            if (ScreenQuad2D::MinMaxIntersect(view, quad, clipped)) {
                if (DebugDrawPortals)
                    link->m_portal->Render3D();
                link->m_node->PrepareGeometryThroughPortal(clipped);
            } else {
                link->m_node->m_visible = 0;
            }
        } else {
            link->m_node->m_visible = 0;
        }
    }
    if (m_staticMethod)
        m_staticMethod->PerformPrepareGeometry();
    if (m_dynamicMethod && m_dynamicMethod != m_staticMethod)
        m_dynamicMethod->PerformPrepareGeometry();
    if (m_drawEnvironment && LpEnvironmentSearchNode && !LpEnvironmentSearchNode->m_visible) {
        LpEnvironmentSearchNode->m_staticMethod->PerformPrepareGeometry();
        LpEnvironmentSearchNode->m_visible = 1;
    }
    return 1;
}


// ---- src/SearchNode/Render3D.cpp

int SearchNode::Render3D()
{
    PortalLink *link = m_portals;
    m_visTimeStamp--;
    for (; link; link = link->m_next) {
        SearchNode *node = link->m_node;
        if (node->m_visible && CurrentVisTimeStamp == node->m_visTimeStamp)
            node->Render3D();
    }
    if (m_staticMethod)
        m_staticMethod->PerformRender3D();
    if (m_dynamicMethod && m_dynamicMethod != m_staticMethod)
        m_dynamicMethod->PerformRender3D();
    if (LpEnvironmentSearchNode && LpEnvironmentSearchNode->m_visible) {
        LpEnvironmentSearchNode->m_staticMethod->PerformRender3D();
        LpEnvironmentSearchNode->m_visible = 0;
    }
    return 1;
}


// ---- src/SearchNode/RenderPost3D.cpp

int SearchNode::RenderPost3D()
{
    PortalLink *link = m_portals;
    m_visible = 0;
    for (; link; link = link->m_next) {
        SearchNode *node = link->m_node;
        if (node->m_visible)
            node->RenderPost3D();
    }
    if (m_staticMethod)
        m_staticMethod->CloseSearch();
    // Retail closes the static method again here instead of the dynamic one.
    if (m_dynamicMethod && m_dynamicMethod != m_staticMethod)
        m_staticMethod->CloseSearch();
    return 1;
}
