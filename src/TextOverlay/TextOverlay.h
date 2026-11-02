// TextOverlay: an Overlay that draws a ScreenText, with fades and moves over
// time (DisplayText). 0x2C0 bytes (new in its users); its data starts at
// Overlay's 0x210. Members from the ctor (the ScreenText it news, the
// start/target/end alpha and position copies of the text's, scales 1) and
// Create (font, camera, text manager).
#ifndef TEXTOVERLAY_H
#define TEXTOVERLAY_H

#include "Overlay/Overlay.h"

class Camera;
class ScreenText;
class ScreenTextFont;
class ScreenTextManager;

class TextOverlay : public Overlay {
public:
    TextOverlay(int enabled, int maxLength);
    TextOverlay *Create(ScreenTextManager *manager, char **fontNames, Camera *camera, int numFonts);
    void SetText(char *text);
    void SetColor(unsigned int color);
    void DisplayText(float a, float b, int alpha, float x, float y, int busy, float c,
                     int alpha2, float x2, float y2, float d, float e, float f);
    void RemoveText();
    void SetAlignment(int justification);
    void SetPosition(int x, int y, float depth);
    void SetAlpha(int alpha, float fadeTime);
    void SetAlphaEnabled(int enabled);
    void SetDepthEnabled(int enabled);
    void SetDepth(float depth);

    // --- overrides in retail slot order ---
    virtual ~TextOverlay();
    virtual int Tick(float dt);
    virtual int Render3D();

    ScreenText *m_screenText;           // +0x210
    ScreenTextFont *m_font;             // +0x214
    _RECT m_rect;                       // +0x218
    _RECT m_startRect;                  // +0x228
    _RECT m_targetRect;                 // +0x238
    float m_moveElapsed;                // +0x248
    float m_moveTime;                   // +0x24C
    int m_x250;                         // +0x250
    int m_justification;                // +0x254
    float m_x258;                       // +0x258
    float m_x25C;                       // +0x25C
    int m_x260;                         // +0x260, 1 from the ctor
    float m_x264;                       // +0x264, -1 from the ctor
    int m_x268;                         // +0x268
    Camera *m_camera;                   // +0x26C
    int m_startAlpha;                   // +0x270
    int m_targetAlpha;                  // +0x274
    float m_fadeElapsed;                // +0x278
    float m_fadeTime;                   // +0x27C
    float m_startX;                     // +0x280
    float m_x;                          // +0x284
    float m_xElapsed;                   // +0x288
    float m_xTime;                      // +0x28C
    float m_y;                          // +0x290
    float m_startY;                     // +0x294
    float m_yElapsed;                   // +0x298
    float m_yTime;                      // +0x29C
    float m_scaleX;                     // +0x2A0
    float m_scaleY;                     // +0x2A4
    int m_busy;                         // +0x2A8, setters are ignored while nonzero
    float m_x2AC;                       // +0x2AC
    int m_endAlpha;                     // +0x2B0
    float m_endX;                       // +0x2B4
    float m_endY;                       // +0x2B8
    ScreenTextManager *m_textManager;   // +0x2BC
};

#endif
