// UIWidgetEventHandler.cpp  @ 0x00318C00 (text 0x318C00-0x3193B0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// UIWidgetEventHandler as one translation unit without a static initializer,
// between UIManager.cpp and UIWidgets.cpp in UIPage.cpp's tu_map row. Its
// .rodata is only its RTTI block, @690 .. the UIWidgetEventHandler RTTI, with
// its own copies of the BaseObject/GameObject records (0x431C80-0x431D00);
// UIWidgets.cpp's labels restart at @909 after it, and its vtable (0x449F20)
// comes before every UIWidgets.cpp vtable. The name is a guess.
// UIPage::ProcessUserInput sits between KeyDownHandler and KeyDownProcessor, as
// in retail.


#include "GameObject/GameObject.h"
#include "UIWidget/UIWidget.h"
#include "UIWidgetEventHandler/UIWidgetEventHandler.h"
#include "UIPage/UIPage.h"
#include "UIManager/UIManager.h"
#include "PSXRenderTarget/PSXRenderTarget.h"
#include "ProcessNode/ProcessNode.h"

// ---- src/UIWidgetEventHandler/ctor.cpp

UIWidgetEventHandler::UIWidgetEventHandler(int enabled)
    : GameObject(enabled)
{
    m_eventLists = 0;
}


// ---- src/UIWidgetEventHandler/dtor.cpp

UIWidgetEventHandler::~UIWidgetEventHandler()
{
    UIWidgetEventList *list;
    UIWidgetEventNode *node;
    for (int i = 0; i < 8; i++) {
        list = m_eventLists[i];
        node = list->m_head;
        while (node) {
            UIWidgetEventNode *dead = node;
            node = node->m_next;
            delete dead;
        }
        delete list;
    }
    delete[] m_eventLists;
    m_eventLists = 0;
}


// ---- src/UIWidgetEventHandler/Create.cpp


UIWidgetEventHandler *UIWidgetEventHandler::Create(UIPage *page, UIManager *manager)
{
    m_page = page;
    m_manager = manager;
    m_eventLists = new UIWidgetEventList *[8];
    for (int i = 0; i < 8; i++) {
        m_eventLists[i] = new UIWidgetEventList;
        m_eventLists[i]->m_tail = 0;
        m_eventLists[i]->m_head = 0;
    }
    return this;
}


// ---- src/UIWidgetEventHandler/AddToJoystickEventList.cpp

// Append the widget to the list of every joystick event it wants.
void UIWidgetEventHandler::AddToJoystickEventList(UIWidget *widget)
{
    UIWidgetEventList *list;
    for (int i = 0; i < 8; i++) {
        if (widget->m_joystickEvents[i]) {
            list = m_eventLists[i];
            if (!list->m_head) {
                list->m_head = new UIWidgetEventNode;
                UIWidgetEventNode *node = list->m_head;
                node->m_widget = widget;
                node->m_next = 0;
                node->m_send = 0;
                list->m_tail = node;
            } else {
                list->m_tail->m_next = new UIWidgetEventNode;
                list->m_tail = list->m_tail->m_next;
                list->m_tail->m_widget = widget;
                list->m_tail->m_send = 0;
                list->m_tail->m_next = 0;
            }
        }
    }
}


// ---- src/UIWidgetEventHandler/KeyDownHandler.cpp


int UIWidgetEventHandler::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    int event = 0;
    int player = 0;
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    if (m_manager->m_state)
        return GameObject::KeyDownHandler(node, key);
    if (KeyDownProcessor(node, key, &event, &player) && m_manager->m_currentPage->ProcessUserInput(event, player)) {
        UIWidgetEventList *list = m_eventLists[event];
        UIWidgetEventNode *n;
        for (n = list->m_head; n; n = n->m_next) {
            if (n->m_widget->m_selected)
                n->m_send = 1;
            else
                n->m_send = 0;
        }
        for (n = list->m_head; n; n = n->m_next) {
            if (n->m_send)
                n->m_widget->WidgetKeyDownHandler(node, key, event, player);
        }
    }
    return GameObject::KeyDownHandler(node, key);
}


// ---- src/UIPage/ProcessUserInput.cpp

int UIPage::ProcessUserInput(int player, int input)
{
    return 1;
}


// ---- src/UIWidgetEventHandler/KeyDownProcessor.cpp


int KEYDOWN_HANDLER_TEST(AssignableActionIndex action, DeviceType device, ProcessNode *node, unsigned int mods);


// Maps a joypad key-down to (player, action 0-15); returns 0 if none matched.
int UIWidgetEventHandler::KeyDownProcessor(ProcessNode *node, KeyInfo *key, int *player, int *action)
{
    int handled = 1;
    *player = 0;
    *action = 0;
    if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)0, (DeviceType)2, node, 0x80000000)) {
        *player = node->m_controller;
        *action = 0;
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)1, (DeviceType)2, node, 0x80000000)) {
        *player = node->m_controller;
        *action = 1;
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)2, (DeviceType)2, node, 0x80000000)) {
        *player = node->m_controller;
        *action = 2;
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)3, (DeviceType)2, node, 0x80000000)) {
        *player = node->m_controller;
        *action = 3;
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)4, (DeviceType)2, node, 0x80000000)) {
        *player = node->m_controller;
        *action = 4;
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)5, (DeviceType)2, node, 0x80000000)) {
        *player = node->m_controller;
        *action = 5;
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)6, (DeviceType)2, node, 0x80000000)) {
        *player = node->m_controller;
        *action = 6;
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)7, (DeviceType)2, node, 0x80000000)) {
        *player = node->m_controller;
        *action = 7;
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)8, (DeviceType)2, node, 0x80000000)) {
        *player = node->m_controller;
        *action = 8;
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)9, (DeviceType)2, node, 0x80000000)) {
        *player = node->m_controller;
        *action = 9;
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)10, (DeviceType)2, node, 0x80000000)) {
        *player = node->m_controller;
        *action = 10;
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)11, (DeviceType)2, node, 0x80000000)) {
        *player = node->m_controller;
        *action = 11;
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)12, (DeviceType)2, node, 0x80000000)) {
        *player = node->m_controller;
        *action = 12;
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)13, (DeviceType)2, node, 0x80000000)) {
        *player = node->m_controller;
        *action = 13;
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)14, (DeviceType)2, node, 0x80000000)) {
        *player = node->m_controller;
        *action = 14;
    } else if (KEYDOWN_HANDLER_TEST((AssignableActionIndex)15, (DeviceType)2, node, 0x80000000)) {
        *player = node->m_controller;
        *action = 15;
    } else {
        handled = 0;
    }
    return handled;
}
