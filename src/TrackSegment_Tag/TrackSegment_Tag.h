// TrackSegment_Tag: a run of track nodes between branch points, linked to
// the segments that follow it; 0x24 bytes. Names come from TrackType's
// methods.
#ifndef TRACKSEGMENT_TAG_TRACKSEGMENT_TAG_H
#define TRACKSEGMENT_TAG_TRACKSEGMENT_TAG_H

#include "ps2_port.h"

struct TrackData_Tag;

struct TrackSegment_Tag {
    signed char m_flags;                // +0x00 (bit 0: shared, 2: visited, 3: steer, 5: random, 6: shortcut)
    float m_length;                     // +0x04
    float m_x08;                        // +0x08
    TrackData_Tag *m_nodes;             // +0x0C, first node
    TrackData_Tag *m_lastNode;          // +0x10
    int m_numLinks;                     // +0x14
    float m_distance;                   // +0x18
    TrackSegment_Tag *m_previous;       // +0x1C
    union {
        TrackSegment_Tag **m_links;     // +0x20, the m_numLinks following segments
        PS2_PTR_INT *m_linkIndices;     // +0x20, the same as segment indices while
                                        // ReadTrackFromDisk loads the file
    };
};

#endif
