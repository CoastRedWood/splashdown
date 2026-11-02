// CheckpointSpacial: one training checkpoint's trigger box (0x50 bytes;
// JetskiTraining.cpp's CheckpointTrigger[20] has a 0x50 stride). Filled by
// JetskiTraining::LoadCheckpoints from training\checkpts.pb.
#ifndef CHECKPOINTSPACIAL_CHECKPOINTSPACIAL_H
#define CHECKPOINTSPACIAL_CHECKPOINTSPACIAL_H

#include "d3dtypes.h"

class CheckpointSpacial {
public:
    CheckpointSpacial();

    _D3DVECTOR m_position;              // +0x00
    _D3DVECTOR m_box;                   // +0x10
    _D3DVECTOR m_look;                  // +0x20
    _D3DVECTOR m_up;                    // +0x30
    float m_radius;                     // +0x40
    char pad44[0x50 - 0x44];
};

extern CheckpointSpacial CheckpointTrigger[20];

#endif
