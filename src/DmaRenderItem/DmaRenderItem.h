// DmaRenderItem — builds a DMA chain of VIF unpacks.
#ifndef DMARENDERITEM_H
#define DMARENDERITEM_H

#include "ps2_port.h"

// MEMCPY_DECLARED: a file that declares memcpy itself (PSXTexMap.cpp: a
// non-const source, as retail) defines it first.
#ifndef MEMCPY_DECLARED
#define MEMCPY_DECLARED
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
#endif

struct DmaTag {
    unsigned short qwc;     // +0x0
    unsigned char mark;     // +0x2
    unsigned char id;       // +0x3
    PS2_PTR(unsigned int) addr;     // +0x4
    unsigned int pad[2];    // +0x8
};

enum DmaRenderDestination {};
struct sceGifTag;
struct _D3DINTVECTOR;
struct _D3DMATRIX;
struct _D3DVECTOR;
struct QWORD;

class DmaRenderItem {
public:
    DmaRenderItem();
    ~DmaRenderItem();
    void Start(DmaRenderDestination destination, int size);
    void End();
    void DmaStartTag();
    void DmaEndTag();
    int DmaBeginUnpack(int address, int format);
    void DmaAddDataToUnpack(void *data, int count);
    void DmaAddGifTagToUnpack(sceGifTag *tag);
    void DmaAddIntVectorToUnpack(_D3DINTVECTOR *v);
    void DmaAddMatrixToUnpack(_D3DMATRIX *m);
    void DmaAddQwordToUnpack(QWORD *q);
    void DmaAddVectorToUnpack(_D3DVECTOR *v);
    void DmaAddVifCode(unsigned int code);
    void DmaAddVifCodeMSCAL();
    // Out of line here; DmaAddVifCode.cpp and others inline their own copy.
    void IncrementDataList(int count);
    void DmaEndUnpack();

    static int UnpackCommandIndex;
    static int MemLoadAddressBegin;
    static int UnpackCount;
    static int UnpackFormat;

    DmaTag m_tag;                    // +0x00
    char pad10[0x20 - 0x10];         // cleared with m_tag by Start
    unsigned int *m_dataList;        // +0x20
    int m_dataListMaxSize;           // +0x24
    int m_dataListCurrentSize;       // +0x28
    int m_destination;               // +0x2C, DmaRenderDestination; -1 until Start
};                                   // size 0x30

#endif
