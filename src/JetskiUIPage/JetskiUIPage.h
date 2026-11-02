// JetskiUIPage: Splashdown's UIPage, the base of the JetskiUIPage* and
// JetskiMP* pages. It adds no virtuals (retail vtable) and one
// member, zeroed by the ctor and used as the transition timer by the
// subclasses' ProcessTransition. 0x168 bytes.
#ifndef JETSKIUIPAGE_JETSKIUIPAGE_H
#define JETSKIUIPAGE_JETSKIUIPAGE_H

#include "UIPage/UIPage.h"

class JetskiUIPage : public UIPage {
public:
    JetskiUIPage(int enabled);
    void ProcessInputSounds(int player, int input);
    void PlayPageSound(int sound);

    virtual ~JetskiUIPage();
    virtual int Tick(float dt);

    float m_fadeTime;                       // +0x160
};

#endif
