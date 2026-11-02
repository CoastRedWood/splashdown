// UIWidgetOverlayText: a UI widget showing one or more lines of text, one
// TextOverlay per line (UIWidgets.cpp). 0x110 bytes (`new` in
// UIManager::GetWidget).
#ifndef UIWIDGETOVERLAYTEXT_H
#define UIWIDGETOVERLAYTEXT_H

extern "C" void free(void *);

#include "UIWidget/UIWidget.h"
#include "TextOverlay/TextOverlay.h"

class ScreenTextManager;

class UIWidgetOverlayText : public UIWidget {
public:
    UIWidgetOverlayText(UIManager *manager, int enabled);
    UIWidgetOverlayText *Create(int id, ParameterBlock *params, ScreenTextManager *manager);
    void ParseMultiLine(char *text, ScreenTextManager *manager);

    // --- virtuals in retail slot order ---
    virtual ~UIWidgetOverlayText();
    virtual void WidgetSelect();
    virtual void WidgetDeSelect();
    virtual void WidgetDisable();
    virtual void WidgetKeepEnabled();

    ScreenTextManager *m_textManager; // +0x58
    TextOverlay **m_overlays;       // +0x5C
    int m_x60;                      // +0x60
    char m_fontName[0x80];          // +0x64
    int m_numOverlays;              // +0xE4
    int m_justification;            // +0xE8
    unsigned int m_color;           // +0xEC
    int m_x;                        // +0xF0
    int m_y;                        // +0xF4
    int m_lineHeight;               // +0xF8
    int m_keepVisible;              // +0xFC
    unsigned int m_selectedColor;   // +0x100
    int m_visibleWhenDisabled;      // +0x104
    unsigned int m_disabledColor;   // +0x108
    int m_x10C;                     // +0x10C
};

#endif
