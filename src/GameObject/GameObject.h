// BaseObject and GameObject with their full retail layout. This is the one
// shared definition: include it instead of declaring GameObject locally.
//
// GameObject is 0x30 bytes, so a direct subclass's members start at +0x30
// (+0x38 after an 8-byte SearchableObject first base). It is 8-aligned
// (BaseObject's m_refCount is a 64-bit long), so every subclass's size
// rounds up to 8: SoultreeObject is 0x1B0, UIManager 0x148, UIWidget 0x48.
// Globals that GameObject's own methods use are in GameObjectGlobals.h.
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

enum DumpType {};
struct FILE_HANDLE_STRUCT;
class KeyInfo;
class ProcessNode;

class BaseObject {
public:
    BaseObject();
    long m_refCount;            // +0x0 (64-bit)
    virtual ~BaseObject();      // vptr at +0x8
    virtual long AddRef();
    virtual long Release();
    virtual long GetRefCount();
};

// Event-mask bits (m_wantedEvents / m_descendantEvents).
enum {
    GOEVENT_TICK = 0x2,
    GOEVENT_RENDER3D = 0x8,
    GOEVENT_PAUSE = 0x20,
    GOEVENT_EVENT = 0x100
};

class GameObject : public BaseObject {
public:
    GameObject(int enabled);
    int RemoveFromChain();
    void UpPropagateMasks(unsigned int mask);
    unsigned int ReEvaluateDescendantsWantedEventMasks();
    long ReleaseSiblingsInReverseCreationOrder();
    GameObject *AddChild(GameObject *child, unsigned int wantedEvents);
    int InsertBefore(GameObject *other, unsigned int wantedEvents);

    // --- virtuals in retail slot order ---
    virtual ~GameObject();
    virtual long Release();
    virtual void Disable();
    virtual void DisableAll();
    virtual void Enable();
    virtual void EnableAll();
    virtual void AncestorsDisabled();
    virtual void AncestorsEnabled();
    virtual GameObject *Create();
    virtual int TickWhileInactive(float dt);
    virtual int Tick(float dt);
    virtual int SimulationTick(float dt);
    virtual int PrepareGeometry();
    virtual int RenderPre3D();
    virtual int Render3D();
    virtual int RenderPost3D();
    virtual int Pause(int pause);
    virtual int LostFocus();
    virtual int Restore();
    virtual int EventHandler(int event);
    virtual int KeyUpHandler(ProcessNode *node, KeyInfo *key);
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual int PacketHandler(unsigned int, void *, unsigned int, unsigned int, unsigned int);
    virtual void Kill();
    virtual void DumpHierarchy(char *, DumpType, int);
    void DumpHierarchy(FILE_HANDLE_STRUCT *file, DumpType type, int depth);

    GameObject *m_prev;                 // +0x10, previous sibling
    GameObject *m_next;                 // +0x14, next sibling
    GameObject *m_child;                // +0x18, first child
    GameObject *m_parent;               // +0x1C
    unsigned int m_wantedEvents;        // +0x20
    unsigned int m_descendantEvents;    // +0x24
    unsigned char m_28;                 // +0x28
    unsigned char m_enabled : 1;        // +0x29 bit 0
    unsigned char m_active : 1;         //        bit 1
    unsigned char m_paused : 1;         //        bit 2
    unsigned char m_killed : 1;         //        bit 3
    unsigned char m_hidden : 1;         //        bit 4
};

#endif
