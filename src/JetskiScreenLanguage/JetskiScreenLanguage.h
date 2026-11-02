// JetskiScreenLanguage: the boot screen that picks the game's language
// (JetskiLoadManager::Tick creates it). A GameObject, 0x14C bytes. Names
// come from the class's own methods.
#ifndef JETSKISCREENLANGUAGE_H
#define JETSKISCREENLANGUAGE_H

#include "GameObject/GameObject.h"

class JetskiLoadManager;
class KeyInfo;
class ProcessNode;
class PSXTextureMap;
class ScreenIcon;
class ScreenIconManager;

class JetskiScreenLanguage : public GameObject {
public:
    JetskiScreenLanguage(int id);
    JetskiScreenLanguage *Create(JetskiLoadManager *loadManager);
    int ChangeFlag();

    // --- virtuals in retail slot order ---
    virtual ~JetskiScreenLanguage();
    virtual int Tick(float dt);
    virtual int RenderPre3D();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *info);

    JetskiLoadManager *m_loadManager;   // +0x30
    PSXTextureMap *m_background;        // +0x34
    ScreenIconManager *m_iconManager;   // +0x38
    ScreenIcon *m_icon;                 // +0x3C
    char m_flagsPath[0x104];            // +0x40, also the DataAccess device name
    int m_language;                     // +0x144, 1-5
    int m_selected;                     // +0x148, set when a language is chosen
};

#endif
