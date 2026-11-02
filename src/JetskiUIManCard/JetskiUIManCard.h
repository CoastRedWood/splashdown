// JetskiUIManCard: the memory card front end's JetskiUIManager. 0x2E0 bytes
// (new in JetskiUIGlobalManager::CreateUIManager); the ctor only clears
// +0x2D0. Nothing matched uses +0x2D4..+0x2E0.
#ifndef JETSKIUIMANCARD_JETSKIUIMANCARD_H
#define JETSKIUIMANCARD_JETSKIUIMANCARD_H

#include "JetskiUIManager/JetskiUIManager.h"

class JetskiUIManCard : public JetskiUIManager {
public:
    JetskiUIManCard(int enabled);
    JetskiUIManCard *Create(int page, UIGlobalManager *global);

    // --- overrides in retail slot order ---
    virtual ~JetskiUIManCard();
    virtual void UIPrepareClose();
    virtual UIPage *GetPage(int number, char *file);
    virtual int Localize();

    int m_x2D0;                         // +0x2D0
    char pad2D4[0x2E0 - 0x2D4];
};

#endif
