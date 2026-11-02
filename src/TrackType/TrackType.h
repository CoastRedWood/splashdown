// TrackType: the race track (segments of track nodes, gates, routing).
// A GameObject, 0xC0 bytes. Names come from the class's own methods.
#ifndef TRACKTYPE_TRACKTYPE_H
#define TRACKTYPE_TRACKTYPE_H

#include "d3dtypes.h"
#include "GameObject/GameObject.h"

class GateClass;
struct SegmentList_Tag;
struct TrackData_Tag;
struct TrackSegment_Tag;
#include "TrackPosition_Tag/TrackPosition_Tag.h"

// Free-list blocks of 12 bytes (next at +8), ReadTrackFromDisk's m_xAC.
struct TrackList12 {
    int m_x00;
    int m_x04;
    TrackList12 *m_next;                // +0x08
};

// Track-file tuning values (tags 19-22): plain floats, not a constructed
// _D3DVECTOR (the ctor stores them one by one after m_offset's inline ctor).
struct TrackTuning {
    float x, y, z, w;
};

class TrackType : public GameObject {
public:
    TrackType(int enabled);
    TrackType *Create(char *file, int x);
    int Reload(char *name);
    int ReadTrackFromDisk(char *file);
    int ReadTrackCleanUp(long file, TrackSegment_Tag **segments, int numSegments);
    int FreeTrackSegment(TrackSegment_Tag *segment);
    int FreeSegmentList(SegmentList_Tag **list, int freeShared);
    int PaintTrack();
    int NodeInSegment(TrackData_Tag *node, TrackSegment_Tag *segment);
    int GetTrackDirection(TrackData_Tag *data, _D3DVECTOR *direction);
    int WithinBorders(_D3DVECTOR point, TrackData_Tag *a, TrackData_Tag *b);
    int CoordinatesFromNode(TrackPosition_Tag *position, _D3DVECTOR *out);
    int PositionsInOrder(TrackPosition_Tag a, TrackPosition_Tag b);
    float DistanceBetween(TrackPosition_Tag from, TrackPosition_Tag to);
    int FindClosest(_D3DVECTOR point, TrackPosition_Tag *closest, float *distance);
    int FindClosestWithinSegment(TrackPosition_Tag *start, _D3DVECTOR point, TrackSegment_Tag *segment,
                                 TrackPosition_Tag *closest, float *distance);
    int FindClosestOnPath(SegmentList_Tag *path, TrackPosition_Tag *start, _D3DVECTOR point,
                          TrackPosition_Tag *closest, float *distance);
    int GetShortestPath(TrackPosition_Tag from, TrackPosition_Tag to, SegmentList_Tag **path, float *distance);
    int GetNextTrackPosition(TrackPosition_Tag from, TrackPosition_Tag *next, _D3DVECTOR point,
                             TrackPosition_Tag *closest, float distance, float x, float *random);
    // Free-list helpers, inline in retail; GetShortestPath.cpp and
    // GetNextTrackPosition.cpp define them (they need SegmentList_Tag).
    SegmentList_Tag *NewSegmentList();
    void ReleaseSegmentList(SegmentList_Tag *list);
    int OnTrack(_D3DVECTOR point, TrackPosition_Tag from, float a, float b, SegmentList_Tag *path,
                TrackPosition_Tag *closest, float *distance);

    // --- virtuals in retail slot order ---
    virtual ~TrackType();
    virtual int Tick(float dt);
    virtual int Render3D();

    int m_flags;                        // +0x30
    float m_behind;                     // +0x34
    float m_ahead;                      // +0x38
    int m_numSegments;                  // +0x3C
    int m_numNodes;                     // +0x40
    TrackSegment_Tag *m_start;          // +0x44
    TrackSegment_Tag **m_segments;      // +0x48
    float m_lapLength;                  // +0x4C
    GateClass *m_startGate;             // +0x50
    GateClass *m_endGate;               // +0x54
    GateClass *m_x58;                   // +0x58
    TrackPosition_Tag m_startTrackPosition; // +0x5C
    TrackPosition_Tag m_finish;         // +0x68
    TrackPosition_Tag m_x74;            // +0x74
    TrackTuning m_tuning;               // +0x80, track-file tags 19-22 (defaults 100, 0.9, 1.5, 0.5);
                                        //        VehicleAIClass::Create copies it, .z scales max RPM on difficulty 1
    _D3DVECTOR m_offset;                // +0x90, added to the file's node positions (.x, .z)
    float m_scale;                      // +0xA0, applied to the file's node positions
    SegmentList_Tag *m_segmentLists;    // +0xA4, calloc'd; m_freeSegmentLists starts here
    SegmentList_Tag *m_freeSegmentLists; // +0xA8
    TrackList12 *m_xAC;                 // +0xAC, calloc'd
    TrackList12 *m_xB0;                 // +0xB0, free list in m_xAC
    SegmentList_Tag *m_xB4;             // +0xB4, calloc'd
    SegmentList_Tag *m_xB8;             // +0xB8, free list in m_xB4
    int m_routeMode;                    // +0xBC
};

#endif
