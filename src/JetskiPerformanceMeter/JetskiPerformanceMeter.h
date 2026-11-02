// JetskiPerformanceMeter: the in-race speedometer, performance gauge and
// trick hold meter. 0xA8 bytes (new in JetskiCompetition::LoadOverlays).
// Members from the ctor, Create (unmatched: +0x34 the icon manager, +0x3C
// the text manager and +0x60 the viewport it is given, +0x4C/+0x50/+0x54
// three ScreenTextFontItemList fonts, +0x58 -1), SetCompetition (+0x30),
// SetSpeedometer (+0x8C) and SetPerformance (+0x90).
#ifndef JETSKIPERFORMANCEMETER_H
#define JETSKIPERFORMANCEMETER_H

#include "GameObject/GameObject.h"
#include "ViewPortCode/ViewPortCode.h"

class ScreenIconManager;
class ScreenTextManager;

class JetskiCompetition;
class ScreenIcon;
class ScreenIconManager;
class ScreenText;
class ScreenTextFont;
class ScreenTextManager;

class JetskiPerformanceMeter : public GameObject {
public:
    JetskiPerformanceMeter(int enabled);
    JetskiPerformanceMeter *Create(ScreenIconManager *icons, ScreenTextManager *texts, ViewPortCode viewport);
    int SetCompetition(JetskiCompetition *competition);
    void SetShowing(int player0, int player1);
    void SetSpeedometer(int show);
    void SetPerformance(int show);
    void SetupInfo(int player);
    void SetupHoldMeter();
    void RenderSpeed();
    void RenderPerformance();
    void RenderHoldMeter();
    char *StuntsName(int stunt, int character);

    // --- overrides in retail slot order ---
    virtual ~JetskiPerformanceMeter();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int Render3D();

    JetskiCompetition *m_competition;   // +0x30
    ScreenIconManager *m_iconManager;   // +0x34
    ScreenIcon **m_items;               // +0x38, 11 entries
    ScreenTextManager *m_textManager;   // +0x3C
    ScreenText **m_labels;              // +0x40, 2 entries
    ScreenText *m_text44;               // +0x44
    ScreenText *m_text48;               // +0x48
    ScreenTextFont *m_font4C;           // +0x4C
    ScreenTextFont *m_font50;           // +0x50
    ScreenTextFont *m_font54;           // +0x54
    int m_x58;                          // +0x58, -1 from Create
    int m_x5C;                          // +0x5C
    int m_viewport;                     // +0x60, a ViewPortCode
    float m_maxSpeed;                   // +0x64
    float m_speed;                      // +0x68
    float m_x6C;                        // +0x6C
    float m_x70;                        // +0x70
    float m_x74;                        // +0x74 (m_x6C - m_x70)
    float m_x78;                        // +0x78
    float m_x7C;                        // +0x7C
    float m_x80;                        // +0x80 (m_x7C - m_x78)
    int m_enabled[2];                   // +0x84, per player
    int m_showSpeed;                    // +0x8C
    int m_showPerformance;              // +0x90
    int m_holding;                      // +0x94
    float m_multiplier;                 // +0x98
    float m_holdFraction;               // +0x9C
    int m_xA0;                          // +0xA0
};

#endif
