// StripOffset: where one PSXMesh strip's vertex attributes sit in its DMA
// data (0x34 bytes; PSXMesh::Create news an array of them).
#ifndef STRIPOFFSET_H
#define STRIPOFFSET_H

// Per-vertex location (0x14 bytes, retail's name; it has a ctor, so new[]
// keeps an array header).
struct StripVertexInfo {
    StripVertexInfo();
    int m_count;                // +0x00
    int m_substrip[2];          // +0x04
    int m_offset[2];            // +0x0C, in quadwords
};

class StripOffset {
public:
    StripOffset();
    ~StripOffset();

    int m_x00;                  // +0x00
    int m_x04;                  // +0x04
    int m_x08;                  // +0x08
    int m_x0C;                  // +0x0C
    int m_x10;                  // +0x10 (-1)
    int *m_array14;             // +0x14 (new[]'d)
    int *m_array18;             // +0x18 (new[]'d)
    int *m_substripBase;        // +0x1C, per substrip, in floats
    int *m_normalBase;          // +0x20, per substrip, in floats
    int *m_tangentBase;         // +0x24, per substrip, in floats
    int *m_colorBase;           // +0x28, per substrip, in words
    int *m_uvBase;              // +0x2C, per substrip, in floats
    StripVertexInfo *m_vertexInfo;  // +0x30
};

#endif
