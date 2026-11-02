// JetskiUIPageSettings.cpp  @ 0x003CA670 (text 0x3CA670-0x3CAE10)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiUIPageSettings.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the object's data
// as well as its code (tools/link_c.py).



#include "JetskiManagerSettingsMain/JetskiManagerSettingsMain.h"
#include "JetskiPageSettingsCharSelect/JetskiPageSettingsCharSelect.h"
#include "JetskiPageSettingsSetup/JetskiPageSettingsSetup.h"
#include "Jetski_Setup_Event_Struct/Jetski_Setup_Event_Struct.h"
#include "JetskiPageSettingsMain/JetskiPageSettingsMain.h"
#include "JetskiPageSettingsOptions/JetskiPageSettingsOptions.h"
#include "JetskiPageSettingsGOptions/JetskiPageSettingsGOptions.h"
#include "JetskiPageSettingsLoadSave/JetskiPageSettingsLoadSave.h"
#include "JetskiPageSettingsVideos/JetskiPageSettingsVideos.h"
#include "JetskiPageSettings/JetskiPageSettings.h"
#include "sinit/AxisVectors.h"

// ---- src/JetskiManagerSettingsMain/ctor.cpp

JetskiManagerSettingsMain::JetskiManagerSettingsMain()
{
    Reset();
}


// ---- src/JetskiManagerSettingsMain/dtor.cpp

JetskiManagerSettingsMain::~JetskiManagerSettingsMain()
{
}


// ---- src/JetskiManagerSettingsMain/Reset.cpp

void JetskiManagerSettingsMain::Reset()
{
    m_x0 = 1;
    m_x4 = 1;
}


// ---- src/JetskiPageSettingsCharSelect/ctor.cpp

JetskiPageSettingsCharSelect::JetskiPageSettingsCharSelect()
{
    Reset();
}


// ---- src/JetskiPageSettingsCharSelect/dtor.cpp

JetskiPageSettingsCharSelect::~JetskiPageSettingsCharSelect()
{
}


// ---- src/JetskiPageSettingsCharSelect/Reset.cpp

void JetskiPageSettingsCharSelect::Reset()
{
    m_x0 = m_x8 = m_x10 = 0;
    m_x4 = m_xC = m_x14 = 0;
}


// ---- src/JetskiPageSettingsSetup/ctor.cpp

JetskiPageSettingsSetup::JetskiPageSettingsSetup()
{
    Reset();
}


// ---- src/Jetski_Setup_Event_Struct/ctor.cpp

Jetski_Setup_Event_Struct::Jetski_Setup_Event_Struct()
{
    m_x0 = -1;
    m_x4 = -1;
    m_x8 = -1;
    m_xC = -1;
}


// ---- src/JetskiPageSettingsSetup/dtor.cpp

JetskiPageSettingsSetup::~JetskiPageSettingsSetup()
{
}


// ---- src/JetskiPageSettingsSetup/Reset.cpp

void JetskiPageSettingsSetup::Reset()
{
    m_xA0 = m_xA4 = 1;
    m_events[0].m_x0 = m_events2[0].m_x0 = 1;
    m_events[0].m_x4 = m_events2[0].m_x4 = 1;
    m_events[0].m_xC = m_events2[0].m_xC = 3;
    m_events[1].m_x0 = m_events2[1].m_x0 = 1;
    m_events[1].m_x4 = m_events2[1].m_x4 = 1;
    m_events[2].m_x0 = m_events2[2].m_x0 = 1;
    m_events[2].m_x4 = m_events2[2].m_x4 = 1;
    m_events[2].m_x8 = m_events2[2].m_x8 = 1;
    m_events[4].m_x0 = m_events2[4].m_x0 = 1;
    m_events[3].m_x0 = m_events2[3].m_x0 = 7;
}


// ---- src/JetskiPageSettingsMain/ctor.cpp

JetskiPageSettingsMain::JetskiPageSettingsMain()
{
    Reset();
}


// ---- src/JetskiPageSettingsMain/dtor.cpp

JetskiPageSettingsMain::~JetskiPageSettingsMain()
{
}


// ---- src/JetskiPageSettingsMain/Reset.cpp

void JetskiPageSettingsMain::Reset()
{
    m_x0 = 3;
}


// ---- src/JetskiPageSettingsOptions/ctor.cpp

JetskiPageSettingsOptions::JetskiPageSettingsOptions()
{
    Reset();
}


// ---- src/JetskiPageSettingsOptions/dtor.cpp

JetskiPageSettingsOptions::~JetskiPageSettingsOptions()
{
}


// ---- src/JetskiPageSettingsOptions/Reset.cpp

void JetskiPageSettingsOptions::Reset()
{
    m_x0 = 1;
}


// ---- src/JetskiPageSettingsGOptions/ctor.cpp

JetskiPageSettingsGOptions::JetskiPageSettingsGOptions()
{
    Reset();
}


// ---- src/JetskiPageSettingsGOptions/dtor.cpp

JetskiPageSettingsGOptions::~JetskiPageSettingsGOptions()
{
}


// ---- src/JetskiPageSettingsGOptions/Reset.cpp

void JetskiPageSettingsGOptions::Reset()
{
    m_x0 = 1;
}


// ---- src/JetskiPageSettingsLoadSave/ctor.cpp

JetskiPageSettingsLoadSave::JetskiPageSettingsLoadSave()
{
    Reset();
}


// ---- src/JetskiPageSettingsLoadSave/dtor.cpp

JetskiPageSettingsLoadSave::~JetskiPageSettingsLoadSave()
{
}


// ---- src/JetskiPageSettingsLoadSave/Reset.cpp

void JetskiPageSettingsLoadSave::Reset()
{
    m_x0 = 0;
}


// ---- src/JetskiPageSettingsVideos/ctor.cpp

JetskiPageSettingsVideos::JetskiPageSettingsVideos()
{
    Reset();
}


// ---- src/JetskiPageSettingsVideos/dtor.cpp

JetskiPageSettingsVideos::~JetskiPageSettingsVideos()
{
}


// ---- src/JetskiPageSettingsVideos/Reset.cpp

void JetskiPageSettingsVideos::Reset()
{
    m_x0 = -1;
}


// ---- src/JetskiPageSettings/ctor.cpp

JetskiPageSettings::JetskiPageSettings()
{
    m_managerMain = 0;
    m_charSelect = 0;
    m_setup = 0;
    m_main = 0;
    m_options = 0;
    m_gameOptions = 0;
    m_loadSave = 0;
    m_videos = 0;
}


// ---- src/JetskiPageSettings/dtor.cpp

JetskiPageSettings::~JetskiPageSettings()
{
    delete m_managerMain;
    m_managerMain = 0;
    delete m_charSelect;
    m_charSelect = 0;
    delete m_setup;
    m_setup = 0;
    delete m_main;
    m_main = 0;
    delete m_options;
    m_options = 0;
    delete m_gameOptions;
    m_gameOptions = 0;
    delete m_loadSave;
    m_loadSave = 0;
    delete m_videos;
    m_videos = 0;
}


// ---- src/JetskiPageSettings/Create.cpp

void JetskiPageSettings::Create()
{
    m_managerMain = new JetskiManagerSettingsMain;
    m_charSelect = new JetskiPageSettingsCharSelect;
    m_setup = new JetskiPageSettingsSetup;
    m_main = new JetskiPageSettingsMain;
    m_options = new JetskiPageSettingsOptions;
    m_gameOptions = new JetskiPageSettingsGOptions;
    m_loadSave = new JetskiPageSettingsLoadSave;
    m_videos = new JetskiPageSettingsVideos;
}


// ---- src/JetskiPageSettings/ResetOptions.cpp

void JetskiPageSettings::ResetOptions()
{
    m_options->Reset();
    m_gameOptions->Reset();
    m_videos->Reset();
}


// ---- src/JetskiPageSettings/ResetCareer.cpp

void JetskiPageSettings::ResetCareer()
{
    m_loadSave->Reset();
}


// ---- src/sinit/JetskiUIPageSettings.cpp
// Static initializer of JetskiUIPageSettings.cpp: only the axis vectors from the shared header.

