// DmaRenderItem.cpp  @ 0x0022D0F0 (text 0x22D0F0-0x22DE20)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's DmaRenderItem.cpp as one translation unit, a TU without a static
// initializer at the end of Cube.cpp's tu_map row: "DmaRenderItem.cpp" (the
// ASSERTs) and its strings at 0x4113D0-0x411528, right before EcoSystem.cpp's;
// DmaRenderItem's statics (.bss 0x451D40-0x451D58) are defined here.
// Create3DVegetationTransformRenderItem (0x22DC70, no data) opens
// src/tu/EcoSystem.cpp, with the other vegetation VIF items. IncrementDataList is
// defined inline before DmaAddVifCode, which inlines it (its ASSERT statics are
// retail's ...$localstaticN$IncrementDataList...). `make link` places its data as
// well as its code.


#include "DmaRenderItem/DmaRenderItem.h"
#include "soultree_assert_block.h"
#include "sceGifTag/sceGifTag.h"

extern "C" void free(void *p);
extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" void *malloc(unsigned int size);
extern "C" void *realloc(void *p, unsigned int size);
extern "C" void *memalign(unsigned int alignment, unsigned int size);

typedef int int128_t __attribute__((mode(TI)));

int DmaRenderItem::UnpackCommandIndex;
int DmaRenderItem::MemLoadAddressBegin;
int DmaRenderItem::UnpackCount;
int DmaRenderItem::UnpackFormat;

// ---- src/DmaRenderItem/ctor.cpp

DmaRenderItem::DmaRenderItem()
{
    m_destination = -1;
    m_dataListMaxSize = 0;
    m_dataListCurrentSize = 0;
    m_dataList = 0;
}

// ---- src/DmaRenderItem/dtor.cpp

DmaRenderItem::~DmaRenderItem()
{
    if (m_dataList)
        free(m_dataList);
}

// ---- src/DmaRenderItem/Start.cpp

// Begins a new DMA chain, growing the data list to hold size words.
void DmaRenderItem::Start(DmaRenderDestination destination, int size)
{
#line 54 "DmaRenderItem.cpp"
    ASSERT(size % 4 == 0, "Start: DmaRenderItem not multiple of 4");
    m_destination = destination;
    memset(this, 0, 0x20);
    if (!m_dataList) {
        m_dataListMaxSize = size;
        m_dataList = (unsigned int *)malloc(m_dataListMaxSize * 4);
#line 63 "DmaRenderItem.cpp"
        ASSERT(m_dataList != 0, "Start: DataList == 0");
    } else if (m_dataListMaxSize < size) {
        m_dataListMaxSize = size;
        m_dataList = (unsigned int *)realloc(m_dataList, m_dataListMaxSize * 4);
#line 70 "DmaRenderItem.cpp"
        ASSERT(m_dataList != 0, "Start: DataList == 0");
    }
    m_dataListCurrentSize = 0;
    MemLoadAddressBegin = 0;
    DmaStartTag();
}

// ---- src/DmaRenderItem/End.cpp

void DmaRenderItem::End()
{
#line 80 "DmaRenderItem.cpp"
    ASSERT(m_dataListCurrentSize <= m_dataListMaxSize, "End: DataListCurrentSize > DataListMaxSize");
    DmaEndTag();
}

// ---- src/DmaRenderItem/DmaStartTag.cpp

void DmaRenderItem::DmaStartTag() {}

// ---- src/DmaRenderItem/DmaEndTag.cpp

void DmaRenderItem::DmaEndTag()
{
    unsigned int *p = &m_dataList[m_dataListCurrentSize];
    while ((unsigned int)p & 0xF)
        *p++ = 0;
    unsigned int *list = m_dataList;
    m_tag.qwc = (p - list) >> 2;
    m_tag.mark = 0;
    m_tag.id = 0x30;
    m_tag.addr = list;
    m_tag.pad[0] = 0;
    m_tag.pad[1] = 0;
}

// ---- src/DmaRenderItem/DmaAddMatrixToUnpack.cpp

void DmaRenderItem::DmaAddMatrixToUnpack(_D3DMATRIX *data)
{
    IncrementDataList(0x10);
    UnpackCount += 0x10;
    memcpy(&m_dataList[m_dataListCurrentSize], data, 0x40);
    m_dataListCurrentSize += 0x10;
}

// ---- src/DmaRenderItem/DmaAddVectorToUnpack.cpp

void DmaRenderItem::DmaAddVectorToUnpack(_D3DVECTOR *data)
{
    IncrementDataList(4);
    UnpackCount += 4;
    memcpy(&m_dataList[m_dataListCurrentSize], data, 0x10);
    m_dataListCurrentSize += 4;
}

// ---- src/DmaRenderItem/DmaAddIntVectorToUnpack.cpp

void DmaRenderItem::DmaAddIntVectorToUnpack(_D3DINTVECTOR *data)
{
    IncrementDataList(4);
    UnpackCount += 4;
    memcpy(&m_dataList[m_dataListCurrentSize], data, 0x10);
    m_dataListCurrentSize += 4;
}

// ---- src/DmaRenderItem/DmaAddQwordToUnpack.cpp

void DmaRenderItem::DmaAddQwordToUnpack(QWORD *data)
{
    IncrementDataList(4);
    UnpackCount += 4;
    memcpy(&m_dataList[m_dataListCurrentSize], data, 0x10);
    m_dataListCurrentSize += 4;
}

// ---- src/DmaRenderItem/DmaAddGifTagToUnpack.cpp

void DmaRenderItem::DmaAddGifTagToUnpack(sceGifTag *data)
{
    IncrementDataList(4);
    UnpackCount += 4;
    memcpy(&m_dataList[m_dataListCurrentSize], data, 0x10);
    m_dataListCurrentSize += 4;
}

// ---- src/DmaRenderItem/DmaAddDataToUnpack.cpp

void DmaRenderItem::DmaAddDataToUnpack(void *data, int count)
{
    IncrementDataList(count);
    UnpackCount += 4;   // retail adds 4 whatever the count
    memcpy(&m_dataList[m_dataListCurrentSize], data, count * 4);
    m_dataListCurrentSize += count;
}

// ---- src/DmaRenderItem/DmaBeginUnpack.cpp

int DmaRenderItem::DmaBeginUnpack(int address, int format)
{
    UnpackFormat = format;
    UnpackCount = 0;
    UnpackCommandIndex = m_dataListCurrentSize;
    if (address == -1)
        address = MemLoadAddressBegin;
    MemLoadAddressBegin = address;
    IncrementDataList(1);
    m_dataListCurrentSize++;
    return m_dataListCurrentSize - 1;
}

// ---- src/DmaRenderItem/DmaEndUnpack.cpp

#define SCE_VIF1_SET_UNPACK(vuaddr, num, cmd, irq) \
    ((unsigned int)(vuaddr) | ((unsigned int)(num) << 16) | \
    ((unsigned int)(0x60 | (cmd)) << 24) | ((unsigned int)(irq) << 31))

// Patches the UNPACK command opened by DmaBeginUnpack with the final
// quadword count (256 is encoded as 0); destination 1 sets the FLG bit.
void DmaRenderItem::DmaEndUnpack()
{
#line 210 "DmaRenderItem.cpp"
    ASSERT(UnpackCount > 0, "DmaEndUnpack: UnpackCount <= 0");
    int qwords = (UnpackCount + 3) / 4;
#line 212 "DmaRenderItem.cpp"
    ASSERT(qwords <= 256, "DmaEndUnpack: unpackQwordCount > 256");
    if (m_destination == 0) {
        m_dataList[UnpackCommandIndex] =
            SCE_VIF1_SET_UNPACK(MemLoadAddressBegin, qwords == 256 ? 0 : qwords, UnpackFormat, 0);
    } else if (m_destination == 1) {
        m_dataList[UnpackCommandIndex] =
            SCE_VIF1_SET_UNPACK(MemLoadAddressBegin | 0x8000, qwords == 256 ? 0 : qwords, UnpackFormat, 0);
    }
    MemLoadAddressBegin += qwords;
}

// ---- src/DmaRenderItem/IncrementDataList.cpp

inline void DmaRenderItem::IncrementDataList(int count)
{
#line 284 "DmaRenderItem.cpp"
    ASSERT(m_dataListCurrentSize <= m_dataListMaxSize, "IncrementDataList: A memory overwrite error has occured.");
    if (m_dataListCurrentSize + count > m_dataListMaxSize) {
        m_dataListMaxSize += 128;
        m_dataList = (unsigned int *)realloc(m_dataList, m_dataListMaxSize * 4);
#line 291 "DmaRenderItem.cpp"
        ASSERT(m_dataList != 0, "IncrementDataList: DataList == 0.");
    }
}

// ---- src/DmaRenderItem/DmaAddVifCode.cpp

void DmaRenderItem::DmaAddVifCode(unsigned int code)
{
    IncrementDataList(1);
    m_dataList[m_dataListCurrentSize] = code;
    m_dataListCurrentSize++;
}

// ---- src/DmaRenderItem/DmaAddVifCodeMSCAL.cpp

// VIF code MSCAL (0x14): start the VU1 microprogram at address 0x19.
void DmaRenderItem::DmaAddVifCodeMSCAL()
{
    DmaAddVifCode(0x14000019);
}
