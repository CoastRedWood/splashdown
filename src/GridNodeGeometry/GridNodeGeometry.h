// GridNodeGeometry: a DrawableGridNode's built mesh (DrawableGridNode +0x64),
// allocated with the node's GridNodeTextures (LoadNode). Not a retail name
// (no symbol mentions it): it is the name the DrawableGridNode files gave it.
// Layout merged from Griddraw.cpp's views.
#ifndef GRIDNODEGEOMETRY_H
#define GRIDNODEGEOMETRY_H

#include "AgeItem/AgeItem.h"

// One 4x4 child's share of the mesh (m_childHeights, 0xC bytes).
struct ChildHeights {
    short m_numVertices;                    // +0x0, running totals at the end of this child
    short m_numIndices;                     // +0x2
    float m_center;                         // +0x4
    float m_halfHeight;                     // +0x8
};

struct GridNodeGeometry {
    unsigned char m_vertexFlags[17 * 17];   // +0x000
    union {
        unsigned char m_flags;              // +0x121 (CompareDuplicates and CheckDeltaEvaluations test bit 0 in the byte)
        struct {
            unsigned char m_hasMesh : 1;    // +0x121 bit 0 (also: no child deltas)
            unsigned char m_changed : 1;    //        bit 1
            unsigned char m_lit : 1;        //        bit 2
            unsigned char m_as4x4 : 1;      //        bit 3
            unsigned char m_x121_4 : 1;     //        bit 4
            unsigned char m_x121_5 : 1;     //        bit 5
        } m_bits;
    };
    unsigned short m_x122;                  // +0x122
    unsigned short m_x124;                  // +0x124
    unsigned short m_x126;                  // +0x126
    unsigned short m_x128;                  // +0x128
    short m_numStrips;                      // +0x12A
    short m_numChildStrips;                 // +0x12C
    char pad12E[0x130 - 0x12E];
    int m_evaluatedFrame;                   // +0x130
    char *m_vertices;                       // +0x134, 0x20 bytes each
    void *m_indices;                        // +0x138
    short m_numVertices;                    // +0x13C
    short m_numIndices;                     // +0x13E
    short m_verticesSize;                   // +0x140
    short m_indicesSize;                    // +0x142
    AgeItem m_ageItem;                      // +0x144, in the Terrain's mesh cache (m_size != 0: aged)
    float m_x158;                           // +0x158
    short m_childVisible;                   // +0x15C, one bit per 4x4 child
    char pad15E[0x160 - 0x15E];
    ChildHeights *m_childHeights;           // +0x160, one per 4x4 child
};

// DrawableGridNode::LoadNode allocates a node's texture data, detail
// entries, geometry and child heights in one block: GridNodeTextures padded
// to 0x40, 16 DetailTextureEntry (0x2C), GridNodeGeometry (0x164), 16
// ChildHeights. On the PC the pointers make the first three larger.
#ifdef SPLASHDOWN_PC
#define GRID_NODE_HEAD ((sizeof(GridNodeTextures) + 15) & ~15)
#define GRID_NODE_DETAILS (16 * sizeof(DetailTextureEntry))
#define GRID_NODE_GEOMETRY ((sizeof(GridNodeGeometry) + 15) & ~15)
#define GRID_NODE_CHILD_HEIGHTS (16 * sizeof(ChildHeights))
#else
#define GRID_NODE_HEAD 0x40
#define GRID_NODE_DETAILS 0x2C0
#define GRID_NODE_GEOMETRY 0x164
#define GRID_NODE_CHILD_HEIGHTS 0xC0
#endif

#endif
