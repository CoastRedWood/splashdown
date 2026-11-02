// BaseObject.cpp  @ 0x00203740 (text 0x203740-0x203810)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// A retail translation unit without a static initializer: BaseObject
// (GR_BitString.cpp follows). Its .rodata is @365-@667 (labels restart after
// SoundGroup.cpp's @585; the BaseObject RTTI string first, then
// GR_BitString/GR_PixelString); BoundingBoxTree.cpp restarts at @421.


#include "GameObject/GameObject.h"
#include "BaseObject/BaseObject.h"
#include "GR_BitString/GR_BitString.h"

// ---- src/BaseObject/ctor.cpp

BaseObject::BaseObject() : m_refCount(1) {}


// ---- src/BaseObject/dtor.cpp

BaseObject::~BaseObject()
{
}


// ---- src/BaseObject/AddRef_002037B0.cpp
long BaseObject::AddRef()
{
    return ++m_refCount;
}


// ---- src/BaseObject/Release.cpp

long BaseObject::Release()
{
    m_refCount--;
    if (m_refCount == 0) {
        delete this;
        return 0;
    }
    return m_refCount;
}
