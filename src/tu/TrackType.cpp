// TrackType.cpp  @ 0x002B8490 (text 0x2B8490-0x2BC740)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// TrackType as one translation unit without a static initializer. Its
// .rodata is "rb" (@1021) and the TrackType RTTI with its own
// BaseObject/GameObject copies, between PSXTextureMapManager.cpp's RTTI and
// DataAccess's strings. The file name is a guess.



#include "GameObject/GameObject.h"
#include "TrackType/TrackType.h"
#include "GateClass/GateClass.h"
#include "DataAccess/DataAccess.h"
#include "d3dtypes.h"
#include "TrackData_Tag/TrackData_Tag.h"
#include "TrackSegment_Tag/TrackSegment_Tag.h"
#include "TrackPosition_Tag/TrackPosition_Tag.h"
#include "SegmentList_Tag/SegmentList_Tag.h"

// ---- src/TrackType/ctor.cpp

TrackType::TrackType(int enabled)
    : GameObject(enabled)
{
    m_numSegments = 0;
    m_segments = 0;
    m_start = 0;
    m_behind = 200.0f;
    m_ahead = 200.0f;
    m_tuning.x = 100.0f;
    m_tuning.y = 0.9f;
    m_tuning.z = 1.5f;
    m_tuning.w = 0.5f;
    m_flags = 0;
    m_startGate = 0;
    m_endGate = 0;
    m_x58 = 0;
    m_segmentLists = 0;
    m_xAC = 0;
    m_xB4 = 0;
    m_routeMode = 3;
    m_numNodes = 0;
}


// ---- src/TrackType/Create.cpp


TrackType *TrackType::Create(char *file, int x)
{
    m_routeMode = x;
    m_numSegments = 0;
    m_segments = 0;
    m_start = 0;
    if (!ReadTrackFromDisk(file)) {
        Release();
        return 0;
    }
    GameObject::Create();
    return this;
}


// ---- src/TrackType/dtor.cpp

extern "C" void free(void *p);

struct TrackSegment_Tag;


// Only its GameObject base matters here.

TrackType::~TrackType()
{
    if (m_startGate)
        m_startGate->Release();
    if (m_endGate)
        m_endGate->Release();
    if (m_x58)
        m_x58->Release();
    int i;
    TrackSegment_Tag **segment = m_segments;
    for (i = 0; i < m_numSegments; segment++, i++) {
        if (*segment)
            FreeTrackSegment(*segment);
    }
    m_start = 0;
    free(m_segments);
    free(m_segmentLists);
    free(m_xAC);
    free(m_xB4);
}


// ---- src/TrackType/Reload.cpp

extern "C" void free(void *p);

struct TrackSegment_Tag;


// Only its GameObject base matters here.

// Frees the loaded track, restores the constructor's defaults and reads
// the named track again.
int TrackType::Reload(char *name)
{
    if (m_startGate)
        m_startGate->Release();
    if (m_endGate)
        m_endGate->Release();
    if (m_x58)
        m_x58->Release();
    int i;
    TrackSegment_Tag **segment = m_segments;
    for (i = 0; i < m_numSegments; segment++, i++) {
        if (*segment)
            FreeTrackSegment(*segment);
    }
    m_start = 0;
    free(m_segments);
    free(m_segmentLists);
    free(m_xAC);
    free(m_xB4);
    m_numSegments = 0;
    m_segments = 0;
    m_start = 0;
    m_behind = 200.0f;
    m_ahead = 200.0f;
    m_tuning.x = 100.0f;
    m_tuning.y = 0.9f;
    m_tuning.z = 1.5f;
    m_tuning.w = 0.5f;
    m_flags = 1;
    m_startGate = 0;
    m_endGate = 0;
    m_x58 = 0;
    m_segmentLists = 0;
    m_xAC = 0;
    m_xB4 = 0;
    return ReadTrackFromDisk(name) != 0;
}


// ---- src/TrackType/Tick.cpp


int TrackType::Tick(float dt)
{
    return GameObject::Tick(dt);
}



// ---- src/TrackType/ReadTrackCleanUp.cpp
extern "C" void free(void *p);

struct TrackSegment_Tag;

extern DataAccess *LpDataAccess;


int TrackType::ReadTrackCleanUp(long file, TrackSegment_Tag **segments, int numSegments)
{
    if (file == -1 || !segments)
        return 0;
    int i;
    TrackSegment_Tag **p = segments;
    for (i = 0; i < numSegments; i++, p++) {
        if (*p)
            FreeTrackSegment(*p);
    }
    free(segments);
    LpDataAccess->FClose(file);
    return 1;
}


// ---- src/TrackType/ReadTrackFromDisk.cpp

extern "C" void *calloc(unsigned int count, unsigned int size);

static inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

extern DataAccess *LpDataAccess;






#define READ_FAIL                                                           \
    {                                                                       \
        ReadTrackCleanUp(file, m_segments, m_numSegments);                  \
        return 0;                                                           \
    }

// Reads a .trk file: a stream of one-byte tags, each followed by its data.
// The Sqrt asm turns the global optimizer off for the whole function.
int TrackType::ReadTrackFromDisk(char *name)
{
    _D3DVECTOR diff;
    TrackPosition_Tag start;
    TrackPosition_Tag end;
    int segIndex;
    int nextIndex;
    int nextSeg;
    int numNext;
    int flag;                           // read as 4 bytes, used as a char
    float length;
    char tag;
    TrackData_Tag **link = 0;
    TrackSegment_Tag **cur;
    long file = LpDataAccess->FOpen(name, "rb");
    if (file == -1)
        return 0;
    m_numNodes = 0;
    LpDataAccess->FRead(file, &tag, 1, 1);
    cur = 0;
    while (!LpDataAccess->FEof(file)) {
        if (tag == 1) {
            LpDataAccess->FRead(file, &m_numSegments, 4, 1);
            if (m_numSegments <= 0) {
                LpDataAccess->FClose(file);
                return 0;
            }
            if (!(m_segments = (TrackSegment_Tag **)calloc(m_numSegments, sizeof(TrackSegment_Tag *)))) {
                LpDataAccess->FClose(file);
                return 0;
            }
            if (!(m_segmentLists = (SegmentList_Tag *)calloc(m_numSegments * 10, sizeof(SegmentList_Tag)))) {
                LpDataAccess->FClose(file);
                return 0;
            }
            m_freeSegmentLists = m_segmentLists;
            SegmentList_Tag *list = m_segmentLists;
            int i;
            for (i = 0; i < m_numSegments * 10 - 1; i++) {
                list->m_next = list + 1;
                list++;
            }
            if (!(m_xAC = (TrackList12 *)calloc(m_numSegments * 10, sizeof(TrackList12)))) {
                LpDataAccess->FClose(file);
                return 0;
            }
            m_xB0 = m_xAC;
            TrackList12 *list12 = m_xAC;
            for (i = 0; i < m_numSegments * 10 - 1; i++) {
                list12->m_next = list12 + 1;
                list12++;
            }
            if (!(m_xB4 = (SegmentList_Tag *)calloc(m_numSegments * 10, sizeof(SegmentList_Tag)))) {
                LpDataAccess->FClose(file);
                return 0;
            }
            m_xB8 = m_xB4;
            list = m_xB4;
            for (i = 0; i < m_numSegments * 10 - 1; i++) {
                list->m_next = list + 1;
                list++;
            }
        } else if (tag == 2) {
            LpDataAccess->FRead(file, &m_offset.x, 4, 1);
        } else if (tag == 3) {
            LpDataAccess->FRead(file, &m_offset.z, 4, 1);
        } else if (tag == 4) {
            LpDataAccess->FRead(file, &m_scale, 4, 1);
        } else if (tag == 5) {
            LpDataAccess->FRead(file, &segIndex, 4, 1);
            if (segIndex < 0 || segIndex >= m_numSegments)
                READ_FAIL
            cur = &m_segments[segIndex];
            if (!(*cur = (TrackSegment_Tag *)calloc(1, sizeof(TrackSegment_Tag))))
                READ_FAIL
            link = &(*cur)->m_nodes;
        } else if (tag == 6) {
            if (!*cur)
                READ_FAIL
            LpDataAccess->FRead(file, &flag, 4, 1);
            (*cur)->m_flags = (char)flag | 0x41;
        } else if (tag == 7) {
            if (!*cur)
                READ_FAIL
            LpDataAccess->FRead(file, &length, 4, 1);
        } else if (tag == 24) {
            if (!*cur)
                READ_FAIL
            LpDataAccess->FRead(file, &(*cur)->m_x08, 4, 1);
        } else if (tag == 8) {
            if (!*cur || !link)
                READ_FAIL
            TrackData_Tag *node = (TrackData_Tag *)calloc(1, sizeof(TrackData_Tag));
            if (!node)
                READ_FAIL
            node->m_x44 = -1.0f;
            node->m_prev = (*cur)->m_lastNode;
            *link = node;
            link = &node->m_next;
            (*cur)->m_lastNode = node;
            node->m_index = m_numNodes;
            m_numNodes++;
        } else if (tag == 23) {
            if (!*cur || !(*cur)->m_lastNode)
                READ_FAIL
            LpDataAccess->FRead(file, &(*cur)->m_lastNode->m_flags, 4, 1);
        } else if (tag == 9) {
            if (!*cur || !(*cur)->m_lastNode)
                READ_FAIL
            LpDataAccess->FRead(file, &(*cur)->m_lastNode->m_position.x, 4, 1);
            LpDataAccess->FRead(file, &(*cur)->m_lastNode->m_position.y, 4, 1);
            LpDataAccess->FRead(file, &(*cur)->m_lastNode->m_position.z, 4, 1);
            (*cur)->m_lastNode->m_position.x = m_offset.x + (*cur)->m_lastNode->m_position.x * m_scale;
            (*cur)->m_lastNode->m_position.y = (*cur)->m_lastNode->m_position.y * m_scale;
            (*cur)->m_lastNode->m_position.z = m_offset.z + (*cur)->m_lastNode->m_position.z * m_scale;
            if ((*cur)->m_lastNode->m_prev) {
                diff = (*cur)->m_lastNode->m_position - (*cur)->m_lastNode->m_prev->m_position;
                float dx = diff.x;
                float dz = diff.z;
                length = Sqrt(dx * dx + dz * dz);
                (*cur)->m_lastNode->m_prev->m_length = length;
                (*cur)->m_length += length;
            }
        } else if (tag == 10) {
            if (!*cur || !(*cur)->m_lastNode)
                READ_FAIL
            LpDataAccess->FRead(file, &(*cur)->m_lastNode->m_left.x, 4, 1);
            LpDataAccess->FRead(file, &(*cur)->m_lastNode->m_left.y, 4, 1);
            LpDataAccess->FRead(file, &(*cur)->m_lastNode->m_left.z, 4, 1);
            (*cur)->m_lastNode->m_left.x = m_offset.x + (*cur)->m_lastNode->m_left.x * m_scale;
            (*cur)->m_lastNode->m_left.y = (*cur)->m_lastNode->m_left.y * m_scale;
            (*cur)->m_lastNode->m_left.z = m_offset.z + (*cur)->m_lastNode->m_left.z * m_scale;
        } else if (tag == 11) {
            if (!*cur || !(*cur)->m_lastNode)
                READ_FAIL
            LpDataAccess->FRead(file, &(*cur)->m_lastNode->m_right.x, 4, 1);
            LpDataAccess->FRead(file, &(*cur)->m_lastNode->m_right.y, 4, 1);
            LpDataAccess->FRead(file, &(*cur)->m_lastNode->m_right.z, 4, 1);
            (*cur)->m_lastNode->m_right.x = m_offset.x + (*cur)->m_lastNode->m_right.x * m_scale;
            (*cur)->m_lastNode->m_right.y = (*cur)->m_lastNode->m_right.y * m_scale;
            (*cur)->m_lastNode->m_right.z = m_offset.z + (*cur)->m_lastNode->m_right.z * m_scale;
        } else if (tag == 12) {
            LpDataAccess->FRead(file, &length, 4, 1);
        } else if (tag == 18) {
            if (!*cur || !(*cur)->m_lastNode)
                READ_FAIL
            LpDataAccess->FRead(file, &(*cur)->m_lastNode->m_x44, 4, 1);
        } else if (tag == 26) {
            if (!*cur || !(*cur)->m_lastNode)
                READ_FAIL
            LpDataAccess->FRead(file, &(*cur)->m_lastNode->m_x48, 4, 1);
        } else if (tag == 13) {
            if (!*cur)
                READ_FAIL
            LpDataAccess->FRead(file, &numNext, 4, 1);
            if (numNext < 0)
                READ_FAIL
            (*cur)->m_numLinks = numNext;
            if (!((*cur)->m_linkIndices = (PS2_PTR_INT *)calloc(numNext, PS2_PTR_SIZE)))
                READ_FAIL
        } else if (tag == 14) {
            if (!*cur || !(*cur)->m_linkIndices)
                READ_FAIL
            LpDataAccess->FRead(file, &nextIndex, 4, 1);
            LpDataAccess->FRead(file, &nextSeg, 4, 1);
            if (nextIndex < 0 || nextIndex > numNext || nextSeg < 0 || nextSeg > m_numSegments)
                READ_FAIL
            (*cur)->m_linkIndices[nextIndex] = nextSeg;
        } else if (tag == 15) {
            m_startGate = new GateClass(1);
            LpDataAccess->FRead(file, &m_startGate->m_center.x, 4, 1);
            LpDataAccess->FRead(file, &m_startGate->m_center.y, 4, 1);
            LpDataAccess->FRead(file, &m_startGate->m_center.z, 4, 1);
            LpDataAccess->FRead(file, &m_startGate->m_normal.x, 4, 1);
            LpDataAccess->FRead(file, &m_startGate->m_normal.y, 4, 1);
            LpDataAccess->FRead(file, &m_startGate->m_normal.z, 4, 1);
            LpDataAccess->FRead(file, &m_startGate->m_halfSize.x, 4, 1);
            LpDataAccess->FRead(file, &m_startGate->m_halfSize.y, 4, 1);
            LpDataAccess->FRead(file, &m_startGate->m_halfSize.z, 4, 1);
            LpDataAccess->FRead(file, &m_startGate->m_x70, 4, 1);
            m_startGate->m_halfSize *= m_scale;
            m_startGate->m_x70 = m_startGate->m_x70 * m_scale;
            m_startGate->m_center.x = m_offset.x + m_startGate->m_center.x * m_scale;
            m_startGate->m_center.y = m_startGate->m_center.y * m_scale;
            m_startGate->m_center.z = m_offset.z + m_startGate->m_center.z * m_scale;
            m_startGate->m_flags = 3;
        } else if (tag == 16) {
            m_endGate = new GateClass(1);
            LpDataAccess->FRead(file, &m_endGate->m_center.x, 4, 1);
            LpDataAccess->FRead(file, &m_endGate->m_center.y, 4, 1);
            LpDataAccess->FRead(file, &m_endGate->m_center.z, 4, 1);
            LpDataAccess->FRead(file, &m_endGate->m_normal.x, 4, 1);
            LpDataAccess->FRead(file, &m_endGate->m_normal.y, 4, 1);
            LpDataAccess->FRead(file, &m_endGate->m_normal.z, 4, 1);
            LpDataAccess->FRead(file, &m_endGate->m_halfSize.x, 4, 1);
            LpDataAccess->FRead(file, &m_endGate->m_halfSize.y, 4, 1);
            LpDataAccess->FRead(file, &m_endGate->m_halfSize.z, 4, 1);
            LpDataAccess->FRead(file, &m_endGate->m_x70, 4, 1);
            m_endGate->m_halfSize *= m_scale;
            m_endGate->m_x70 = m_endGate->m_x70 * m_scale;
            m_endGate->m_center.x = m_offset.x + m_endGate->m_center.x * m_scale;
            m_endGate->m_center.y = m_endGate->m_center.y * m_scale;
            m_endGate->m_center.z = m_offset.z + m_endGate->m_center.z * m_scale;
            m_endGate->m_flags = 3;
        } else if (tag == 25) {
            m_x58 = new GateClass(1);
            LpDataAccess->FRead(file, &m_x58->m_center.x, 4, 1);
            LpDataAccess->FRead(file, &m_x58->m_center.y, 4, 1);
            LpDataAccess->FRead(file, &m_x58->m_center.z, 4, 1);
            LpDataAccess->FRead(file, &m_x58->m_normal.x, 4, 1);
            LpDataAccess->FRead(file, &m_x58->m_normal.y, 4, 1);
            LpDataAccess->FRead(file, &m_x58->m_normal.z, 4, 1);
            LpDataAccess->FRead(file, &m_x58->m_halfSize.x, 4, 1);
            LpDataAccess->FRead(file, &m_x58->m_halfSize.y, 4, 1);
            LpDataAccess->FRead(file, &m_x58->m_halfSize.z, 4, 1);
            LpDataAccess->FRead(file, &m_x58->m_x70, 4, 1);
            m_x58->m_halfSize *= m_scale;
            m_x58->m_x70 = m_x58->m_x70 * m_scale;
            m_x58->m_center.x = m_offset.x + m_x58->m_center.x * m_scale;
            m_x58->m_center.y = m_x58->m_center.y * m_scale;
            m_x58->m_center.z = m_offset.z + m_x58->m_center.z * m_scale;
            m_x58->m_flags = 3;
        } else if (tag == 19) {
            LpDataAccess->FRead(file, &m_tuning.x, 4, 1);
        } else if (tag == 20) {
            LpDataAccess->FRead(file, &m_tuning.y, 4, 1);
        } else if (tag == 21) {
            LpDataAccess->FRead(file, &m_tuning.z, 4, 1);
        } else if (tag == 22) {
            LpDataAccess->FRead(file, &m_tuning.w, 4, 1);
        }
        LpDataAccess->FRead(file, &tag, 1, 1);
    }

    // Turn each segment's next-segment indices into pointers.
    for (cur = m_segments, segIndex = 0; segIndex < m_numSegments; segIndex++, cur++) {
        if (*cur && (*cur)->m_linkIndices) {
            for (nextIndex = 0; nextIndex < (*cur)->m_numLinks; nextIndex++) {
                PS2_PTR_INT *next = &(*cur)->m_linkIndices[nextIndex];
                if (*next >= 0 && *next < m_numSegments)
                    *next = (PS2_PTR_INT)m_segments[*next];
            }
        }
    }
    m_start = m_segments[0];
    if (m_startGate) {
        m_startGate->m_height = 150.0f;
        FindClosest(m_startGate->m_center, &m_startTrackPosition, 0);
    }
    if (m_endGate) {
        m_endGate->m_height = 150.0f;
        FindClosest(m_endGate->m_center, &m_finish, 0);
    }
    if (m_x58) {
        m_x58->m_height = 150.0f;
        FindClosest(m_x58->m_center, &m_x74, 0);
    }
    start.m_segment = m_start;
    start.m_data = m_start->m_nodes;
    start.m_t = 0.0001f;
    end.m_segment = start.m_segment;
    end.m_data = start.m_data;
    end.m_t = 0.0f;
    GetShortestPath(start, end, 0, &m_lapLength);
    LpDataAccess->FClose(file);
    return 1;
}


// ---- src/TrackType/FreeTrackSegment.cpp
extern "C" void free(void *p);




int TrackType::FreeTrackSegment(TrackSegment_Tag *segment)
{
    if (!segment)
        return 0;
    TrackData_Tag *piece;
    while ((piece = segment->m_nodes) != 0) {
        segment->m_nodes = piece->m_next;
        free(piece);
    }
    if (segment->m_links)
        free(segment->m_links);
    free(segment);
    return 1;
}


// ---- src/TrackType/FindClosest.cpp
// The Sqrt asm turns the global optimizer off. GPR side: the pushes use the
// TrackType::NewSegmentList/ReleaseSegmentList inlines (as GetShortestPath),
// the pop releases a block-scoped copy, and i/count/nextSegments are declared
// in that order. FP side (as FindClosestWithinSegment): t, ex, ez declared
// first; multiply-adds are fused by a pcode peephole walking each block
// backwards, so the projection's sum goes back into z (one of its own
// multiplicands: unfused), and in the else branch z before x / dx before dz
// leaves dx's sub unfused (dz's suba/msub keeps the accumulator live across it).








inline SegmentList_Tag *TrackType::NewSegmentList()
{
    SegmentList_Tag *list = 0;
    if (m_freeSegmentLists) {
        list = m_freeSegmentLists;
        m_freeSegmentLists = m_freeSegmentLists->m_next;
        list->m_next = 0;
    }
    return list;
}

inline void TrackType::ReleaseSegmentList(SegmentList_Tag *list)
{
    if (list) {
        list->m_next = m_freeSegmentLists;
        m_freeSegmentLists = list;
    }
}

static inline SegmentList_Tag *AllocNode(TrackType *track)
{
    SegmentList_Tag *free = track->m_freeSegmentLists;
    SegmentList_Tag *node = 0;
    if (free) {
        node = free;
        track->m_freeSegmentLists = free->m_next;
        free->m_next = 0;
    }
    return node;
}

static inline void FreeNode(TrackType *track, SegmentList_Tag *node)
{
    if (node) {
        node->m_next = track->m_freeSegmentLists;
        track->m_freeSegmentLists = node;
    }
}

// Nearest point on the whole track: flood-fills the segment graph from the
// start segment and tests every node span, then clears the visited marks.
int TrackType::FindClosest(_D3DVECTOR point, TrackPosition_Tag *closest, float *distance)
{
    float bestDistance = -1.0f;
    float t;
    float ex;
    float ez;
    float dist;
    int i;
    int count;
    TrackSegment_Tag **nextSegments;
    SegmentList_Tag *node;
    SegmentList_Tag *stack;
    if (!closest)
        return 0;
    if (!m_start)
        return 0;
    stack = 0;
    node = NewSegmentList();
    if (!node)
        return 0;
    node->m_segment = m_start;
    stack = node;
    while (stack) {
        TrackSegment_Tag *segment = stack->m_segment;
        if (segment->m_flags & 4) {
            SegmentList_Tag *done = stack;
            stack = stack->m_next;
            ReleaseSegmentList(done);
            continue;
        }
        segment->m_flags |= 4;
        TrackData_Tag *data = stack->m_segment->m_nodes;
        if (data && !data->m_next) {
            float dz = point.z - data->m_position.z;
            float dx = point.x - data->m_position.x;
            float dist = dz * dz + dx * dx;
            if (dist < bestDistance || bestDistance < 0.0f) {
                closest->m_segment = stack->m_segment;
                bestDistance = dist;
                closest->m_data = data;
                closest->m_t = 0;
            }
        }
        for (; data && data->m_next; data = data->m_next) {
            TrackData_Tag *next = data->m_next;
            ex = next->m_position.x - data->m_position.x;
            ez = next->m_position.z - data->m_position.z;
                        if (ex != 0.0f || ez != 0.0f) {
                float x = point.x - data->m_position.x;
                float z = point.z - data->m_position.z;
                z = ex * x + ez * z;    // unfused: z is also a multiplicand
                t = z / (ez * ez + ex * ex);
            } else {
                t = 0.0f;
            }
            if (t > 1.0f)
                t = 1.0f;
            else if (t < 0.0f)
                t = 0.0f;
                        if (t == 1.0f) {
                float dz = point.z - next->m_position.z;
                float dx = point.x - next->m_position.x;
                dist = dz * dz + dx * dx;
            } else if (t == 0.0f) {
                float dz = point.z - data->m_position.z;
                float dx = point.x - data->m_position.x;
                dist = dz * dz + dx * dx;
            } else {
                float z = point.z - data->m_position.z;
                float x = point.x - data->m_position.x;
                float dx = x - ex * t;
                float dz = z - ez * t;
                dist = dz * dz + dx * dx;
            }
            if (dist < bestDistance || bestDistance < 0.0f) {
                closest->m_segment = stack->m_segment;
                bestDistance = dist;
                closest->m_data = data;
                closest->m_t = t;
            }
        }
        count = stack->m_segment->m_numLinks;
        nextSegments = stack->m_segment->m_links;
        for (i = 0; i < count; i++) {
            if (nextSegments[i]->m_flags & 4)
                continue;
            node = NewSegmentList();
            if (!node)
                return 0;
            node->m_segment = nextSegments[i];
            node->m_next = stack;
            stack = node;
        }
    }
    TrackSegment_Tag **segments = m_segments;
    for (int k = 0; k < m_numSegments; k++) {
        (*segments)->m_flags &= ~4;
        segments++;
    }
    if (distance) {
        if (bestDistance > 0.0f)
            *distance = Sqrt(bestDistance);
        else if (bestDistance == 0.0f)
            *distance = 0;
    }
    return 1;
}


// ---- src/TrackType/FindClosestWithinSegment.cpp
// The Sqrt asm turns the global optimizer off. Multiply-adds are fused by a
// pcode peephole that walks each block backwards. In the else branch z is
// computed before x and dz after dx, so dz's suba/msub (fused first) keeps
// the accumulator live across dx's sub, which stays unfused as in retail; in
// the projection the sum goes back into z, one of its own multiplicands,
// which blocks fusion of the numerator.







// Nearest point of a segment (searched within m_ahead/m_behind of start).
int TrackType::FindClosestWithinSegment(TrackPosition_Tag *start, _D3DVECTOR point, TrackSegment_Tag *segment,
                                        TrackPosition_Tag *closest, float *distance)
{
    float bestDistance = -1.0f;
    TrackPosition_Tag position;

    if (!segment || !closest)
        return 0;
    closest->m_segment = segment;
    position.m_segment = segment;
    position.m_data = segment->m_nodes;
    position.m_t = 0;
    TrackData_Tag *node = segment->m_nodes;
    if (node && !node->m_next) {
        closest->m_data = node;
        closest->m_t = 0;
        if (distance) {
            float dx = point.x - node->m_position.x;
            float dz = point.z - node->m_position.z;
            *distance = Sqrt(dx * dx + dz * dz);
        }
        return 1;
    }
    float t;
    float ex;
    float ez;
    float dist;
    float d = DistanceBetween(position, *start);
    for (; node && node->m_next; node = node->m_next) {
        if (d < m_ahead || !(d <= -m_behind)) {
            TrackData_Tag *next = node->m_next;
            ex = next->m_position.x - node->m_position.x;
            ez = next->m_position.z - node->m_position.z;
            if (ex != 0.0f || ez != 0.0f) {
                float x = point.x - node->m_position.x;
                float z = point.z - node->m_position.z;
                z = ex * x + ez * z;    // unfused: z is also a multiplicand
                t = z / (ez * ez + ex * ex);
            } else {
                t = 0.0f;
            }
            if (t > 1.0f)
                t = 1.0f;
            else if (t < 0.0f)
                t = 0.0f;
            if (t == 1.0f) {
                float dz = point.z - next->m_position.z;
                float dx = point.x - next->m_position.x;
                dist = dz * dz + dx * dx;
            } else if (t == 0.0f) {
                float dz = point.z - node->m_position.z;
                float dx = point.x - node->m_position.x;
                dist = dz * dz + dx * dx;
            } else {
                float z = point.z - node->m_position.z;
                float x = point.x - node->m_position.x;
                float dx = x - ex * t;
                float dz = z - ez * t;
                dist = dz * dz + dx * dx;
            }
            if (dist < bestDistance || bestDistance < 0.0f) {
                position.m_data = node;
                position.m_t = t;
                bestDistance = dist;
                closest->m_data = node;
                closest->m_t = t;
            }
        }
        d -= node->m_length;
    }
    if (distance) {
        if (bestDistance > 0.0f)
            *distance = Sqrt(bestDistance);
        else if (bestDistance == 0.0f)
            *distance = 0;
    }
    return 1;
}


// ---- src/TrackType/FindClosestOnPath.cpp

struct TrackData_Tag;
struct TrackSegment_Tag;

// A circular list of track segments.


int TrackType::FindClosestOnPath(SegmentList_Tag *path, TrackPosition_Tag *start, _D3DVECTOR point,
                                 TrackPosition_Tag *closest, float *distance)
{
    TrackPosition_Tag best;
    float bestDistance = -1.0f;
    if (!path)
        return 0;
    SegmentList_Tag *list = path;
    while (list) {
        TrackPosition_Tag position;
        float d;
        if (!FindClosestWithinSegment(start, point, list->m_segment, &position, &d))
            return 0;
        if (bestDistance == -1.0f || d < bestDistance) {
            bestDistance = d;
            best = position;
        }
        if (list->m_next == path)
            list = 0;
        else
            list = list->m_next;
    }
    if (closest)
        *closest = best;
    if (distance)
        *distance = bestDistance;
    return 1;
}


// ---- src/TrackType/WithinBorders.cpp



// Even-odd crossing test of one polygon edge (e0, e1).
#define EDGE(e0, e1)                                                                            \
    if ((e0.x <= point.x || e1.x <= point.x) &&                                                \
        ((!(point.z < e0.z) && point.z < e1.z) || (!(point.z < e1.z) && point.z < e0.z))) {   \
        bool left = ((e1.x - e0.x) * (point.z - e0.z) <= (point.x - e0.x) * (e1.z - e0.z)); \
        bool above = point.z >= e0.z;                                                          \
        if (left == above)                                                                     \
            inside = 1 - inside;                                                               \
    }

// Whether point lies inside the track polygon between nodes a and b.
int TrackType::WithinBorders(_D3DVECTOR point, TrackData_Tag *a, TrackData_Tag *b)
{
    int inside = 0;
    EDGE(a->m_left, b->m_left)
    EDGE(a->m_left, a->m_position)
    EDGE(a->m_right, a->m_position)
    EDGE(a->m_right, b->m_right)
    EDGE(b->m_left, b->m_position)
    EDGE(b->m_right, b->m_position)
    return inside;
}


// ---- src/TrackType/NodeInSegment.cpp



int TrackType::NodeInSegment(TrackData_Tag *node, TrackSegment_Tag *segment)
{
    for (TrackData_Tag *p = segment->m_nodes; p; p = p->m_next) {
        if (p == node)
            return 1;
    }
    return 0;
}


// ---- src/TrackType/OnTrack.cpp
// OnTrack__9TrackTypeF10_D3DVECTOR17TrackPosition_TagffP15SegmentList_TagP17TrackPosition_TagPf  @ 0x002BACA0






// A track position near the point, in the m_xB4/m_xB8 free list.
struct TrackCandidate {
    TrackPosition_Tag m_position;       // +0x00
    TrackCandidate *m_next;             // +0x0C
};

// A segment linking into the start segment, in the m_xAC/m_xB0 free list.
struct TrackLink {
    float m_distance;                   // +0x00
    TrackSegment_Tag *m_segment;        // +0x04
    TrackLink *m_next;                  // +0x08
};



// The free-list pop as written in the link loop (the head copied into a
// local first); it colours differently from NewSegmentList.
static inline SegmentList_Tag *NewList2(TrackType *t)
{
    SegmentList_Tag *list = 0;
    SegmentList_Tag *free = t->m_freeSegmentLists;
    if (free) {
        list = free;
        t->m_freeSegmentLists = free->m_next;
        free->m_next = 0;
    }
    return list;
}
static inline TrackCandidate *NewCandidate(TrackType *t)
{
    TrackCandidate *c = 0;
    if (t->m_xB8) {
        c = (TrackCandidate *)t->m_xB8;
        t->m_xB8 = (SegmentList_Tag *)((TrackCandidate *)t->m_xB8)->m_next;
        c->m_next = 0;
    }
    return c;
}

static inline void ReleaseCandidate(TrackType *t, TrackCandidate *c)
{
    if (c) {
        c->m_next = (TrackCandidate *)t->m_xB8;
        t->m_xB8 = (SegmentList_Tag *)c;
    }
}

// Takes `candidate` as the closest position when it beats the best so far.
#define UPDATE_CLOSEST(d1, d2)                                                  \
    if (closest) {                                                              \
        if ((m_flags & 2) && (d1 > bestT || -d2 > bestT)) {                     \
            *closest = candidate->m_position;                                   \
            if (d1 > bestT)                                                     \
                bestT = d1;                                                     \
            else                                                                \
                bestT = -d2;                                                    \
        } else {                                                                \
            CoordinatesFromNode(&candidate->m_position, &coords);               \
            float dz = point.z - coords.z;                                      \
            float dx = point.x - coords.x;                                      \
            float dsq = dx * dx + dz * dz;                                      \
            if (dsq < bestDsq) {                                                \
                bestDsq = dsq;                                                  \
                bestT = d1;                                                     \
                *closest = candidate->m_position;                               \
            }                                                                   \
        }                                                                       \
    }

// Whether `point` lies on the track near `from` (within `behind`/`ahead`
// along it, or on `path`): returns 1 or 2 and sets the closest position and
// its distance. Candidates are the nodes whose borders contain the point.
// Locals are declared in the order that reproduces retail's registers.
int TrackType::OnTrack(_D3DVECTOR point, TrackPosition_Tag from, float behind, float ahead, SegmentList_Tag *path,
                       TrackPosition_Tag *closest, float *distance)
{
    TrackLink *link;
    TrackLink *freeLink;
    int count3;
    float d1;
    TrackSegment_Tag **links3;
    int count2;
    TrackSegment_Tag **links2;
    SegmentList_Tag *first;
    SegmentList_Tag *p;
    float t;
    float dx;
    float dz;
    TrackData_Tag *data;
    SegmentList_Tag *current;
    TrackSegment_Tag *segment;
    float bestT;
    TrackData_Tag *next;
    TrackCandidate *candidate;
    TrackSegment_Tag *s;
    TrackPosition_Tag start;
    float bestDsq;
    float d2;
    int i;
    int count;
    TrackSegment_Tag **segs;
    int j2;
    int j3;
    SegmentList_Tag *queue;
    TrackLink *links;
    TrackCandidate *cand2;
    TrackLink *link2;
    TrackCandidate *candidates;
    int found;
    SegmentList_Tag *node;
    bestDsq = 1000000.0f;
    queue = 0;
    candidates = 0;
    found = 0;
    links = 0;
    _D3DVECTOR coords;
    first = NewSegmentList();
    if (!first)
        return 0;
    if (closest) {
        *closest = from;
        bestT = -ahead;
    }
    first->m_segment = m_start;
    queue = first;
    while ((current = queue) != 0) {
        segment = current->m_segment;
        if (segment->m_flags & 4) {
            queue = current->m_next;
            ReleaseSegmentList(current);
        } else {
            segment->m_flags |= 4;
            for (data = segment->m_nodes; data && (next = data->m_next) != 0; data = data->m_next) {
                if (WithinBorders(point, data, next)) {
                    t = 0.0f;
                    dx = next->m_position.x - data->m_position.x;
                    dz = next->m_position.z - data->m_position.z;
                    if (dx != 0.0f || dz != 0.0f)
                        t = (dx * (point.x - data->m_position.x) + dz * (point.z - data->m_position.z)) /
                            (dx * dx + dz * dz);
                    else
                        t = 0.0f;
                    if (t > 1.0f)
                        t = 1.0f;
                    else if (t < 0.0f)
                        t = 0.0f;
                    candidate = NewCandidate(this);
                    if (!candidate)
                        return 0;
                    candidate->m_position.m_segment = segment;
                    candidate->m_position.m_data = data;
                    candidate->m_position.m_t = t;
                    candidate->m_next = candidates;
                    candidates = candidate;
                }
            }
            TrackSegment_Tag **segLinks;
            count = segment->m_numLinks;
            segLinks = segment->m_links;
            for (i = 0; i < count; i++) {
                if (!((*segLinks)->m_flags & 4)) {
                    if (!(node = NewList2(this))) {
                        FreeSegmentList(&queue, 0);
                        return 0;
                    }
                    node->m_segment = *segLinks;
                    node->m_next = queue;
                    queue = node;
                }
                segLinks++;
            }
        }
    }
    segs = m_segments;
    for (i = 0; i < m_numSegments; i++)
        segs[i]->m_flags &= ~4;

    for (candidate = candidates; candidate; candidate = candidate->m_next) {
        if (behind < 0.0f || ahead < 0.0f) {
            if (!found) {
                if (closest)
                    *closest = candidate->m_position;
            }
            found = 1;
        }
        if (found && !closest)
            continue;
        d1 = DistanceBetween(from, candidate->m_position);
        d2 = DistanceBetween(candidate->m_position, from);
        if ((!(d1 < 0.0f) && d1 <= behind) || (!(d2 < 0.0f) && d2 <= ahead)) {
            found = 1;
            UPDATE_CLOSEST(d1, d2)
        }
        for (p = path; !found && p; p = p->m_next) {
            s = p->m_segment;
            start.m_segment = s;
            start.m_data = s->m_nodes;
            start.m_t = 0.0f;
            d1 = DistanceBetween(start, candidate->m_position);
            d2 = DistanceBetween(start, from);
            if (NodeInSegment(candidate->m_position.m_data, s)) {
                if (!NodeInSegment(from.m_data, s) || (!(d1 < 0.0f) && !(d2 < 0.0f) && d1 < d2)) {
                    found = 1;
                    UPDATE_CLOSEST(d1, d2)
                }
            }
            if ((!(d1 < 0.0f) && d1 <= behind) || (!(d2 < 0.0f) && d2 <= ahead)) {
                found = 1;
                UPDATE_CLOSEST(d1, d2)
            }
        }
    }

    if (!found) {
        if (candidates) {
            segs = m_segments;
            for (i = 0; i < m_numSegments; i++) {
                count2 = segs[i]->m_numLinks;
                links2 = segs[i]->m_links;
                for (j2 = 0; j2 < count2; j2++) {
                    if (*links2 == from.m_segment) {
                        link = 0;
                        freeLink = (TrackLink *)m_xB0;
                        if (freeLink) {
                            link = freeLink;
                            m_xB0 = (TrackList12 *)freeLink->m_next;
                            freeLink->m_next = 0;
                        }
                        if (link) {
                            link->m_distance = d1;     // retail stores whatever d1 last held
                            link->m_segment = segs[i];
                            link->m_next = links;
                            links = link;
                        }
                    }
                    links2++;
                }
            }
        }
        for (cand2 = candidates; cand2; cand2 = cand2->m_next) {
            if (cand2->m_position.m_segment == from.m_segment)
                continue;
            for (link2 = links; !found && link2; link2 = link2->m_next) {
                count3 = link2->m_segment->m_numLinks;
                links3 = link2->m_segment->m_links;
                for (j3 = 0; j3 < count3; j3++) {
                    if (*links3 == cand2->m_position.m_segment) {
                        found = 2;
                        start.m_segment = link2->m_segment;
                        start.m_data = link2->m_segment->m_lastNode;
                        start.m_t = 0.0f;
                        d1 = DistanceBetween(start, cand2->m_position);
                        d2 = DistanceBetween(start, from);
                        bestT = d1 - d2;
                        *closest = cand2->m_position;
                    }
                    links3++;
                }
            }
        }
    }

    while (candidates) {
        candidate = candidates;
        candidates = candidate->m_next;
        ReleaseCandidate(this, candidate);
    }
    if (found && distance)
        *distance = bestT;
    while (links) {
        link = links;
        links = links->m_next;
        link->m_next = (TrackLink *)m_xB0;
        m_xB0 = (TrackList12 *)link;
    }
    return found;
}


// ---- src/TrackType/FreeSegmentList.cpp



int TrackType::FreeSegmentList(SegmentList_Tag **list, int freeShared)
{
    if (!list)
        return 0;
    SegmentList_Tag *node;
    while ((node = *list) != 0) {
        if (node->m_segment) {
            if (!(node->m_segment->m_flags & 1) || freeShared)
                FreeTrackSegment(node->m_segment);
        }
        *list = (*list)->m_next;
        if (node) {
            node->m_next = m_freeSegmentLists;
            m_freeSegmentLists = node;
        }
    }
    return 1;
}


// ---- src/TrackType/PositionsInOrder.cpp

struct TrackSegment_Tag;


int TrackType::PositionsInOrder(TrackPosition_Tag a, TrackPosition_Tag b)
{
    if (a.m_segment != b.m_segment)
        return 0;
    if (a.m_data == b.m_data) {
        if (a.m_t <= b.m_t)
            return 1;
        return 0;
    }
    for (TrackData_Tag *piece = a.m_data; piece; piece = piece->m_next) {
        if (piece == b.m_data)
            return 1;
    }
    return 0;
}


// ---- src/TrackType/GetShortestPath.cpp
// GetShortestPath__9TrackTypeF17TrackPosition_Tag17TrackPosition_TagPP15SegmentList_TagPf  @ 0x002BB890
// Register notes: the path-building loop's free-list pop uses the AllocNode shape
// (NewList2: the head copied into a local). count, links and i are each reused by both
// branches of the main loop, so their second ranges become compiler temporaries, created
// in the order of the variables' first definitions; retail's colouring of the unvisited
// branch (count $a3, links $a1, i $a2) needs count defined after links and i in the
// visited branch, hence `i = 1;` before `count = ...` there.






// The free-list pop as written at the path-building site (the head copied into a
// local first); it colours differently from NewSegmentList.



int TrackType::GetShortestPath(TrackPosition_Tag from, TrackPosition_Tag to, SegmentList_Tag **path, float *distance)
{
    int i;
    int count;
    TrackSegment_Tag **links;
    TrackSegment_Tag *segment;
    SegmentList_Tag *current;
    SegmentList_Tag *node;
    SegmentList_Tag **tail;
    SegmentList_Tag *queue = 0;
    if (from.m_segment == to.m_segment && PositionsInOrder(from, to)) {
        if (path) {
            if (!(*path = NewSegmentList()))
                return 0;
            (*path)->m_segment = to.m_segment;
        }
        if (distance)
            *distance = DistanceBetween(from, to);
        return 1;
    }
    TrackPosition_Tag end;
    end.m_segment = from.m_segment;
    end.m_data = from.m_segment->m_lastNode;
    end.m_t = 1.0f;
    float startDistance = DistanceBetween(from, end);
    end.m_segment = to.m_segment;
    end.m_data = to.m_segment->m_nodes;
    end.m_t = 0.0f;
    to.m_segment->m_distance = DistanceBetween(end, to);
    node = NewSegmentList();
    if (!node) {
        FreeSegmentList(&queue, 0);
        FreeSegmentList(path, 0);
        return 0;
    }
    node->m_segment = from.m_segment;
    node->m_next = queue;
    queue = node;
    while ((current = queue) != 0) {
        segment = current->m_segment;
        if (segment->m_flags & 4) {
            queue = current->m_next;
            links = segment->m_links;
            i = 1;     // before count: see the header note
            count = segment->m_numLinks;
            segment->m_distance = (*links)->m_distance;
            segment->m_previous = *links;
            links++;
            for (; i < count; i++) {
                if ((*links)->m_distance < segment->m_distance) {
                    segment->m_distance = (*links)->m_distance;
                    segment->m_previous = *links;
                }
                links++;
            }
            if (!queue) {
                segment->m_distance += startDistance;
                if (distance)
                    *distance = segment->m_distance;
                if (path) {
                    tail = path;
                    while (segment->m_previous != to.m_segment) {
                        if (!(node = NewList2(this))) {
                            FreeSegmentList(&queue, 0);
                            FreeSegmentList(path, 0);
                            return 0;
                        }
                        node->m_segment = segment;
                        *tail = node;
                        segment = segment->m_previous;
                        tail = &node->m_next;
                    }
                    if (!(node = NewSegmentList())) {
                        FreeSegmentList(&queue, 0);
                        FreeSegmentList(path, 0);
                        return 0;
                    }
                    node->m_segment = segment->m_previous;
                    *tail = node;
                }
            } else {
                segment->m_distance += segment->m_length;
            }
            ReleaseSegmentList(current);
        } else {
            segment->m_flags |= 4;
            links = segment->m_links;
            count = segment->m_numLinks;
            for (i = 0; i < count; i++) {
                if (!(links[i]->m_flags & 4) && links[i] != to.m_segment) {
                    if (!(node = NewSegmentList())) {
                        FreeSegmentList(&queue, 0);
                        FreeSegmentList(path, 0);
                        return 0;
                    }
                    node->m_segment = links[i];
                    node->m_next = queue;
                    queue = node;
                }
            }
        }
    }
    TrackSegment_Tag **segments = m_segments;
    for (i = 0; i < m_numSegments; i++)
        segments[i]->m_flags &= ~4;
    return 1;
}


// ---- src/TrackType/GetNextTrackPosition.cpp
// GetNextTrackPosition__9TrackTypeF17TrackPosition_TagP17TrackPosition_Tag10_D3DVECTORP17TrackPosition_TagffPf  @ 0x002BBD20
// The Fabs asm turns the global optimizer off. The random branch reads m_links into
// `links` before the `r > 9` wrap (retail loads it in that branch's delay slot); that
// fixed the size. Register notes: the declaration order sets the FP colours (best $f4,
// total $f3) and the link/chosen GPRs ($a3/$t2); `link` is function-scope for that.

#pragma fast_fptosi on





static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}




int TrackType::GetNextTrackPosition(TrackPosition_Tag from, TrackPosition_Tag *next, _D3DVECTOR point,
                                    TrackPosition_Tag *closest, float distance, float x, float *random)
{
    float d;
    float length;
    float total;
    float best;
    SegmentList_Tag **tail;
    TrackSegment_Tag *link;
    int i;
    int count;
    TrackSegment_Tag *chosen;
    TrackSegment_Tag **links;
    int r = 0;
    SegmentList_Tag *path = 0;
    if (!next || !closest)
        return 0;
    tail = &path;
    TrackPosition_Tag pos;
    pos.m_segment = from.m_segment;
    pos.m_data = from.m_segment->m_lastNode->m_prev;
    pos.m_t = 1.0f;
    length = DistanceBetween(from, pos);
    total = length;
    pos = from;
    SegmentList_Tag *node = NewSegmentList();
    if (!node) {
        FreeSegmentList(&path, 0);
        return 0;
    }
    node->m_segment = from.m_segment;
    *tail = node;
    tail = &node->m_next;
    while (total < distance) {
        TrackSegment_Tag *segment = pos.m_segment;
        if (segment->m_flags & 8) {
            best = -1.0f;
            count = segment->m_numLinks;
            links = segment->m_links;
            chosen = links[0];
            for (i = 0; i < count; i++) {
                d = Fabs((*links)->m_x08 - x);
                if (d < best || best == -1.0f) {
                    chosen = *links;
                    best = d;
                }
                links++;
            }
            pos.m_segment = chosen;
        } else if (segment->m_flags & 0x20) {
            int index = (float)segment->m_numLinks * random[r++];
            links = segment->m_links;
            if (r > 9)
                r = 0;
            pos.m_segment = links[index];
        } else {
            best = -1.0f;
            count = segment->m_numLinks;
            links = segment->m_links;
            chosen = links[0];
            for (i = 0; i < count; i++) {
                link = *links;
                d = link->m_length;
                if (m_routeMode == 3) {
                    if (d < best || best == -1.0f) {
                        chosen = *links;
                        best = d;
                    }
                } else if (m_routeMode == 2 && !(link->m_flags & 0x40)) {
                    if (d < best || best == -1.0f) {
                        chosen = *links;
                        best = d;
                    }
                } else if (m_routeMode == 1 || ((*links)->m_flags & 0x40)) {
                    if (d > best || best == -1.0f) {
                        chosen = *links;
                        best = d;
                    }
                }
                links++;
            }
            pos.m_segment = chosen;
        }
        pos.m_data = pos.m_segment->m_nodes;
        pos.m_t = 0.0f;
        length = pos.m_segment->m_length;
        node = NewSegmentList();
        if (!node) {
            FreeSegmentList(&path, 0);
            return 0;
        }
        total += length;
        node->m_segment = pos.m_segment;
        *tail = node;
        tail = &node->m_next;
    }
    total -= length;
    while (total < distance) {
        total += (1.0f - pos.m_t) * pos.m_data->m_length;
        if (total < distance) {
            pos.m_data = pos.m_data->m_next;
            pos.m_t = 0.0f;
            if (!pos.m_data) {
                FreeSegmentList(&path, 0);
                return 0;
            }
        }
    }
    next->m_segment = pos.m_segment;
    next->m_data = pos.m_data;
    next->m_t = (pos.m_data->m_length + (distance - total)) / pos.m_data->m_length;
    if (!FindClosestOnPath(path, &from, point, closest, 0)) {
        FreeSegmentList(&path, 0);
        return 0;
    }
    FreeSegmentList(&path, 0);
    return 1;
}
#pragma fast_fptosi reset


// ---- src/TrackType/DistanceBetween.cpp


struct SegmentList_Tag;


float TrackType::DistanceBetween(TrackPosition_Tag from, TrackPosition_Tag to)
{
    float distance = 0.0f;
    if (from.m_segment == to.m_segment) {
        if (from.m_data == to.m_data) {
            if (from.m_t <= to.m_t)
                return from.m_data->m_length * (to.m_t - from.m_t);
            GetShortestPath(from, to, 0, &distance);
            return distance;
        }
        TrackData_Tag *data = from.m_data;
        while (data && data != to.m_data) {
            distance += data->m_length;
            data = data->m_next;
        }
        if (!data) {
            GetShortestPath(from, to, 0, &distance);
            return distance;
        }
        distance += to.m_t * to.m_data->m_length - from.m_t * from.m_data->m_length;
        return distance;
    }
    GetShortestPath(from, to, 0, &distance);
    return distance;
}


// ---- src/TrackType/CoordinatesFromNode.cpp



int TrackType::CoordinatesFromNode(TrackPosition_Tag *position, _D3DVECTOR *out)
{
    if (!position)
        return 0;
    if (!position->m_data)
        return 0;
    if (!position->m_data->m_next) {
        if (position->m_t == 0.0f) {
            out->x = position->m_data->m_position.x;
            out->y = position->m_data->m_position.y;
            out->z = position->m_data->m_position.z;
        }
        return 0;
    }
    if (position->m_t < 0.0f)
        position->m_t = 0.0f;
    else if (position->m_t > 1.0f)
        position->m_t = 1.0f;
    TrackData_Tag *next = position->m_data->m_next;
    float dx = next->m_position.x - position->m_data->m_position.x;
    float dy = next->m_position.y - position->m_data->m_position.y;
    float dz = next->m_position.z - position->m_data->m_position.z;
    out->x = position->m_data->m_position.x + position->m_t * dx;
    out->y = position->m_data->m_position.y + position->m_t * dy;
    out->z = position->m_data->m_position.z + position->m_t * dz;
    return 1;
}


// ---- src/TrackType/GetTrackDirection.cpp


// VU0 normalize (inline asm, like retail's; it turns off the global optimizer).
static inline void Normalize(_D3DVECTOR *out, _D3DVECTOR *in)
{
    asm("lqc2 vf4, 0x0(%1); vmul.xyz vf5, vf4, vf4; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf4w; vwaitq; vmulq.xyz vf6, vf4, Q; sqc2 vf6, 0x0(%0)"
        : : "r"(out), "r"(in) : "memory");
}



int TrackType::GetTrackDirection(TrackData_Tag *data, _D3DVECTOR *direction)
{
    TrackData_Tag *next;
    if (!direction || !data || !(next = data->m_next))
        return 0;
    if (data->m_length <= 0.0f)
        return 0;
    _D3DVECTOR delta(next->m_position.x - data->m_position.x, next->m_position.y - data->m_position.y,
                     next->m_position.z - data->m_position.z);
    Normalize(direction, &delta);
    return 1;
}


// ---- src/TrackType/Render3D.cpp

int TrackType::Render3D()
{
    if (m_flags & 1)
        PaintTrack();
    return GameObject::Render3D();
}


// ---- src/TrackType/PaintTrack.cpp
// PaintTrack__9TrackTypeFv  @ 0x002BC590
// Register notes: retail keeps a byte offset into m_next ($a1) beside the counter i, both
// named locals, so the m_next walk is written with an explicit `off` (a plain m_next[i]
// becomes a compiler temporary numbered below segment and colours wrong). The child pop
// uses AllocNode2, the same pop with its two locals declared the other way round: the
// order of those inline temporaries puts free/child in retail's $a3/$t0.

struct TrackSegment_Tag;






// The child pop: the same free-list pop with its locals declared the other way round
// (the order of the two inline temporaries sets retail's $a3/$t0 here).
static inline SegmentList_Tag *AllocNode2(TrackType *track)
{
    SegmentList_Tag *node = 0;
    SegmentList_Tag *free = track->m_freeSegmentLists;
    if (free) {
        node = free;
        track->m_freeSegmentLists = free->m_next;
        free->m_next = 0;
    }
    return node;
}

// Flood-fills the segment graph from the start segment (depth first, with an
// explicit stack), then clears the visited marks again.
int TrackType::PaintTrack()
{
    SegmentList_Tag *stack;
    TrackData_Tag *link;
    TrackSegment_Tag **segments;
    int i;
    int off;
    SegmentList_Tag *node;
    TrackSegment_Tag *segment;
    SegmentList_Tag *child;
    stack = 0;
    node = AllocNode(this);
    if (!node)
        return 0;
    node->m_segment = m_start;
    node->m_next = stack;
    stack = node;
    while ((node = stack) != 0) {
        segment = node->m_segment;
        if (segment->m_flags & 4) {
            stack = node->m_next;
            if (node) {
                node->m_next = m_freeSegmentLists;
                m_freeSegmentLists = node;
            }
            continue;
        }
        for (link = segment->m_nodes; link && link->m_next; link = link->m_next)
            ;
        segment->m_flags |= 4;
        // off is i * sizeof(TrackSegment_Tag *), kept by hand (see the notes above).
        for (i = 0, off = 0; i < node->m_segment->m_numLinks; off += PS2_PTR_SIZE, i++) {
            if ((*(TrackSegment_Tag **)((char *)node->m_segment->m_links + off))->m_flags & 4)
                continue;
            child = AllocNode2(this);
            if (!child) {
                FreeSegmentList(&stack, 0);
                return 0;
            }
            child->m_segment = *(TrackSegment_Tag **)((char *)node->m_segment->m_links + off);
            child->m_next = stack;
            stack = child;
        }
    }
    segments = m_segments;
    for (i = 0; i < m_numSegments; i++)
        segments[i]->m_flags &= ~4;
    return 1;
}

