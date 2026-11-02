// TrackData_Tag: one node of a track segment (centre point, border points,
// length to the next node), 0x60 bytes (ReadTrackFromDisk callocs 0x60). TrackType's copies call it
// TrackPiece_Tag in places; retail symbols name it TrackData_Tag.
#ifndef TRACKDATA_TAG_TRACKDATA_TAG_H
#define TRACKDATA_TAG_TRACKDATA_TAG_H

#include "d3dtypes.h"

struct TrackData_Tag {
    int m_index;                        // +0x00, TrackType::m_numNodes when read
    unsigned int m_flags;               // +0x04, tested with & 2/4/8/0x10/0x20
    char pad08[0x10 - 0x08];
    _D3DVECTOR m_position;              // +0x10, centre line
    _D3DVECTOR m_left;                  // +0x20, border points
    _D3DVECTOR m_right;                 // +0x30
    float m_length;                     // +0x40, to the next node
    float m_x44;                        // +0x44, -1 until read
    float m_x48;                        // +0x48
    TrackData_Tag *m_prev;              // +0x4C
    TrackData_Tag *m_next;              // +0x50
    char pad54[0x60 - 0x54];
};

#endif
