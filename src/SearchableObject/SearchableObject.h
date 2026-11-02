// SearchableObject: the 4-byte, non-polymorphic base of objects the
// search tree can place (SoultreeObject, CollisionObject, VehicleAIClass,
// SearchableGameObject). Its constructor is always inlined (retail has no
// out-of-line copy) and stores m_x00, m_typeID, then each bit. It is 4
// bytes: Camera and LightEmitter keep it at +0x30 with their own data from
// +0x34. As a first base before the 8-aligned GameObject it still takes 8.
#ifndef SEARCHABLEOBJECT_H
#define SEARCHABLEOBJECT_H

// x86-64: 4-aligned, as on the PS2. The search lists tag pointers in their
// low two bits (bit 0: sorted, bit 1: skip); with 2-byte alignment clang
// put SearchableGameObject's SearchableObject in GameObject's tail padding
// at +0x3A, and PrepareGeometryOnSearchListContents skipped every one.
#ifdef SPLASHDOWN_PC
#define SEARCHABLEOBJECT_ALIGN __attribute__((aligned(4)))
#else
#define SEARCHABLEOBJECT_ALIGN
#endif

class SEARCHABLEOBJECT_ALIGN SearchableObject {
public:
    SearchableObject() { m_x00 = 0; m_typeID = 0xFF; m_searchVisible = 0; m_addToList = 0; m_hidden = 0; }

    short m_x00;                    // +0x0
    unsigned char m_typeID;         // +0x2, 0xFF until placed
    unsigned char m_searchVisible : 1;  // +0x3 bit 0 (SoultreeObject has its own m_visible)
    unsigned char m_addToList : 1;  //      bit 1
    unsigned char m_hidden : 1;     //      bit 2
};

#endif
