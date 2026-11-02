// JetskiUIManager: Splashdown's UIManager. It loads the front end's 3D scene
// (lights, fog, camera, water) and its pages and widgets; JetskiUIManMain
// (0x310 bytes) and JetskiUIManCard (0x2E0) derive from it. Data from the
// ctor (src/JetskiUIManager/ctor.cpp, which zeroes +0x190..+0x1B4 and
// +0x2BC..+0x2CC) and its own methods; other names are the most common among
// the files that declared it locally, including the pages' "JetskiUIState"
// copies (mwcc's layout JetskiUIState). 0x2D0 bytes.
#ifndef JETSKIUIMANAGER_JETSKIUIMANAGER_H
#define JETSKIUIMANAGER_JETSKIUIMANAGER_H

#include "UIManager/UIManager.h"

class Fog;
class LightData;
class PSXSound;
class PSXSoundGroup;
class PSXUIWaterEffect;
class ResponseEmitter;
class ScreenPrimitiveManager;
class SearchList;
class SearchNode;
class UICamera;
struct _D3DCOLORVALUE;

class JetskiUIManager : public UIManager {
public:
    JetskiUIManager(int enabled);
    int LoadScene();
    int ReadLightData(int index);
    int LoadFog();
    int ReadFogData();
    int LoadCamera();
    int AttachScene();
    UIWidget *GetWidget(ParameterBlock *block, int type);
    int ReadColorRGB(_D3DCOLORVALUE *color, char *block, char *name, char *defaultValue);
    int ReadColorRGB(int *color, char *block, char *name, char *defaultValue);
    void ReportError(char *a, char *b, char *c);
    void ReportError(char *message);
    void ReportSuccess(char *a, char *b, char *c);

    // --- virtuals in retail slot order ---
    virtual ~JetskiUIManager();
    virtual int Tick(float dt);
    virtual int LoadPages();
    virtual int LoadWidgets();
    virtual JetskiUIManager *Create(char *name, int page, UIGlobalManager *global, int water);

    char pad188[0x190 - 0x188];
    ResponseEmitter *m_responses;           // +0x190, the UI sounds
    PSXSound *m_tuneSound;                  // +0x194 (JetskiUIManMain::Create)
    PSXSound *m_negativeSound;              // +0x198
    PSXSound *m_heliSound;                  // +0x19C
    PSXSoundGroup *m_musicGroup;            // +0x1A0
    PSXSoundGroup *m_riderGroup;            // +0x1A4
    PSXSoundGroup *m_soundGroup;            // +0x1A8
    SearchList *m_searchList;               // +0x1AC
    SearchNode *m_searchNode;               // +0x1B0
    ParameterBlock *m_params;               // +0x1B4, only while loading
    int m_page;                             // +0x1B8
    char m_name[0x100];                     // +0x1BC
    LightData *m_lights;                    // +0x2BC, new LightData[n]
    Fog *m_fog;                             // +0x2C0
    UICamera *m_camera;                     // +0x2C4
    ScreenPrimitiveManager *m_primitives;   // +0x2C8
    PSXUIWaterEffect *m_waterEffect;        // +0x2CC
};

#endif
