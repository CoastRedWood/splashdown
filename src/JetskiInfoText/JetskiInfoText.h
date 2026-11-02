// JetskiInfoText: the in-race message line per player ("LAP 2", stunt
// names), each player with a ring of queued messages. 0x70 bytes (new in
// JetskiCompetition::LoadOverlays). Members from the ctor, Create (+0x30 the
// text manager and +0x40 the viewport it is given, +0x3C a font, +0x34 the
// two ScreenTexts, +0x54 the message rings) and AddInfoText/NextMessage.
#ifndef JETSKIINFOTEXT_JETSKIINFOTEXT_H
#define JETSKIINFOTEXT_JETSKIINFOTEXT_H

#include "GameObject/GameObject.h"
#include "ViewPortCode/ViewPortCode.h"

class ScreenTextManager;

class ScreenText;
class ScreenTextFont;
class ScreenTextManager;

struct InfoMessage {
    char m_text[0x40];                  // +0x00
    float m_duration;                   // +0x40, 0 = until replaced
    float m_time;                       // +0x44
    InfoMessage *m_next;                // +0x48
};

class JetskiInfoText : public GameObject {
public:
    JetskiInfoText(int enabled);
    JetskiInfoText *Create(ScreenTextManager *texts, ViewPortCode viewport);
    void AddInfoText(char *text, int player, float duration, int flush, float x6C);
    void FlushQueue(int player);
    void NextMessage(int player);

    // --- overrides in retail slot order ---
    virtual ~JetskiInfoText();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int Render3D();

    ScreenTextManager *m_textManager;   // +0x30
    ScreenText *m_texts[2];             // +0x34, per player
    ScreenTextFont *m_font;             // +0x3C
    int m_viewport;                     // +0x40, a ViewPortCode
    int m_x44[2];                       // +0x44
    int m_x4C[2];                       // +0x4C
    InfoMessage *m_messages[2];         // +0x54, ring of 3 per player
    InfoMessage *m_current[2];          // +0x5C, per player
    int m_numQueued[2];                 // +0x64, per player
    float m_x6C;                        // +0x6C
};

#endif
