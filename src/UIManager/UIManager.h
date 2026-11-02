// UIManager: owns a front end's pages and widgets and runs the page
// transitions (Tick). Data from the ctor (src/UIManager/ctor.cpp) and
// UIManager's own methods; other names are the most common among the files
// that declared UIManager locally (mwcc's layout UIManager). 0x188
// bytes; JetskiUIManager adds members from +0x188.
#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "GameObject/GameObject.h"

class ParameterBlock;
class ScreenIconManager;
class ScreenTextManager;
class UIGlobalManager;
class UIPage;
class UIWidget;

// Built by its out-of-line ctor (0x317B80, src/tu/UIManager.cpp),
// which UIManager's ctor runs through __construct_array.
struct UICameraInfo {
    UICameraInfo();
    float m_position[4];                    // +0x00, w = 1
    char pad10[0x20 - 0x10];
    float m_target[4];                      // +0x20, w = 1
    float m_up[4];                          // +0x30, w = 1
};

class UIManager : public GameObject {
public:
    UIManager(int enabled);

    // --- virtuals in retail slot order ---
    virtual ~UIManager();
    virtual int Tick(float dt);
    virtual UIManager *Create(UIGlobalManager *global);
    virtual void SetCurrentPage(int page);
    virtual int SwitchPage(int page);
    virtual void UIPrepareClose();
    virtual void PlaySound(int sound, int a, unsigned int b, int c);
    virtual int LoadPages();
    virtual int AttachPages();
    virtual UIPage *GetPage(int number, char *file) = 0;  // pure; JetskiUIManMain/Card::GetPage
    virtual int Localize();
    virtual int LoadWidgets();
    virtual UIWidget *GetWidget(ParameterBlock *block, char *type);
    virtual int AttachWidgets();
    virtual void LoadRequestedMaterials();

    UIGlobalManager *m_global;              // +0x030
    ScreenTextManager *m_textManager;       // +0x034
    int m_x38;                              // +0x038
    char pad3C[0x40 - 0x3C];
    UICameraInfo m_cameras[4];              // +0x040
    ScreenIconManager *m_iconManager;       // +0x140
    int m_x144;                             // +0x144
    UIPage **m_pages;                       // +0x148
    int m_numPages;                         // +0x14C
    UIPage *m_currentPage;                  // +0x150
    int m_currentIndex;                     // +0x154, 1-based
    UIWidget **m_widgets;                   // +0x158
    int m_numWidgets;                       // +0x15C
    int m_closing;                          // +0x160, Tick calls UIPrepareClose
    int m_x164;                             // +0x164
    int m_x168;                             // +0x168
    int m_nextPage;                         // +0x16C
    int m_state;                            // +0x170, Tick's transition state
    int m_direction;                        // +0x174, 1 in, 2 out
    int m_x178;                             // +0x178
    char pad17C[0x180 - 0x17C];
    int m_x180;                             // +0x180, 1 from the ctor
};

#endif
