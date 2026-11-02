// SearchNode: a cell of the search tree (a GameObject). 0x50 bytes (`new`
// in Scene loading); data +0x30..+0x4B. Names follow its own methods.
// +0x30 is the portal/neighbour list AddNeighbor grows (some copies call it
// m_neighbors); +0x48 is never named consistently (m_index/m_zone).
#ifndef SEARCHNODE_SEARCHNODE_H
#define SEARCHNODE_SEARCHNODE_H

#include "GameObject/GameObject.h"

class Portal;
class ScreenQuad2D;
class SearchMethod;
class SearchNode;

// One malloc'd link per neighbour node (AddNeighbor): the node and the portal
// that leads to it.
struct PortalLink {
    PortalLink *m_next;                 // +0x0
    SearchNode *m_node;                 // +0x4
    Portal *m_portal;                   // +0x8
};

class SearchNode : public GameObject {
public:
    SearchNode(int enabled);
    void AddNeighbor(SearchNode *node, Portal *portal);
    SearchMethod *FindSearchMethod(unsigned int type);
    int PrepareGeometryThroughPortal(ScreenQuad2D &view);
    int SetDynamicSearchMethod(SearchMethod *method);
    int SetStaticSearchMethod(SearchMethod *method);

    // --- virtuals in retail slot order ---
    virtual ~SearchNode();
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual int RenderPost3D();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);

    static ScreenQuad2D WholeScreen;
    // retail keeps this 2-byte static in .sdata ($gp-relative)
    static unsigned short CurrentVisTimeStamp __attribute__((section(".sdata")));
    static int DebugDrawPortals;

    PortalLink *m_portals;              // +0x30
    SearchMethod *m_staticMethod;       // +0x34
    SearchMethod *m_dynamicMethod;      // +0x38
    unsigned short m_visTimeStamp;      // +0x3C
    int m_visible;                      // +0x40
    int m_drawEnvironment;              // +0x44
    int m_x48;                          // +0x48
};

#endif
