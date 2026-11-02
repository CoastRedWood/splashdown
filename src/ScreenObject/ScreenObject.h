// ScreenObject — owns the icon, primitive and text managers of a 2D overlay.
#ifndef SCREENOBJECT_SCREENOBJECT_H
#define SCREENOBJECT_SCREENOBJECT_H

#include "GameObject/GameObject.h"

class ScreenTextFont;

#include "ScreenIconManager/ScreenIconManager.h"

#include "ScreenPrimitiveManager/ScreenPrimitiveManager.h"

#include "ScreenTextManager/ScreenTextManager.h"

class ScreenObject : public GameObject {
public:
    ScreenObject(int enabled);
    virtual ~ScreenObject();
    virtual int Render3D();
    ScreenObject *Create(int maxIcons, int maxPrimitives, int maxTexts);

    ScreenIconManager *m_iconManager;            // +0x30
    ScreenTextManager *m_textManager;            // +0x34
    ScreenPrimitiveManager *m_primitiveManager;  // +0x38
    int m_renderOrder[3];                        // +0x3C, indices into m_managers
    GameObject *m_managers[3];                   // +0x48: icons, primitives, text
};

#endif
