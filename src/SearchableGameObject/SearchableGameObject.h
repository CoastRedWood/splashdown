// SearchableGameObject: a GameObject the search tree can place
// (GameObject + SearchableObject at +0x30; no data of its own, so 0x38
// bytes, rounded to GameObject's 8-byte alignment). Water, DrawableCube,
// EcoSystem and BuoyObject derive from it. Its ctor sets SearchableObject's
// m_typeID and bits.
//
// Water.h, DrawableCube.h and EcoSystem.h carry the same definition under
// the same guard, so any of them can come first.
#ifndef SEARCHABLEGAMEOBJECT_DEFINED
#define SEARCHABLEGAMEOBJECT_DEFINED

#include "GameObject/GameObject.h"
#include "SearchableObject/SearchableObject.h"

class SearchableGameObject : public GameObject, public SearchableObject {
public:
    SearchableGameObject(int enabled);
    virtual ~SearchableGameObject() {}   // inline: derived dtors inline it (retail's weak copy is at 0x2289F0)
};

#endif
