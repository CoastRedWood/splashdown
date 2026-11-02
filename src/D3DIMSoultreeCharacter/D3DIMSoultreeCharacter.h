// D3DIMSoultreeCharacter: a Character (which virtually inherits GameObject)
// for D3DIM/Soultree models. Character's part is 0x1A8 bytes (vptr at
// +0x1A4); this class's data runs +0x1A8..+0x210, then the virtual GameObject
// base (`new D3DIMSoultreeCharacter` is 0x248). Create is a new virtual;
// Enable/Disable override GameObject's (a virtual base), so they also get
// slots in the primary vtable (retail vtable).
#ifndef D3DIMSOULTREECHARACTER_H
#define D3DIMSOULTREECHARACTER_H

#include "Character/Character.h"

class D3DIMSoultreeCharacter : public Character {
public:
    D3DIMSoultreeCharacter(int construct);

    // --- virtuals in retail slot order ---
    virtual ~D3DIMSoultreeCharacter();
    virtual void LoadParts();
    virtual void Clone(Character *other);
    virtual GameObject *Create(char *name, char *ext, int a, int flags);
    virtual void Enable();
    virtual void Disable();

    char m_name[0x50];                  // +0x1A8 (Clone strcpy's it)
    int m_1F8;                          // +0x1F8
    int m_1FC;                          // +0x1FC
    int m_200;                          // +0x200
    int m_204;                          // +0x204
    int m_useContentDirectory;          // +0x208 (Create, LoadParts)
    int m_20C;                          // +0x20C (Create)
};

#endif
