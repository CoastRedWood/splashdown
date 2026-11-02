// ContainerList<T> — growable array template used across the engine.
// Members are declared here; each method's body lives in its own .cpp,
// which explicitly instantiates the class for every retail element type.
// (mwccps2 2.4 can't explicitly instantiate a single member, but
// `template class` only emits members whose definitions are visible.)
#ifndef CONTAINERLIST_H
#define CONTAINERLIST_H

class PSXSound;
class SoundEmitter;
class GameObject;
class AnalogAxis;
struct _tLoadedSound;
struct _tStreamData;
struct _SoundResponse;
struct Rider_Stream_Info;
struct ObjectSoundData;
struct _sceDmaTag;
class VideoPreFlipObject;
class Sound;

extern "C" void *memset(void *dst, int c, unsigned int n);
// MEMCPY_DECLARED: a file that declares memcpy itself (PSXTexMap.cpp: a
// non-const source, as retail) defines it first (DmaRenderItem.h does the same).
#ifndef MEMCPY_DECLARED
#define MEMCPY_DECLARED
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
#endif

template <class T>
class ContainerList {
public:
    // Declared in retail's order: mwcc instantiates a class template's
    // members at the end of the TU in declaration order (retail's text:
    // ctor, Create, AddItem, RemoveItem(ByIndex), GetItem, Sort, Clear; Grow, only
    // reached through AddItem, is declared before it, so it is found after
    // the pass and comes last).
    ContainerList();
    int Create(int size, int growBy);
    int Grow(int newMax);
    int AddItem(T item);
    int AddItemUnique(T item);
    int RemoveItem(T item);
    int RemoveItemByIndex(int i);
    T GetItem(int i);
    void Sort(int (*cmp)(const void *, const void *));
    void Clear();
    ~ContainerList() { delete[] m_items; }
    // ~ContainerList() is not declared here. Retail's dtor is
    // `{ delete[] m_items; }` (out-of-line copies for <char *>, <AnalogAxis *>,
    // <Rider_Stream_Info *>), but declaring it inline makes the ctors of
    // classes with ContainerList members emit copies retail doesn't have
    // (AuralScape, PSXSoundGroup, the sound-emitter ctors). Files that
    // destroy lists declare it themselves.

    int m_numItems;     // +0x00
    T *m_items;         // +0x04
    int m_growBy;       // +0x08
    int m_maxItems;     // +0x0C
    unsigned char m_created : 1;  // +0x10, set by Create()
};

#endif  // CONTAINERLIST_H
