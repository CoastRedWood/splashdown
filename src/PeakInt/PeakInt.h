// PeakInt: the largest value recorded over a time window (ms, timeGetTime).
// 0x0C bytes; static instances in Terrain.cpp, VisibilityQuadTree.cpp and
// PSXTextureMapManager.cpp. The PeakInt files keep local views for now.
#ifndef PEAKINT_PEAKINT_H
#define PEAKINT_PEAKINT_H

class PeakInt {
public:
    PeakInt(unsigned int window);
    int GetPeak();
    void Record(int value);

    unsigned int m_time;                // +0x0
    int m_peak;                         // +0x4
    unsigned int m_window;              // +0x8
};

#endif
