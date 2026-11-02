// The AI state space: a k-d tree (AIStateSpaceType_Tag nodes, each holding
// a list of AIStateDataType_Tag states) shared by every VehicleAIClass
// through the AIStateSpace global. Layouts from FreeStateSpace and
// GetNewState.
#ifndef VEHICLEAICLASS_AISTATESPACE_H
#define VEHICLEAICLASS_AISTATESPACE_H

#include "d3dtypes.h"

struct AIStateDataType_Tag {
    char pad0[0x10];
    _D3DVECTOR m_position;              // +0x10
    _D3DVECTOR m_direction;             // +0x20
    float m_height;                     // +0x30
    char pad34[0x50 - 0x34];
    AIStateDataType_Tag *next;          // +0x50
};

struct AIStateSpaceType_Tag {
    int axis;                           // +0x00, 0/1/2 = x/y/z
    float split;                        // +0x04
    float radius;                       // +0x08
    AIStateDataType_Tag *states;        // +0x0C
    AIStateSpaceType_Tag *left;         // +0x10
    AIStateSpaceType_Tag *right;        // +0x14
};

// Node of the explicit stack used to walk the tree.
struct StateStack {
    AIStateSpaceType_Tag *state;
    StateStack *next;
};

extern AIStateSpaceType_Tag *AIStateSpace;

#endif
