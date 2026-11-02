// SegmentList_Tag: one link of a path through the track's segments
// (TrackType::GetShortestPath), 0x10 bytes.
#ifndef SEGMENTLIST_TAG_SEGMENTLIST_TAG_H
#define SEGMENTLIST_TAG_SEGMENTLIST_TAG_H

struct TrackSegment_Tag;

struct SegmentList_Tag {
    int m_x00;                          // +0x00
    TrackSegment_Tag *m_segment;        // +0x04
    int m_x08;                          // +0x08
    SegmentList_Tag *m_next;            // +0x0C
};

#endif
