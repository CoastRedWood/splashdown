// UIWidgetEventHandler — per-button lists of widgets that want joystick
// events (UIWidgets.cpp). UIPage::Create allocates 0x40 bytes.
#ifndef UIWIDGETEVENTHANDLER_H
#define UIWIDGETEVENTHANDLER_H

#include "GameObject/GameObject.h"
#include "UIWidget/UIWidget.h"

class UIPage;
class UIManager;

struct UIWidgetEventNode {
    UIWidget *m_widget;                 // +0x0
    int m_send;                         // +0x4, set while KeyDownHandler dispatches
    UIWidgetEventNode *m_next;          // +0x8
};

struct UIWidgetEventList {
    UIWidgetEventNode *m_head;          // +0x0
    UIWidgetEventNode *m_tail;          // +0x4
};

class UIWidgetEventHandler : public GameObject {
public:
    UIWidgetEventHandler(int enabled);
    UIWidgetEventHandler *Create(UIPage *page, UIManager *manager);
    void AddToJoystickEventList(UIWidget *widget);
    int KeyDownProcessor(ProcessNode *node, KeyInfo *key, int *event, int *player);

    // --- virtuals in retail slot order ---
    virtual ~UIWidgetEventHandler();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);

    UIManager *m_manager;               // +0x30
    UIPage *m_page;                     // +0x34
    UIWidgetEventList **m_eventLists;   // +0x38, 8 entries
    char pad3C[0x40 - 0x3C];
};

#endif
