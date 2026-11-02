// JetskiRaceOverMenuCareer: the end-of-race menu in career mode (results,
// qualifying, circuit standings), a JetskiMenu. 0x148 bytes (new in
// JetskiCareerCompetition's overlays); its data starts at JetskiMenu's 0xF8.
// Members from Create (the text manager, ScreenTexts, three fonts, the
// JetskiScreenText headers and the circuit table) and the ctor (clears
// +0xF8..+0x138, m_circuitInfoDirty 1, m_attractDelay 5).
#ifndef JETSKIRACEOVERMENUCAREER_H
#define JETSKIRACEOVERMENUCAREER_H

#include "JetskiMenu/JetskiMenu.h"

class JetskiScreenText;
class ScreenText;
class ScreenTextFont;
class ScreenTextManager;

class JetskiRaceOverMenuCareer : public JetskiMenu {
public:
    JetskiRaceOverMenuCareer(int enabled);
    JetskiRaceOverMenuCareer *Create(ScreenIconManager *icons, ScreenTextManager *texts, ViewPortCode viewport);
    void RenderQualifier();
    void RenderCircuit();
    void RenderReQual();

    // --- overrides in retail slot order ---
    virtual ~JetskiRaceOverMenuCareer();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int Render3D();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual void EnableMenu();
    virtual void DisableMenu();
    virtual void SetupInfo();

    ScreenTextManager *m_textManager;   // +0xF8
    ScreenText **m_rows;                // +0xFC, 6
    ScreenText *m_title;                // +0x100
    ScreenText *m_titleShadow;          // +0x104
    ScreenText *m_x108;                 // +0x108
    ScreenText *m_x10C;                 // +0x10C
    ScreenText *m_prompts[2];           // +0x110 (Create's loop)
    ScreenText *m_lines[3];             // +0x118
    ScreenTextFont *m_titleFont;        // +0x124
    ScreenTextFont *m_font;             // +0x128
    ScreenTextFont *m_smallFont;        // +0x12C
    JetskiScreenText **m_headers;       // +0x130, 4
    ScreenText **m_columns;             // +0x134, 5
    ScreenText **m_cells;               // +0x138, 25
    int m_circuitInfoDirty;             // +0x13C, 1 from the ctor
    float m_idleTime;                   // +0x140
    float m_attractDelay;               // +0x144, 5 from the ctor
};

#endif
