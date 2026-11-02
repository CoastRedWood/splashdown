// GameObject.cpp  @ 0x00201DE0 (text 0x201DE0-0x203440)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// A retail translation unit without a static initializer: GameObject, the weak
// BaseObject::GetRefCount, do_global_dtors and mwInit. Split from Game.cpp:
// mwcc writes RTTI only at the end of a TU, and retail's Game RTTI (@996) lies
// between Game's literals and this file's (@1073-@1121, ending with the
// GameObject/BaseObject RTTI), so Game.cpp ended there. Retail's labels don't
// restart here because each TU's count starts after its headers'.


#include "DataAccess/DataAccess.h"
#include "Game/Game.h"
#include "PSXControlInterface/PSXControlInterface.h"
#include "GameObject/GameObject.h"
#include "AuralScape/AuralScape.h"
#include "LightManager/LightManager.h"
#include "PSXTextureMapManager/PSXTextureMapManager.h"
#include "RenderTarget/RenderTarget.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "Camera/Camera.h"
#include "DebugOverlay/DebugOverlay.h"
#include "ControlInterface/ControlInterface.h"
#include "PSXSoundInterface/PSXSoundInterface.h"
#include "GameObject/GameObjectGlobals.h"
#include "BaseObject/BaseObject.h"
#include "MemoryPoolManager/MemoryPoolManager.h"
#include "ControlInterface/ControlTypes.h"

// Defined here for its retail .bss slot.
static int SearchesOpen;
static int TotalGameObjects;



// ---- src/GameObject/ctor.cpp

GameObject::GameObject(int enabled)
{
    m_28 = 0xFF;
    m_prev = 0;
    m_next = 0;
    m_child = 0;
    m_parent = 0;
    m_enabled = enabled;
    m_active = enabled;
    m_paused = 0;
    m_killed = 0;
    m_wantedEvents = 0;
    m_descendantEvents = 0;
    TotalGameObjects++;
}


// ---- src/GameObject/dtor.cpp

GameObject::~GameObject()
{
    TotalGameObjects--;
}


// ---- src/GameObject/AncestorsDisabled.cpp

void GameObject::AncestorsDisabled()
{
    for (GameObject *child = m_child; child; child = child->m_next) {
        if (!child->m_killed)
            child->AncestorsDisabled();
    }
}


// ---- src/GameObject/AncestorsEnabled.cpp

void GameObject::AncestorsEnabled()
{
    for (GameObject *child = m_child; child; child = child->m_next) {
        if (!child->m_killed)
            child->AncestorsEnabled();
    }
}


// ---- src/GameObject/Disable.cpp

void GameObject::Disable()
{
    m_enabled = 0;
    m_active = 0;
    AncestorsDisabled();
}


// ---- src/GameObject/Enable.cpp

void GameObject::Enable()
{
    m_enabled = 1;
    m_active = 1;
    AncestorsEnabled();
}


// ---- src/GameObject/DisableAll.cpp

void GameObject::DisableAll()
{
    m_enabled = 0;
    m_active = 0;
    for (GameObject *child = m_child; child; child = child->m_next) {
        if (!child->m_killed)
            child->DisableAll();
    }
}


// ---- src/GameObject/EnableAll.cpp

void GameObject::EnableAll()
{
    m_enabled = 1;
    m_active = 1;
    for (GameObject *child = m_child; child; child = child->m_next) {
        if (!child->m_killed)
            child->EnableAll();
    }
}


// ---- src/GameObject/UpPropagateMasks.cpp

void GameObject::UpPropagateMasks(unsigned int mask)
{
    m_descendantEvents |= mask;
    if (m_parent)
        m_parent->UpPropagateMasks(m_descendantEvents | m_wantedEvents);
}


// ---- src/GameObject/ReEvaluateDescendantsWantedEventMasks.cpp

unsigned int GameObject::ReEvaluateDescendantsWantedEventMasks()
{
    unsigned int mask = 0;
    for (GameObject *child = m_child; child; child = child->m_next)
        mask |= child->ReEvaluateDescendantsWantedEventMasks();
    m_descendantEvents = mask;
    return m_descendantEvents | m_wantedEvents;
}


// ---- src/GameObject/AddChild.cpp

GameObject *GameObject::AddChild(GameObject *child, unsigned int wantedEvents)
{
    if (SearchesOpen)
        return 0;
    if (child) {
        if (!m_child) {
            m_child = child;
            child->m_prev = 0;
        } else {
            GameObject *last = m_child;
            while (last->m_next)
                last = last->m_next;
            last->m_next = child;
            child->m_prev = last;
        }
        for (GameObject *p = child; p; p = p->m_next)
            p->m_parent = this;
        child->m_wantedEvents = wantedEvents;
        child->UpPropagateMasks(child->m_descendantEvents);
    }
    return child;
}


// ---- src/GameObject/RemoveFromChain.cpp

int GameObject::RemoveFromChain()
{
    if (SearchesOpen)
        return 0;
    if (m_prev)
        m_prev->m_next = m_next;
    if (m_next)
        m_next->m_prev = m_prev;
    if (m_parent && m_parent->m_child == this)
        m_parent->m_child = m_next;
    GameObject *root = this;
    while (root->m_parent)
        root = root->m_parent;
    root->ReEvaluateDescendantsWantedEventMasks();
    m_prev = m_next = m_parent = 0;
    return 1;
}


// ---- src/GameObject/InsertBefore.cpp

int GameObject::InsertBefore(GameObject *other, unsigned int wantedEvents)
{
    if (SearchesOpen)
        return 0;
    RemoveFromChain();
    m_prev = other->m_prev;
    m_next = other;
    if (m_prev)
        m_prev->m_next = this;
    m_next->m_prev = this;
    if (other->m_parent && other->m_parent->m_child == other)
        other->m_parent->m_child = this;
    m_wantedEvents = wantedEvents;
    UpPropagateMasks(m_descendantEvents);
    return 1;
}


// ---- src/GameObject/Kill.cpp

void GameObject::Kill()
{
    for (GameObject *child = m_child; child; child = child->m_next)
        child->Kill();
    m_killed = 1;
}


// ---- src/GameObject/Create.cpp
// (The old draft here also held a copy of Render3D__10GameObjectFv; its draft is src/GameObject/Render3D.cpp.)


GameObject *GameObject::Create()
{
    return this;
}


// ---- src/GameObject/PrepareGeometry.cpp

int GameObject::PrepareGeometry()
{
    if (m_descendantEvents & 0x1000) {
        for (GameObject *child = m_child; child; child = child->m_next) {
            if (child->m_enabled && !child->m_killed && (child->m_wantedEvents & 0x1000)) {
                LpRenderTarget->TrySendRenderQueue(0);
                child->PrepareGeometry();
            }
        }
    }
    return 1;
}


// ---- src/GameObject/RenderPre3D.cpp

int GameObject::RenderPre3D()
{
    if (m_descendantEvents & 0x4) {
        for (GameObject *child = m_child; child; child = child->m_next) {
            if (child->m_enabled && !child->m_killed && (child->m_wantedEvents & 0x4))
                child->RenderPre3D();
        }
    }
    return 1;
}


// ---- src/GameObject/Render3D.cpp

int GameObject::Render3D()
{
    if (m_descendantEvents & 0x8) {
        for (GameObject *child = m_child; child; child = child->m_next) {
            if (child->m_enabled && !child->m_killed && (child->m_wantedEvents & 0x8)) {
                LpRenderTarget->TrySendRenderQueue(0);
                child->Render3D();
            }
        }
    }
    return 1;
}


// ---- src/GameObject/RenderPost3D.cpp

int GameObject::RenderPost3D()
{
    if (m_descendantEvents & 0x10) {
        for (GameObject *child = m_child; child; child = child->m_next) {
            if (child->m_enabled && !child->m_killed && (child->m_wantedEvents & 0x10)) {
                LpRenderTarget->TrySendRenderQueue(0);
                child->RenderPost3D();
            }
        }
    }
    return 1;
}


// ---- src/GameObject/Tick.cpp

int GameObject::Tick(float dt)
{
    if (m_descendantEvents & 0x2) {
        for (GameObject *child = m_child; child; child = child->m_next) {
            if (child->m_enabled && !child->m_paused && !child->m_killed && (child->m_wantedEvents & 0x2)) {
                LpRenderTarget->TrySendRenderQueue(0);
                child->Tick(dt);
            }
        }
    }
    return 1;
}


// ---- src/GameObject/TickWhileInactive.cpp

int GameObject::TickWhileInactive(float dt)
{
    if (m_descendantEvents & 0x1) {
        for (GameObject *child = m_child; child; child = child->m_next) {
            if (child->m_enabled && !child->m_paused && !child->m_killed && (child->m_wantedEvents & 0x1))
                child->TickWhileInactive(dt);
        }
    }
    return 1;
}


// ---- src/GameObject/Pause.cpp

int GameObject::Pause(int pause)
{
    m_paused = pause;
    if (m_descendantEvents & 0x20) {
        for (GameObject *child = m_child; child; child = child->m_next) {
            if (child->m_enabled && !child->m_killed && (child->m_wantedEvents & 0x20))
                child->Pause(pause);
        }
    }
    return 1;
}


// ---- src/GameObject/Restore.cpp

int GameObject::Restore()
{
    if (m_descendantEvents & 0x80) {
        for (GameObject *child = m_child; child; child = child->m_next) {
            if (!child->m_killed)
                child->Restore();
        }
    }
    return 1;
}


// ---- src/GameObject/LostFocus.cpp

int GameObject::LostFocus()
{
    if (m_descendantEvents & 0x40) {
        for (GameObject *child = m_child; child; child = child->m_next) {
            if (!child->m_killed)
                child->LostFocus();
        }
    }
    return 1;
}


// ---- src/GameObject/EventHandler.cpp

int GameObject::EventHandler(int event)
{
    if (m_descendantEvents & 0x100) {
        for (GameObject *child = m_child; child; child = child->m_next) {
            if (child->m_enabled && !child->m_killed) {
                if (child->EventHandler(event))
                    return 1;
            }
        }
    }
    return 0;
}


// ---- src/GameObject/KeyUpHandler.cpp

int GameObject::KeyUpHandler(ProcessNode *node, KeyInfo *key)
{
    if (m_descendantEvents & 0x200) {
        for (GameObject *child = m_child; child; child = child->m_next) {
            if (child->m_enabled && !child->m_killed) {
                if (child->KeyUpHandler(node, key))
                    return 1;
            }
        }
    }
    return 0;
}


// ---- src/GameObject/KeyDownHandler.cpp

int GameObject::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (m_descendantEvents & 0x400) {
        for (GameObject *child = m_child; child; child = child->m_next) {
            if (child->m_enabled && !child->m_killed) {
                if (child->KeyDownHandler(node, key))
                    return 1;
            }
        }
    }
    return 0;
}


// ---- src/GameObject/PacketHandler.cpp

int GameObject::PacketHandler(unsigned int type, void *data, unsigned int a, unsigned int b, unsigned int c)
{
    if (m_descendantEvents & 0x800) {
        for (GameObject *child = m_child; child; child = child->m_next) {
            if (child->m_active && !child->m_killed) {
                if (child->PacketHandler(type, data, a, b, c))
                    return 1;
            }
        }
    }
    return 0;
}


// ---- src/GameObject/ReleaseSiblingsInReverseCreationOrder.cpp

long GameObject::ReleaseSiblingsInReverseCreationOrder()
{
    if (SearchesOpen)
        return 0;
    if (m_next)
        m_next->ReleaseSiblingsInReverseCreationOrder();
    if (m_prev)
        m_prev->m_next = 0;
    m_next = 0;
    return Release();
}


// ---- src/GameObject/Release.cpp

long GameObject::Release()
{
    if (SearchesOpen)
        return 0;
    if (m_child)
        m_child->ReleaseSiblingsInReverseCreationOrder();
    if (m_prev)
        m_prev->m_next = m_next;
    if (m_next)
        m_next->m_prev = m_prev;
    if (m_parent && m_parent->m_child == this)
        m_parent->m_child = m_next;
    return BaseObject::Release();
}


// ---- src/GameObject/DumpHierarchy.cpp
extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);
extern "C" unsigned int strlen(const char *s);
extern "C" int sprintf(char *buf, const char *fmt, ...);
void PSXprintf(const char *fmt, ...);

struct FILE_HANDLE_STRUCT;
unsigned int FWRITE(void *buf, unsigned int size, unsigned int count, FILE_HANDLE_STRUCT *file);

namespace std {
class type_info {
public:
    const char *name() const { return m_name; }
private:
    const char *m_name;
};
}


// DumpType bits
enum {
    DUMP_ENABLED_ONLY = 1,
    DUMP_WANTED_EVENTS = 2,
};


// Writes one line per object: indent, class name (in parentheses when
// disabled), address and optionally the wanted-event mask, then recurses.
void GameObject::DumpHierarchy(FILE_HANDLE_STRUCT *file, DumpType type, int depth)
{
    char typeName[0x80];
    char line[0x100];
    char address[0x1C];

    const std::type_info &info = typeid(*this);
    char indent[] = "  ";
    strcpy(typeName, info.name());
    if (!file) {
        PSXprintf("Invalid File Handle in GameObject::DumpHeirarchy");
        return;
    }
    line[0] = 0;
    for (int i = 0; i < depth; i++)
        strcat(line, indent);
    if (!m_enabled) {
        if (type & DUMP_ENABLED_ONLY)
            return;
        strcat(line, "(");
        strcat(line, typeName);
        strcat(line, ")");
    } else {
        strcat(line, typeName);
    }
    strcat(line, ", 0x");
    sprintf(address, "%X", this);
    strcat(line, address);
    if (type & DUMP_WANTED_EVENTS) {
        if (m_wantedEvents == -1) {
            strcat(line, " Wants all events");
        } else if (m_wantedEvents == 0) {
            strcat(line, " Wants nothing");
        } else {
            if (m_wantedEvents & 0x1)
                strcat(line, ", tick while inactive");
            if (m_wantedEvents & 0x2)
                strcat(line, ", wants tick");
            if (m_wantedEvents & 0x4)
                strcat(line, ", wants RenderPre3D");
            if (m_wantedEvents & 0x8)
                strcat(line, ", wants Render3D");
            if (m_wantedEvents & 0x10)
                strcat(line, ", wants RenderPost3D");
            if (m_wantedEvents & 0x20)
                strcat(line, ", wants pause");
            if (m_wantedEvents & 0x40)
                strcat(line, ", wants lost focus");
            if (m_wantedEvents & 0x80)
                strcat(line, ", wants restore");
            if (m_wantedEvents & 0x100)
                strcat(line, ", wants event handler");
            if (m_wantedEvents & 0x200)
                strcat(line, ", wants keyup");
            if (m_wantedEvents & 0x400)
                strcat(line, ", wants keydown");
            if (m_wantedEvents & 0x800)
                strcat(line, ", wants packet handler");
            if (m_wantedEvents & 0x1000)
                strcat(line, ", wants prepare geom");
        }
    }
    strcat(line, "\r\n");
    FWRITE(line, 1, strlen(line), file);
    for (GameObject *child = m_child; child; child = child->m_next)
        child->DumpHierarchy(file, type, depth + 1);
}


// ---- src/GameObject/DumpHierarchy_00203260.cpp
extern "C" int sprintf(char *buf, const char *fmt, ...);
void PSXprintf(const char *fmt, ...);

struct FILE_HANDLE_STRUCT;
FILE_HANDLE_STRUCT *FOPEN(char *name, const char *mode);
void FCLOSE(FILE_HANDLE_STRUCT *file);
void FPRINTF(FILE_HANDLE_STRUCT *file, const char *fmt, ...);



// Write the hierarchy to the first free HierarchyN.txt.
void GameObject::DumpHierarchy(char *title, DumpType type, int depth)
{
    static int currentDump = 0;
    char name[260];
    FILE_HANDLE_STRUCT *file;
    sprintf(name, "Hierarchy%d.txt", currentDump);
    for (file = FOPEN(name, "r"); file; file = FOPEN(name, "r")) {
        FCLOSE(file);
        currentDump++;
        sprintf(name, "Hierarchy%d.txt", currentDump);
    }
    file = FOPEN(name, "w");
    if (!file) {
        PSXprintf("Cannot open %s in GameObject::DumpHeirarchy", name);
        return;
    }
    if (title)
        FPRINTF(file, "GameObject Hierarchy Dump (%s)\n\n", title);
    else
        FPRINTF(file, "GameObject Hierarchy Dump\n\n");
    DumpHierarchy(file, type, depth);
    FCLOSE(file);
}


// ---- src/GameObject/SimulationTick.cpp

int GameObject::SimulationTick(float)
{
    return 1;
}


// ---- src/BaseObject/GetRefCount.cpp

long BaseObject::GetRefCount() { return m_refCount; }


// ---- src/GameObject/do_global_dtors.cpp
// MSL C++ runtime startup/shutdown hooks that sit right after
// GameObject::SimulationTick in retail, in retail's order: __do_global_dtors,
// mwInit, mwExit (mwcc emits functions in definition order).
extern "C" void __destroy_global_chain(void);
extern "C" void mwExit(void);

extern "C" void __do_global_dtors(void)
{
    mwExit();
}


// ---- src/GameObject/mwInit.cpp
// MSL C++ runtime startup hook (between GameObject::SimulationTick and
// __do_global_dtors in retail): run the static initializers. The four
// arguments are linker-defined symbols (retail's .symtab has them as global
// no-type symbols); retail references them without relocations, which
// tools/match.py accepts when the %hi/%lo pair encodes the symbol's address.
// This build's __initialize_cpp_rts (src/global/initialize_cpp_rts.cpp) only
// reads the first two; the exception-table bounds are passed but unused.
extern "C" {

extern char __static_init[];            // 0x445CF0: .p__sinit_* pointer table
extern char __static_init_end[];        // 0x445EFC
extern char __exception_table_start__[];// 0x44E904 (empty table)
extern char __exception_table_end__[];  // 0x44E904

void __initialize_cpp_rts(void *si, void *sie, void *ets, void *ete);

void mwInit(void)
{
    __initialize_cpp_rts(__static_init, __static_init_end,
                         __exception_table_start__, __exception_table_end__);
}

}

extern "C" void mwExit(void)
{
    __destroy_global_chain();
}
