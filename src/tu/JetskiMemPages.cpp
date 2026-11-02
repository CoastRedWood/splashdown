// JetskiMemPages.cpp  @ 0x0038B6D0 (text 0x38B6D0-0x38F120)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// The JetskiMP* memory-card pages (JetskiMPInitNo .. JetskiMPSaveFormat) as one
// translation unit (its static initializer only builds the axis vectors),
// unanchored inside JetskiVCRController.cpp's tu_map row. The page classes are
// in JetskiUIPage/JetskiMemPages.h. The JetskiMPInit* pages belong to
// JetskiUIManCard (+0x2D0 the card state), the Load/Save pages to
// JetskiUIManMain (+0x300 the save mode). TrainingTriggerCallBack, at 0x38F120
// after the last page, opens JetskiTraining.cpp.



#include "JetskiUIPage/JetskiUIPage.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "JetskiGame/JetskiGame.h"
#include "JetskiUIManager/JetskiUIManager.h"
#include "RenderTarget/RenderTarget.h"
#include "JetskiLoadManager/JetskiLoadManager.h"
#include "TextureMap/TextureMap.h"
#include "GameObject/GameObject.h"
#include "PSXJoystickDevice/PSXJoystickDevice.h"
#include "JetskiUIPage/JetskiMemPages.h"
#include "JetskiUIManMain/JetskiUIManMain.h"
#include "JetskiUIManCard/JetskiUIManCard.h"
#include "ControlInterface/ControlInterface.h"
#include "JoystickDevice/JoystickDevice.h"
#include "PSXUIWaterEffect/PSXUIWaterEffect.h"
#include "JetskiGame/JetskiGame.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "sinit/AxisVectors.h"

extern JetskiGame *LpGame;

// ---- src/JetskiMPInitNo/ctor.cpp

JetskiMPInitNo::JetskiMPInitNo(int enabled)
    : JetskiUIPage(enabled)
{
    m_timer = 0.0f;
}


// ---- src/JetskiMPInitNo/dtor.cpp


JetskiMPInitNo::~JetskiMPInitNo()
{
}


// ---- src/JetskiMPInitNo/Tick.cpp






int JetskiMPInitNo::Tick(float dt)
{
    if (m_timer > 0.0f)
        m_timer -= dt;
    if (m_timer > 0.0f || ((JetskiUIManCard *)m_manager)->m_state) {
        return JetskiUIPage::Tick(dt);
    }
    if (LpGame->m_gameOptions->CheckForMemoryCard(0)) {
        ((JetskiUIManCard *)m_manager)->m_state = 7;
        ((JetskiUIManCard *)m_manager)->m_nextPage = 5;
    } else {
        m_timer = 0.3f;
    }
    return JetskiUIPage::Tick(dt);
}


// ---- src/JetskiMPInitNo/ResetPage.cpp


void JetskiMPInitNo::ResetPage()
{
    if (LpGame->m_controls->m_joysticks[0]->IsUsingMultiTap())
        SetPlayerFocus(1, 2, 0);
    else
        SetPlayerFocus(1, 1, 0);
}


// ---- src/JetskiMPInitNo/ProcessUserInput.cpp



int JetskiMPInitNo::ProcessUserInput(int player, int input)
{
    JetskiUIManCard *panel = ((JetskiUIManCard *)m_manager);
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            panel->m_x2D0 = 1;
            panel->m_state = 7;
            panel->m_nextPage = 5;
            PlayPageSound(2);
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiMPInitNoSpace/ctor.cpp

JetskiMPInitNoSpace::JetskiMPInitNoSpace(int enabled)
    : JetskiUIPage(enabled)
{
    m_timer = 0.0f;
}


// ---- src/JetskiMPInitNoSpace/dtor.cpp


JetskiMPInitNoSpace::~JetskiMPInitNoSpace()
{
}


// ---- src/JetskiMPInitNoSpace/Tick.cpp






int JetskiMPInitNoSpace::Tick(float dt)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    if (((JetskiUIManCard *)m_manager)->m_state) {
        return JetskiUIPage::Tick(dt);
    }
    if (m_timer < 0.25f) {
        m_timer += dt;
    } else {
        m_timer = 0.0f;
        if (!options->CheckForMemoryCard(0)) {
            ((JetskiUIManCard *)m_manager)->m_state = 7;
            ((JetskiUIManCard *)m_manager)->m_nextPage = 5;
        }
    }
    return JetskiUIPage::Tick(dt);
}


// ---- src/JetskiMPInitNoSpace/ResetPage.cpp

void JetskiMPInitNoSpace::ResetPage()
{
    m_timer = 0.0f;
}


// ---- src/JetskiMPInitNoSpace/ProcessUserInput.cpp



int JetskiMPInitNoSpace::ProcessUserInput(int player, int input)
{
    JetskiUIManCard *panel = ((JetskiUIManCard *)m_manager);
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            panel->m_x2D0 = 1;
            panel->m_state = 7;
            panel->m_nextPage = 5;
            PlayPageSound(2);
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiMPInitYes/ctor.cpp

JetskiMPInitYes::JetskiMPInitYes(int enabled)
    : JetskiUIPage(enabled)
{
    m_loaded = 0;
    m_loadFailed = 0;
    m_startLoad = 0;
    m_loadDelay = 0;
}


// ---- src/JetskiMPInitYes/dtor.cpp


JetskiMPInitYes::~JetskiMPInitYes()
{
}


// ---- src/JetskiMPInitYes/Tick.cpp

extern RenderTarget *LpRenderTarget;




int JetskiMPInitYes::Tick(float dt)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    JetskiUIManCard *state = ((JetskiUIManCard *)m_manager);
    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    if (((JetskiUIManCard *)m_manager)->m_state)
        return JetskiUIPage::Tick(dt);
    if (m_checkTimer < 0.25f) {
        m_checkTimer += dt;
    } else {
        m_checkTimer = 0;
        if (!m_loadDelay && !m_x174 && !m_cancel && !m_loaded && !m_loadFailed && !m_startLoad &&
            !options->CheckForMemoryCard(0)) {
            // The card was removed.
            ((JetskiUIManCard *)m_manager)->m_state = 7;
            ((JetskiUIManCard *)m_manager)->m_nextPage = 5;
            return JetskiUIPage::Tick(dt);
        }
    }
    if (m_cancel) {
        m_cancel = 0;
        state->m_x2D0 = 1;
        state->m_state = 7;
        state->m_nextPage = 5;
        PlayPageSound(2);
    }
    if (m_loaded) {
        m_loaded = 0;
        for (int i = 0; i < 5; i++)
            DisableGroupControl(1, i + 1);
        EnableGroupControl(1, 3);
        SetPlayerFocus(1, 3, 0);
    }
    if (m_loadFailed) {
        m_loadFailed = 0;
        for (int i = 0; i < 5; i++)
            DisableGroupControl(1, i + 1);
        EnableGroupControl(1, 4);
        SetPlayerFocus(1, 4, 0);
    }
    if (m_loadDelay > 1) {
        m_loadDelay = 0;
        m_x174 = 1;
        if (options->LoadGameProfileCardYes())
            m_loaded = 1;
        else
            m_loadFailed = 1;
    } else if (m_loadDelay > 0) {
        m_loadDelay++;
    }
    if (m_startLoad) {
        m_startLoad = 0;
        m_loadDelay = 1;
        for (int i = 0; i < 5; i++)
            DisableGroupControl(1, i + 1);
        EnableGroupControl(1, 5);
        SetPlayerFocus(1, 5, 0);
    }
    return JetskiUIPage::Tick(dt);
}


// ---- src/JetskiMPInitYes/ResetPage.cpp


void JetskiMPInitYes::ResetPage()
{
    m_loaded = 0;
    m_loadFailed = 0;
    m_startLoad = 0;
    m_x174 = 0;
    m_cancel = 0;
    m_x17C = 1;
    m_x180 = 0;
    m_checkTimer = 0;
    m_loadDelay = 0;
    if (LpGame->m_controls->m_joysticks[0]->IsUsingMultiTap())
        SetPlayerFocus(1, 2, 0);
    else
        SetPlayerFocus(1, 1, 0);
}


// ---- src/JetskiMPInitYes/ProcessUserInput.cpp




int JetskiMPInitYes::ProcessUserInput(int player, int input)
{
    JetskiUIManCard *state = ((JetskiUIManCard *)m_manager);
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            switch (GetGroupControlID(1)) {
            case 1:
            case 2:
                if (m_x17C) {
                    m_x17C = 0;
                    m_x180 = 1;
                    m_startLoad = 1;
                }
                PlayPageSound(2);
                break;
            case 3:
            case 4:
                if (m_x174 && m_x180) {
                    m_x180 = 0;
                    m_cancel = 1;
                }
                PlayPageSound(2);
                break;
            }
            break;
        case 6:
            switch (GetGroupControlID(1)) {
            case 1:
            case 2:
                state->m_x2D0 = 1;
                state->m_state = 7;
                state->m_nextPage = 5;
                PlayPageSound(2);
                break;
            }
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiMPInitFormat/ctor.cpp

JetskiMPInitFormat::JetskiMPInitFormat(int enabled)
    : JetskiUIPage(enabled)
{
    m_formatted = 0;
    m_formatFailed = 0;
    m_startFormat = 0;
    m_x174 = 0;
    m_x178 = 1;
    m_x17C = 0;
    m_checkTimer = 0.0f;
    m_formatDelay = 0;
}


// ---- src/JetskiMPInitFormat/dtor.cpp


JetskiMPInitFormat::~JetskiMPInitFormat()
{
}


// ---- src/JetskiMPInitFormat/Tick.cpp




int JetskiMPInitFormat::Tick(float dt)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    if (((JetskiUIManCard *)m_manager)->m_state)
        return JetskiUIPage::Tick(dt);
    if (m_checkTimer < 0.25f) {
        m_checkTimer += dt;
    } else {
        m_checkTimer = 0;
        if (!m_formatDelay && !m_x174 && !m_formatted && !m_formatFailed && !m_startFormat &&
            !options->CheckForMemoryCard(0)) {
            // The card was removed.
            ((JetskiUIManCard *)m_manager)->m_state = 7;
            ((JetskiUIManCard *)m_manager)->m_nextPage = 5;
            return JetskiUIPage::Tick(dt);
        }
    }
    if (m_formatted) {
        m_formatted = 0;
        for (int i = 0; i < 5; i++)
            DisableGroupControl(1, i + 1);
        EnableGroupControl(1, 3);
        SetPlayerFocus(1, 3, 0);
    }
    if (m_formatFailed) {
        m_formatFailed = 0;
        for (int i = 0; i < 5; i++)
            DisableGroupControl(1, i + 1);
        EnableGroupControl(1, 4);
        SetPlayerFocus(1, 4, 0);
    }
    if (m_formatDelay > 1) {
        m_formatDelay = 0;
        m_x174 = 1;
        if (options->FormatMemoryCard())
            m_formatted = 1;
        else
            m_formatFailed = 1;
    } else if (m_formatDelay > 0) {
        m_formatDelay++;
    }
    if (m_startFormat) {
        m_startFormat = 0;
        m_formatDelay = 1;
        for (int i = 0; i < 5; i++)
            DisableGroupControl(1, i + 1);
        EnableGroupControl(1, 5);
        SetPlayerFocus(1, 5, 0);
    }
    return JetskiUIPage::Tick(dt);
}


// ---- src/JetskiMPInitFormat/ResetPage.cpp


void JetskiMPInitFormat::ResetPage()
{
    m_formatted = 0;
    m_formatFailed = 0;
    m_startFormat = 0;
    m_x174 = 0;
    m_x178 = 1;
    m_x17C = 0;
    m_checkTimer = 0;
    m_formatDelay = 0;
    if (LpGame->m_controls->m_joysticks[0]->IsUsingMultiTap())
        SetPlayerFocus(1, 2, 0);
    else
        SetPlayerFocus(1, 1, 0);
}


// ---- src/JetskiMPInitFormat/ProcessUserInput.cpp



int JetskiMPInitFormat::ProcessUserInput(int player, int input)
{
    switch (player) {
    case 0:
        switch (GetGroupControlID(1)) {
        case 1:
        case 2:
            switch (input) {
            case 7:
                m_startFormat = 1;
                PlayPageSound(2);
                break;
            case 6:
                Done();
                PlayPageSound(2);
                break;
            }
            break;
        case 3:
        case 4:
            switch (input) {
            case 4:
                Done();
                PlayPageSound(2);
                break;
            }
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiMPInitFormat/Done.cpp

void JetskiMPInitFormat::Done()
{
    JetskiUIManCard *manager = (JetskiUIManCard *)m_manager;
    manager->m_x2D0 = 1;
    manager->m_state = 7;
    manager->m_nextPage = 5;
}


// ---- src/JetskiMPInitEmpty/ctor.cpp

JetskiMPInitEmpty::JetskiMPInitEmpty(int enabled)
    : JetskiUIPage(enabled)
{
    m_x16C = 0;
    m_checked = 0;
    m_delay = 0.125f;
}


// ---- src/JetskiMPInitEmpty/dtor.cpp


JetskiMPInitEmpty::~JetskiMPInitEmpty()
{
}


// ---- src/JetskiMPInitEmpty/Tick.cpp
extern int TimerOverflowCounterT0;

// Session-state shim: reproduces retail's register state from its one-run build.
static void MWCC_SHIM_TEXT MwccSessionState(long *a) { a[0] = a[0] / a[1]; }

// timeGetTime as a float number of milliseconds.
inline float f_timeGetTime()
{
    static long Time = 0;
    static int lastCount = 0;
    int count;
    int elapsed;
    int overflows;
    count = *(volatile int *)0x10000000;
    overflows = TimerOverflowCounterT0;
    TimerOverflowCounterT0 = 0;
    if (lastCount <= count)
        elapsed = count - lastCount;
    else
        elapsed = 0x10000 - lastCount + count;
    if (lastCount >= count && overflows > 0)
        overflows--;
    lastCount = count;
    Time += elapsed + (overflows << 16);
    return 0.0017066667f * Time;
}



// The front end's UI state: JetskiUIManager plus the memory-card page state.


int JetskiMPInitEmpty::Tick(float dt)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    JetskiUIManCard *manager = (JetskiUIManCard *)m_manager;

    switch (manager->m_x2D0) {
    case 1:
        if (manager->m_state)
            return JetskiUIPage::Tick(dt);
        {
            float start = f_timeGetTime();
            float end = start + m_delay;
            while (f_timeGetTime() < end)
#ifdef SPLASHDOWN_PC
                port_time_spin()
#endif
                ;
        }
        options->SetUI(1, 1);
        LpGame->m_loadManager->SetStatus(7, 10, 7);
        break;
    case 0:
        if (m_checked)
            return JetskiUIPage::Tick(dt);
        {
            int result = options->CheckForMemoryCard(0);
            if (result > 0) {
                if (!options->IsGameInstalled()) {
                    if (options->GetFreeMemoryCardSpace() < 0x5C) {
                        manager->m_state = 7;
                        manager->m_nextPage = 2;
                    } else {
                        if (m_background) {
                            m_background->Release();
                            m_background = 0;
                        }
                        m_delay = 0;
                        manager->m_x2D0 = 1;
                        manager->m_state = 7;
                        manager->m_nextPage = 5;
                    }
                } else {
                    manager->m_state = 7;
                    manager->m_nextPage = 3;
                }
            } else if (result == 0) {
                manager->m_state = 7;
                manager->m_nextPage = 1;
            } else {
                manager->m_state = 7;
                manager->m_nextPage = 4;
            }
        }
        m_checked = 1;
        break;
    }
    return JetskiUIPage::Tick(dt);
}


// ---- (JetskiMPInitEmpty::ResetPage)

void JetskiMPInitEmpty::ResetPage()
{
    m_x16C = 0;
    m_checked = 0;
}


// ---- src/JetskiMPInitEmpty/RenderPre3D.cpp
class TextureMap;
struct _RECT;





int JetskiMPInitEmpty::RenderPre3D()
{
    if (m_background && m_checked)
        LpRenderTarget->Blt(0, m_background, 0, 0);
    return GameObject::RenderPre3D();
}


// ---- src/JetskiMPSaveEmptyChamp/ctor.cpp

JetskiMPSaveEmptyChamp::JetskiMPSaveEmptyChamp(int enabled)
    : JetskiUIPage(enabled)
{
}


// ---- src/JetskiMPSaveEmptyChamp/dtor.cpp


JetskiMPSaveEmptyChamp::~JetskiMPSaveEmptyChamp()
{
}


// ---- src/JetskiMPSaveEmptyChamp/Tick.cpp






int JetskiMPSaveEmptyChamp::Tick(float dt)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    JetskiUIManMain *state = ((JetskiUIManMain *)m_manager);
    if (m_checked) {
        return JetskiUIPage::Tick(dt);
    }
    state->m_x300 = 2;
    int cards = options->CheckForMemoryCard(0);
    if (cards > 0) {
        if (!options->IsGameInstalled()) {
            if (options->GetFreeMemoryCardSpace() < 0x5C) {
                state->m_state = 7;
                state->m_nextPage = 0x14;
            } else {
                state->m_state = 7;
                state->m_nextPage = 0x12;
            }
        } else {
            state->m_state = 7;
            state->m_nextPage = 0x12;
        }
    } else if (cards == 0) {
        state->m_state = 7;
        state->m_nextPage = 0x13;
    } else {
        state->m_state = 7;
        state->m_nextPage = 0x12;
    }
    m_checked = 1;
    return JetskiUIPage::Tick(dt);
}


// ---- src/JetskiMPSaveEmptyChamp/ResetPage.cpp





void JetskiMPSaveEmptyChamp::ResetPage()
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    if (options->m_careerMode)
        options->m_careerMode = 0;
    m_checked = 0;
    ((JetskiUIManMain *)m_manager)->m_waterEffect->Pause(1);
}


// ---- src/JetskiMPLoadEmptyOpt/ctor.cpp

JetskiMPLoadEmptyOpt::JetskiMPLoadEmptyOpt(int enabled)
    : JetskiUIPage(enabled)
{
}


// ---- src/JetskiMPLoadEmptyOpt/dtor.cpp


JetskiMPLoadEmptyOpt::~JetskiMPLoadEmptyOpt()
{
}


// ---- src/JetskiMPLoadEmptyOpt/Tick.cpp




// Once, pick the page to go to from the memory card's state.
int JetskiMPLoadEmptyOpt::Tick(float dt)
{
    JetskiUIManMain *parent = ((JetskiUIManMain *)m_manager);
    JetskiGameOptions *options = LpGame->m_gameOptions;
    if (m_checked)
        return JetskiUIPage::Tick(dt);
    int card = options->CheckForMemoryCard(0);
    if (card > 0) {
        if (!options->IsGameInstalled()) {
            parent->m_state = 7;
            parent->m_nextPage = 0xF;
        } else {
            parent->m_state = 7;
            parent->m_nextPage = 0xD;
        }
    } else if (card == 0) {
        parent->m_state = 7;
        parent->m_nextPage = 0xE;
    } else {
        parent->m_state = 7;
        parent->m_nextPage = 0x10;
    }
    m_checked = 1;
    return JetskiUIPage::Tick(dt);
}


// ---- src/JetskiMPLoadEmptyOpt/ResetPage.cpp



void JetskiMPLoadEmptyOpt::ResetPage()
{
    m_checked = 0;
    ((JetskiUIManMain *)m_manager)->m_waterEffect->Pause(1);
}


// ---- src/JetskiMPLoadYes/ctor.cpp

JetskiMPLoadYes::JetskiMPLoadYes(int enabled)
    : JetskiUIPage(enabled)
{
    m_loaded = 0;
    m_failed = 0;
    m_loading = 0;
    m_loadDelay = 0;
}


// ---- src/JetskiMPLoadYes/dtor.cpp


JetskiMPLoadYes::~JetskiMPLoadYes()
{
}


// ---- src/JetskiMPLoadYes/Tick.cpp






int JetskiMPLoadYes::Tick(float dt)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    if (m_cardCheckTimer < 17)
        m_cardCheckTimer++;
    if (((JetskiUIManMain *)m_manager)->m_state) {
        return JetskiUIPage::Tick(dt);
    }
    if (!m_loadDelay && !m_busy && !m_cancelled && !m_loaded && !m_failed && !m_loading && m_cardCheckTimer > 15) {
        m_cardCheckTimer = 0;
        if (!options->CheckForMemoryCard(0)) {
            ((JetskiUIManMain *)m_manager)->m_state = 7;
            ((JetskiUIManMain *)m_manager)->m_nextPage = 12;
            return JetskiUIPage::Tick(dt);
        }
    }
    if (m_cancelled) {
        m_cancelled = 0;
        ((JetskiUIManMain *)m_manager)->m_state = 7;
        ((JetskiUIManMain *)m_manager)->m_nextPage = 6;
        PlayPageSound(2);
    }
    if (m_loaded) {
        m_loaded = 0;
        for (int i = 0; i < 5; i++)
            DisableGroupControl(1, i + 1);
        EnableGroupControl(1, 3);
        SetPlayerFocus(1, 3, 0);
    }
    if (m_failed) {
        m_failed = 0;
        for (int i = 0; i < 5; i++)
            DisableGroupControl(1, i + 1);
        EnableGroupControl(1, 4);
        SetPlayerFocus(1, 4, 0);
    }
    if (m_loadDelay > 1) {
        m_loadDelay = 0;
        m_busy = 1;
        if (options->LoadGameProfileCardYes())
            m_loaded = 1;
        else
            m_failed = 1;
    } else if (m_loadDelay > 0) {
        m_loadDelay++;
    }
    if (m_loading) {
        m_loading = 0;
        m_loadDelay = 1;
        for (int i = 0; i < 5; i++)
            DisableGroupControl(1, i + 1);
        EnableGroupControl(1, 5);
        SetPlayerFocus(1, 5, 0);
    }
    return JetskiUIPage::Tick(dt);
}


// ---- src/JetskiMPLoadYes/ResetPage.cpp


void JetskiMPLoadYes::ResetPage()
{
    m_loaded = 0;
    m_failed = 0;
    m_loading = 0;
    m_busy = 0;
    m_cancelled = 0;
    m_x17C = 1;
    m_x180 = 0;
    m_cardCheckTimer = 0;
    m_loadDelay = 0;
    if (LpGame->m_controls->m_joysticks[0]->IsUsingMultiTap())
        SetPlayerFocus(1, 2, 0);
    else
        SetPlayerFocus(1, 1, 0);
}


// ---- src/JetskiMPLoadYes/ProcessUserInput.cpp




int JetskiMPLoadYes::ProcessUserInput(int player, int input)
{
    switch (player) {
    case 0:
        switch (input) {
        case 4:
            switch (GetGroupControlID(1)) {
            case 1:
            case 2:
                if (m_x17C) {
                    m_x17C = 0;
                    m_x180 = 1;
                    m_loading = 1;
                }
                PlayPageSound(2);
                break;
            case 3:
            case 4:
                if (m_busy && m_x180) {
                    m_x180 = 0;
                    m_cancelled = 1;
                }
                PlayPageSound(2);
                break;
            }
            break;
        case 6:
            switch (GetGroupControlID(1)) {
            case 1:
            case 2:
                ((JetskiUIManMain *)m_manager)->m_state = 7;
                ((JetskiUIManMain *)m_manager)->m_nextPage = 6;
                PlayPageSound(2);
                break;
            }
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiMPLoadNo/ctor.cpp

JetskiMPLoadNo::JetskiMPLoadNo(int enabled)
    : JetskiUIPage(enabled)
{
    m_cardCheckTimer = 0.0f;
}


// ---- src/JetskiMPLoadNo/dtor.cpp


JetskiMPLoadNo::~JetskiMPLoadNo()
{
}


// ---- src/JetskiMPLoadNo/Tick.cpp






int JetskiMPLoadNo::Tick(float dt)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    if (((JetskiUIManMain *)m_manager)->m_state) {
        JetskiUIPage::Tick(dt);
    } else {
        if (m_cardCheckTimer >= 0.25f) {
            m_cardCheckTimer = 0;
            if (options->CheckForMemoryCard(0)) {
                ((JetskiUIManMain *)m_manager)->m_state = 7;
                ((JetskiUIManMain *)m_manager)->m_nextPage = 12;
            }
        } else {
            m_cardCheckTimer += dt;
        }
        JetskiUIPage::Tick(dt);
    }
}


// ---- src/JetskiMPLoadNo/ResetPage.cpp


void JetskiMPLoadNo::ResetPage()
{
    m_cardCheckTimer = 0;
    if (LpGame->m_controls->m_joysticks[0]->IsUsingMultiTap())
        SetPlayerFocus(1, 2, 0);
    else
        SetPlayerFocus(1, 1, 0);
}


// ---- src/JetskiMPLoadNo/ProcessUserInput.cpp



int JetskiMPLoadNo::ProcessUserInput(int player, int input)
{
    JetskiUIManager *state = ((JetskiUIManMain *)m_manager);
    switch (player) {
    case 0:
        switch (input) {
        case 6:
            state->m_state = 7;
            state->m_nextPage = 6;
            PlayPageSound(2);
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiMPLoadNoGame/ctor.cpp

JetskiMPLoadNoGame::JetskiMPLoadNoGame(int enabled)
    : JetskiUIPage(enabled)
{
    m_pollTimer = 0.0f;
}


// ---- src/JetskiMPLoadNoGame/dtor.cpp


JetskiMPLoadNoGame::~JetskiMPLoadNoGame()
{
}


// ---- src/JetskiMPLoadNoGame/Tick.cpp





// Poll for the memory card four times a second while the page is up.
int JetskiMPLoadNoGame::Tick(float dt)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    if (((JetskiUIManMain *)m_manager)->m_state)
        return JetskiUIPage::Tick(dt);
    if (m_pollTimer >= 0.25f) {
        m_pollTimer = 0;
        if (!options->CheckForMemoryCard(0)) {
            ((JetskiUIManMain *)m_manager)->m_state = 7;
            ((JetskiUIManMain *)m_manager)->m_nextPage = 12;
        }
    } else {
        m_pollTimer += dt;
    }
    return JetskiUIPage::Tick(dt);
}


// ---- (JetskiMPLoadNoGame::ResetPage)

void JetskiMPLoadNoGame::ResetPage()
{
    m_pollTimer = 0.0f;
}


// ---- src/JetskiMPLoadNoGame/ProcessUserInput.cpp



int JetskiMPLoadNoGame::ProcessUserInput(int player, int input)
{
    JetskiUIManager *state = ((JetskiUIManMain *)m_manager);
    switch (player) {
    case 0:
        switch (input) {
        case 6:
            state->m_state = 7;
            state->m_nextPage = 6;
            PlayPageSound(2);
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiMPLoadNoFormat/ctor.cpp

JetskiMPLoadNoFormat::JetskiMPLoadNoFormat(int enabled)
    : JetskiUIPage(enabled)
{
    m_timer = 0.0f;
}


// ---- src/JetskiMPLoadNoFormat/dtor.cpp


JetskiMPLoadNoFormat::~JetskiMPLoadNoFormat()
{
}


// ---- src/JetskiMPLoadNoFormat/Tick.cpp






int JetskiMPLoadNoFormat::Tick(float dt)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    if (((JetskiUIManMain *)m_manager)->m_state) {
        return JetskiUIPage::Tick(dt);
    }
    if (!(m_timer < 0.25f)) {
        m_timer = 0.0f;
        if (!options->CheckForMemoryCard(0)) {
            ((JetskiUIManMain *)m_manager)->m_state = 7;
            ((JetskiUIManMain *)m_manager)->m_nextPage = 12;
        }
    } else {
        m_timer += dt;
    }
    return JetskiUIPage::Tick(dt);
}


// ---- (JetskiMPLoadNoFormat::ResetPage)

void JetskiMPLoadNoFormat::ResetPage()
{
    m_timer = 0.0f;
}


// ---- src/JetskiMPLoadNoFormat/ProcessUserInput.cpp



int JetskiMPLoadNoFormat::ProcessUserInput(int player, int input)
{
    JetskiUIManager *state = ((JetskiUIManMain *)m_manager);
    switch (player) {
    case 0:
        switch (input) {
        case 6:
            state->m_state = 7;
            state->m_nextPage = 6;
            PlayPageSound(2);
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiMPSaveEmptyOpt/ctor.cpp

JetskiMPSaveEmptyOpt::JetskiMPSaveEmptyOpt(int enabled)
    : JetskiUIPage(enabled)
{
}


// ---- src/JetskiMPSaveEmptyOpt/dtor.cpp


JetskiMPSaveEmptyOpt::~JetskiMPSaveEmptyOpt()
{
}


// ---- src/JetskiMPSaveEmptyOpt/Tick.cpp






int JetskiMPSaveEmptyOpt::Tick(float dt)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    JetskiUIManMain *state = ((JetskiUIManMain *)m_manager);
    if (m_checked) {
        return JetskiUIPage::Tick(dt);
    }
    state->m_x300 = 1;
    int cards = options->CheckForMemoryCard(0);
    if (cards > 0) {
        if (!options->IsGameInstalled()) {
            if (options->GetFreeMemoryCardSpace() < 0x5C) {
                state->m_state = 7;
                state->m_nextPage = 0x14;
            } else {
                state->m_state = 7;
                state->m_nextPage = 0x12;
            }
        } else {
            state->m_state = 7;
            state->m_nextPage = 0x12;
        }
    } else if (cards == 0) {
        state->m_state = 7;
        state->m_nextPage = 0x13;
    } else {
        state->m_state = 7;
        state->m_nextPage = 0x12;
    }
    m_checked = 1;
    return JetskiUIPage::Tick(dt);
}


// ---- src/JetskiMPSaveEmptyOpt/ResetPage.cpp





void JetskiMPSaveEmptyOpt::ResetPage()
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    if (options->m_careerMode)
        options->m_careerMode = 0;
    m_checked = 0;
    ((JetskiUIManMain *)m_manager)->m_waterEffect->Pause(1);
}


// ---- src/JetskiMPSaveYes/ctor.cpp

JetskiMPSaveYes::JetskiMPSaveYes(int enabled)
    : JetskiUIPage(enabled)
{
    m_saved = 0;
    m_failed = 0;
    m_saving = 0;
    m_saveDelay = 0;
}


// ---- src/JetskiMPSaveYes/dtor.cpp


JetskiMPSaveYes::~JetskiMPSaveYes()
{
}


// ---- src/JetskiMPSaveYes/Tick.cpp






int JetskiMPSaveYes::Tick(float dt)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    if (m_cardCheckTimer < 17)
        m_cardCheckTimer++;
    if (((JetskiUIManMain *)m_manager)->m_state) {
        return JetskiUIPage::Tick(dt);
    }
    if (!m_saveDelay && !m_saved && !m_failed && !m_saving && !m_busy && m_cardCheckTimer > 15) {
        m_cardCheckTimer = 0;
        if (!options->CheckForMemoryCard(0)) {
            MoveToEmpty();
            return JetskiUIPage::Tick(dt);
        }
    }
    if (m_saved) {
        m_saved = 0;
        for (int i = 0; i < 5; i++)
            DisableGroupControl(1, i + 1);
        EnableGroupControl(1, 5);
        SetPlayerFocus(1, 5, 0);
    }
    if (m_failed) {
        m_failed = 0;
        for (int i = 0; i < 5; i++)
            DisableGroupControl(1, i + 1);
        EnableGroupControl(1, 6);
        SetPlayerFocus(1, 6, 0);
    }
    if (m_saveDelay > 1) {
        m_saveDelay = 0;
        m_busy = 1;
        if (options->SaveAll())
            m_saved = 1;
        else
            m_failed = 1;
    } else if (m_saveDelay > 0) {
        m_saveDelay++;
    }
    if (m_saving) {
        m_saving = 0;
        m_saveDelay = 1;
        for (int i = 0; i < 5; i++)
            DisableGroupControl(1, i + 1);
        EnableGroupControl(1, 7);
        SetPlayerFocus(1, 7, 0);
    }
    return JetskiUIPage::Tick(dt);
}


// ---- src/JetskiMPSaveYes/ResetPage.cpp
class KeyInfo;


struct GameInput {
    char pad0[0x10];
    PSXJoystickDevice *m_joystick;      // +0x10
};





// Focuses the first usable group: 2 or 4 with an installed game, else 1 or 3
// (the multitap layouts use the higher one).
void JetskiMPSaveYes::ResetPage()
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    m_saved = 0;
    m_failed = 0;
    m_saving = 0;
    m_busy = 0;
    m_cardCheckTimer = 0;
    m_saveDelay = 0;
    if (options->CheckForMemoryCard(0) > 0) {
        if (options->IsGameInstalled()) {
            if (LpGame->m_controls->m_joysticks[0]->IsUsingMultiTap())
                SetPlayerFocus(1, 4, 0);
            else
                SetPlayerFocus(1, 2, 0);
        } else {
            if (LpGame->m_controls->m_joysticks[0]->IsUsingMultiTap())
                SetPlayerFocus(1, 3, 0);
            else
                SetPlayerFocus(1, 1, 0);
        }
    } else {
        if (LpGame->m_controls->m_joysticks[0]->IsUsingMultiTap())
            SetPlayerFocus(1, 3, 0);
        else
            SetPlayerFocus(1, 1, 0);
    }
}


// ---- src/JetskiMPSaveYes/ProcessUserInput.cpp






int JetskiMPSaveYes::ProcessUserInput(int player, int input)
{
    JetskiUIManager *manager = ((JetskiUIManMain *)m_manager);
    JetskiGameOptions *options = LpGame->m_gameOptions;
    switch (player) {
    case 0:
        switch (GetGroupControlID(1)) {
        case 1:
        case 2:
        case 3:
        case 4:
            switch (input) {
            case 4:
                if (options->CheckForMemoryCard(0) < 0) {
                    manager->m_state = 7;
                    manager->m_nextPage = 0x15;
                } else {
                    m_saving = 1;
                }
                PlayPageSound(2);
                break;
            case 6:
                Done();
                PlayPageSound(2);
                break;
            }
            break;
        case 5:
        case 6:
            switch (input) {
            case 4:
                Done();
                PlayPageSound(2);
                break;
            }
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiMPSaveYes/Done.cpp




void JetskiMPSaveYes::Done()
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    switch (((JetskiUIManMain *)m_manager)->m_x300) {
    case 2:
        ((JetskiUIManMain *)m_manager)->m_nextPage = options->m_x5FD0;
        ((JetskiUIManMain *)m_manager)->m_state = 7;
        break;
    case 1:
        ((JetskiUIManMain *)m_manager)->m_state = 7;
        ((JetskiUIManMain *)m_manager)->m_nextPage = options->m_x5FD4;
        break;
    }
}


// ---- src/JetskiMPSaveFormat/MoveToEmpty.cpp


void JetskiMPSaveYes::MoveToEmpty()
{
    switch (((JetskiUIManMain *)m_manager)->m_x300) {
    case 2:
        ((JetskiUIManMain *)m_manager)->m_state = 7;
        ((JetskiUIManMain *)m_manager)->m_nextPage = 0x1D;
        break;
    case 1:
        ((JetskiUIManMain *)m_manager)->m_state = 7;
        ((JetskiUIManMain *)m_manager)->m_nextPage = 0x11;
        break;
    }
}


// ---- src/JetskiMPSaveNo/ctor.cpp

JetskiMPSaveNo::JetskiMPSaveNo(int enabled)
    : JetskiUIPage(enabled)
{
    m_checkTimer = 0.0f;
}


// ---- src/JetskiMPSaveNo/dtor.cpp


JetskiMPSaveNo::~JetskiMPSaveNo()
{
}


// ---- src/JetskiMPSaveNo/Tick.cpp





// Polls for a memory card four times a second.
int JetskiMPSaveNo::Tick(float dt)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    if (((JetskiUIManMain *)m_manager)->m_state)
        return JetskiUIPage::Tick(dt);
    if (m_checkTimer >= 0.25f) {
        m_checkTimer = 0;
        if (options->CheckForMemoryCard(0))
            MoveToEmpty();
    } else {
        m_checkTimer += dt;
    }
    return JetskiUIPage::Tick(dt);
}


// ---- src/JetskiMPSaveNo/ResetPage.cpp
class KeyInfo;








void JetskiMPSaveNo::ResetPage()
{
    m_checkTimer = 0;
    if (LpGame->m_controls->m_joysticks[0]->IsUsingMultiTap())
        SetPlayerFocus(1, 2, 0);
    else
        SetPlayerFocus(1, 1, 0);
    JetskiGameOptions *options = LpGame->m_gameOptions;
    switch (((JetskiUIManMain *)m_manager)->m_x300) {
    case 2:
        DisableGroupControlWidget(0, 1, 0x1A);
        DisableGroupControlWidget(0, 1, 0x1B);
        EnableGroupControlWidget(0, 1, 0x18);
        EnableGroupControlWidget(0, 1, 0x19);
        break;
    case 1:
        if (options->m_x5FD4 == 6) {
            DisableGroupControlWidget(0, 1, 0x18);
            DisableGroupControlWidget(0, 1, 0x19);
            EnableGroupControlWidget(0, 1, 0x1A);
            EnableGroupControlWidget(0, 1, 0x1B);
        } else {
            DisableGroupControlWidget(0, 1, 0x1A);
            DisableGroupControlWidget(0, 1, 0x1B);
            EnableGroupControlWidget(0, 1, 0x18);
            EnableGroupControlWidget(0, 1, 0x19);
        }
        break;
    }
}


// ---- src/JetskiMPSaveNo/ProcessUserInput.cpp





int JetskiMPSaveNo::ProcessUserInput(int player, int input)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    // Retail mixes the cached state pointer and fresh ((JetskiUIManMain *)m_manager) loads.
    JetskiUIManMain *state = ((JetskiUIManMain *)m_manager);
    switch (player) {
    case 0:
        switch (state->m_x300) {
        case 2:
            switch (input) {
            case 4:
                state->m_nextPage = options->m_x5FD0;
                ((JetskiUIManMain *)m_manager)->m_state = 7;
                PlayPageSound(2);
                break;
            }
            break;
        case 1:
            if (options->m_x5FD4 == 6) {
                switch (input) {
                case 6:
                    state->m_state = 7;
                    ((JetskiUIManMain *)m_manager)->m_nextPage = options->m_x5FD4;
                    PlayPageSound(2);
                    break;
                }
            } else {
                switch (input) {
                case 4:
                    state->m_state = 7;
                    ((JetskiUIManMain *)m_manager)->m_nextPage = options->m_x5FD4;
                    PlayPageSound(2);
                    break;
                }
            }
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiMPSaveNoSpace/MoveToEmpty.cpp


void JetskiMPSaveNo::MoveToEmpty()
{
    switch (((JetskiUIManMain *)m_manager)->m_x300) {
    case 2:
        ((JetskiUIManMain *)m_manager)->m_state = 7;
        ((JetskiUIManMain *)m_manager)->m_nextPage = 29;
        break;
    case 1:
        ((JetskiUIManMain *)m_manager)->m_state = 7;
        ((JetskiUIManMain *)m_manager)->m_nextPage = 17;
        break;
    }
}


// ---- src/JetskiMPSaveNoSpace/ctor.cpp

JetskiMPSaveNoSpace::JetskiMPSaveNoSpace(int enabled)
    : JetskiUIPage(enabled)
{
    m_checkTimer = 0.0f;
}


// ---- src/JetskiMPSaveNoSpace/dtor.cpp


JetskiMPSaveNoSpace::~JetskiMPSaveNoSpace()
{
}


// ---- src/JetskiMPSaveNoSpace/Tick.cpp





// Checks the memory card four times a second.
int JetskiMPSaveNoSpace::Tick(float dt)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    if (((JetskiUIManMain *)m_manager)->m_state)
        return JetskiUIPage::Tick(dt);
    if (m_checkTimer >= 0.25f) {
        m_checkTimer = 0;
        if (!options->CheckForMemoryCard(0))
            MoveToEmpty();
    } else {
        m_checkTimer += dt;
    }
    return JetskiUIPage::Tick(dt);
}


// ---- src/JetskiMPSaveNoSpace/ResetPage.cpp





void JetskiMPSaveNoSpace::ResetPage()
{
    m_checkTimer = 0;
    JetskiGameOptions *options = LpGame->m_gameOptions;
    switch (((JetskiUIManMain *)m_manager)->m_x300) {
    case 2:
        DisableGroupControlWidget(0, 1, 0x1A);
        DisableGroupControlWidget(0, 1, 0x1B);
        EnableGroupControlWidget(0, 1, 0x18);
        EnableGroupControlWidget(0, 1, 0x19);
        break;
    case 1:
        if (options->m_x5FD4 == 6) {
            DisableGroupControlWidget(0, 1, 0x18);
            DisableGroupControlWidget(0, 1, 0x19);
            EnableGroupControlWidget(0, 1, 0x1A);
            EnableGroupControlWidget(0, 1, 0x1B);
        } else {
            DisableGroupControlWidget(0, 1, 0x1A);
            DisableGroupControlWidget(0, 1, 0x1B);
            EnableGroupControlWidget(0, 1, 0x18);
            EnableGroupControlWidget(0, 1, 0x19);
        }
        break;
    }
}


// ---- src/JetskiMPSaveNoSpace/ProcessUserInput.cpp





int JetskiMPSaveNoSpace::ProcessUserInput(int player, int input)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    // Retail mixes the cached state pointer and fresh ((JetskiUIManMain *)m_manager) loads.
    JetskiUIManMain *state = ((JetskiUIManMain *)m_manager);
    switch (player) {
    case 0:
        switch (state->m_x300) {
        case 2:
            switch (input) {
            case 4:
                state->m_nextPage = options->m_x5FD0;
                ((JetskiUIManMain *)m_manager)->m_state = 7;
                PlayPageSound(2);
                break;
            }
            break;
        case 1:
            if (options->m_x5FD4 == 6) {
                switch (input) {
                case 6:
                    state->m_state = 7;
                    ((JetskiUIManMain *)m_manager)->m_nextPage = options->m_x5FD4;
                    PlayPageSound(2);
                    break;
                }
            } else {
                switch (input) {
                case 4:
                    state->m_state = 7;
                    ((JetskiUIManMain *)m_manager)->m_nextPage = options->m_x5FD4;
                    PlayPageSound(2);
                    break;
                }
            }
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiMPSaveNo/MoveToEmpty.cpp


void JetskiMPSaveNoSpace::MoveToEmpty()
{
    switch (((JetskiUIManMain *)m_manager)->m_x300) {
    case 2:
        ((JetskiUIManMain *)m_manager)->m_state = 7;
        ((JetskiUIManMain *)m_manager)->m_nextPage = 0x1D;
        break;
    case 1:
        ((JetskiUIManMain *)m_manager)->m_state = 7;
        ((JetskiUIManMain *)m_manager)->m_nextPage = 0x11;
        break;
    }
}


// ---- src/JetskiMPSaveFormat/ctor.cpp

JetskiMPSaveFormat::JetskiMPSaveFormat(int enabled)
    : JetskiUIPage(enabled)
{
    m_formatFailed = 0;
    m_formatted = 0;
    m_save = 0;
    m_saved = 0;
    m_saveFailed = 0;
    m_startFormat = 0;
    m_x180 = 0;
    m_delay = 0;
}


// ---- src/JetskiMPSaveFormat/dtor.cpp


JetskiMPSaveFormat::~JetskiMPSaveFormat()
{
}


// ---- src/JetskiMPSaveFormat/Tick.cpp







// Shows group `group` alone and gives it the focus.
#define SHOW_GROUP(group)                       \
    for (int i = 0; i < 7; i++)                 \
        DisableGroupControl(1, i + 1);          \
    EnableGroupControl(1, group);               \
    SetPlayerFocus(1, group, 0);

int JetskiMPSaveFormat::Tick(float dt)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    if (m_checkCount < 17)
        m_checkCount++;
    if (((JetskiUIManMain *)m_manager)->m_state)
        return JetskiUIPage::Tick(dt);
    if (!m_delay && !m_formatFailed && !m_formatted && !m_saved && !m_saveFailed && !m_startFormat
        && !m_x180 && !m_save && m_checkCount > 15) {
        m_checkCount = 0;
        if (!options->CheckForMemoryCard(0)) {
            // The card was removed.
            MoveToEmpty();
            return JetskiUIPage::Tick(dt);
        }
    }
    if (m_formatFailed) {
        m_formatFailed = 0;
        SHOW_GROUP(6)
    }
    if (m_saved) {
        m_saved = 0;
        SHOW_GROUP(3)
    }
    if (m_saveFailed) {
        m_saveFailed = 0;
        SHOW_GROUP(4)
    }
    if (m_delay > 1) {
        if (m_save) {
            m_delay = 0;
            m_save = 0;
            if (options->SaveAll())
                m_saved = 1;
            else
                m_saveFailed = 1;
        } else {
            m_delay = 0;
            m_x180 = 1;
            if (!options->FormatMemoryCard()) {
                m_formatFailed = 1;
            } else {
                m_formatted = 1;
                m_save = 1;
            }
        }
    } else if (m_delay > 0) {
        m_delay++;
    }
    if (m_startFormat) {
        m_startFormat = 0;
        m_delay = 1;
        SHOW_GROUP(7)
    }
    if (m_formatted) {
        m_delay = 1;
        m_formatted = 0;
        SHOW_GROUP(5)
    }
    return JetskiUIPage::Tick(dt);
}


// ---- src/JetskiMPSaveFormat/ResetPage.cpp


void JetskiMPSaveFormat::ResetPage()
{
    m_formatFailed = 0;
    m_formatted = 0;
    m_save = 0;
    m_saved = 0;
    m_saveFailed = 0;
    m_startFormat = 0;
    m_x180 = 0;
    m_checkCount = 0;
    m_delay = 0;
    if (LpGame->m_controls->m_joysticks[0]->IsUsingMultiTap())
        SetPlayerFocus(1, 2, 0);
    else
        SetPlayerFocus(1, 1, 0);
}


// ---- src/JetskiMPSaveFormat/ProcessUserInput.cpp



int JetskiMPSaveFormat::ProcessUserInput(int player, int input)
{
    switch (player) {
    case 0:
        switch (GetGroupControlID(1)) {
        case 1:
        case 2:
            switch (input) {
            case 7:
                m_startFormat = 1;
                PlayPageSound(2);
                break;
            case 6:
                Done();
                PlayPageSound(2);
                break;
            }
            break;
        case 6:
        case 4:
        case 3:
            switch (input) {
            case 4:
                Done();
                PlayPageSound(2);
                break;
            }
            break;
        }
        break;
    }
    return 1;
}


// ---- src/JetskiMPSaveFormat/Done.cpp




void JetskiMPSaveFormat::Done()
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    switch (((JetskiUIManMain *)m_manager)->m_x300) {
    case 2:
        ((JetskiUIManMain *)m_manager)->m_nextPage = options->m_x5FD0;
        ((JetskiUIManMain *)m_manager)->m_state = 7;
        break;
    case 1:
        ((JetskiUIManMain *)m_manager)->m_state = 7;
        ((JetskiUIManMain *)m_manager)->m_nextPage = options->m_x5FD4;
        break;
    }
}


// ---- src/JetskiMPSaveYes/MoveToEmpty.cpp


void JetskiMPSaveFormat::MoveToEmpty()
{
    switch (((JetskiUIManMain *)m_manager)->m_x300) {
    case 2:
        ((JetskiUIManMain *)m_manager)->m_state = 7;
        ((JetskiUIManMain *)m_manager)->m_nextPage = 29;
        break;
    case 1:
        ((JetskiUIManMain *)m_manager)->m_state = 7;
        ((JetskiUIManMain *)m_manager)->m_nextPage = 17;
        break;
    }
}

