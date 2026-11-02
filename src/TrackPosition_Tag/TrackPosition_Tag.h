// TrackPosition_Tag: a position along the track: a segment, the node in
// it, and the fraction of the way to the next node; 0xC bytes.
#ifndef TRACKPOSITION_TAG_TRACKPOSITION_TAG_H
#define TRACKPOSITION_TAG_TRACKPOSITION_TAG_H

struct TrackSegment_Tag;
struct TrackData_Tag;

struct TrackPosition_Tag {
    TrackSegment_Tag *m_segment;        // +0x00
    TrackData_Tag *m_data;              // +0x04
    float m_t;                          // +0x08
};

#endif
