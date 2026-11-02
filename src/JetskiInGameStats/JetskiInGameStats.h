// JetskiInGameStats — the in-game HUD (lap, time, position, ...). A
// GameObject, 0xE8 bytes (`new` in JetskiCompetition::LoadOverlays). Names
// come from the class's own methods.
#ifndef JETSKIINGAMESTATS_H
#define JETSKIINGAMESTATS_H

#include "GameObject/GameObject.h"
#include "ViewPortCode/ViewPortCode.h"

class JetskiCompetition;
class ScreenIconManager;
class ScreenIcon;
class ScreenText;
class ScreenTextFont;
class ScreenTextManager;

class JetskiInGameStats : public GameObject {
public:
    JetskiInGameStats(int enabled);
    int SetCompetition(JetskiCompetition *competition);
    void SetShowing(int lapTimes, int positions);
    void SetTotalRaceTime(int on);
    void SetRecordLapTime(int on);
    void SetBestLap(int on);
    void SetLapTime(int on);
    void SetTimeBehindLeader(int on);
    void SetPosition(int on);
    void SetLap(int on);
    void SetCountdownText(int on);
    void SetScoreText(int on);
    void SetNewRecordText(int on);
    void SetReplayText(int on);
    void SetRaceTypeText(int on);
    void SetWrongWayText(int on);
    void SetWings(int left, int right);
    JetskiInGameStats *Create(ScreenIconManager *icons, ScreenTextManager *texts, ViewPortCode viewport, int x78,
                              int leftWing, int rightWing);
    void RenderIcons();
    void RenderBestLap();
    void RenderCountdownText();
    void RenderLap();
    void RenderLapTime();
    void RenderPosition();
    void RenderRaceTypeText();
    void RenderRecordLapTime();
    void RenderReplayText();
    void RenderScoreText();
    void RenderSecretFoundText();
    void RenderTimeBehindLeader();
    void RenderWrongWayText();

    // --- virtuals in retail slot order ---
    virtual ~JetskiInGameStats();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int Render3D();

    JetskiCompetition *m_competition;   // +0x30
    ScreenIconManager *m_iconManager;   // +0x34
    ScreenIcon **m_icons;               // +0x38
    ScreenTextManager *m_textManager;   // +0x3C
    ScreenText **m_texts;               // +0x40, the HUD texts (dtor deletes each)
    ScreenText *m_recordLabel;          // +0x44
    ScreenText *m_bestLapLabel;         // +0x48
    ScreenText *m_lapTimeLabel;         // +0x4C
    ScreenText *m_lapLabel;             // +0x50
    ScreenText *m_timeBehindLabel;      // +0x54
    ScreenText *m_positionSuffix;       // +0x58
    ScreenText *m_rankTexts[4];         // +0x5C
    ScreenTextFont *m_font;             // +0x6C
    ScreenTextFont *m_font70;           // +0x70, countdown/position font
    int m_twoPlayers;                   // +0x74, the viewport code
    // +0x78 is an int Create sets (its x78 argument). It stays padding: any
    // edit to this line except a constant-expression array bound reschedules
    // JetskiCompetition::SetPlayback (mwcc state, not layout).
    char pad78[0x7C - 0x78];
    int m_showing[2];                   // +0x7C, per player
    int m_showTotalRaceTime;            // +0x84
    int m_showRecordLapTime;            // +0x88
    int m_showBestLap;                  // +0x8C
    int m_showLapTime;                  // +0x90
    int m_showTimeBehindLeader;         // +0x94
    int m_showPosition;                 // +0x98
    int m_showLap;                      // +0x9C
    int m_showScoreText;                // +0xA0
    int m_showCountdownText;            // +0xA4
    int m_showLeftWing;                 // +0xA8
    int m_showRightWing;                // +0xAC
    int m_showWrongWayText;             // +0xB0
    int m_showNewRecordText;            // +0xB4
    int m_showReplayText;               // +0xB8
    int m_showRaceTypeText;             // +0xBC
    int m_wasAhead;                     // +0xC0
    float m_secretTime[2];              // +0xC4, per player
    float m_wrongWayTime[2];            // +0xCC, per player
    float m_lastCountdown;              // +0xD4
    int m_lastPosition;                 // +0xD8
    int m_lastLap[2];                   // +0xDC, per player
    char padE4[0xE8 - 0xE4];
};

#endif
