// JetskiGameOptions.cpp  @ 0x002C5E80 (text 0x2C5E80-0x2C9050)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiGameOptions.cpp as one translation unit: JetskiGameOptions' methods,
// JetskiUserOptions' ctor and dtor, then the static initializer (the zlib helpers after the dtor are
// PSXWater.cpp's: their "1.0" follows this TU's RTTI). `make link` places the object's data as well
// as its code (tools/link_c.py). The C-mode stubs are members again (GetFreeMemoryCardSpace,
// Save/RestoreSplitScreenMode, CheatUnlockAllTracks, JetskiUserOptions' ctor); m_hallOfFame is a
// HallOfFameListStruct; ChangeControllerPreset's key table is LpGame->m_controls->m_keyLookupTable.





#include "JetskiGameOptions/JetskiGameOptions.h"
#include "GameObject/GameObject.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "ParameterBlock/ParameterBlock.h"
#include "StringTable/StringTable.h"
#include "soultree_assert_block.h"
#include "LocalizedString/LocalizedString.h"
#include "GameText/GameText.h"
#include "JetskiGame/JetskiGame.h"
#include "MappableKeyLookupTable/MappableKeyLookupTable.h"
#include "CheatFlags/CheatFlags.h"
#include "ControlInterface/ControlInterface.h"
#include "ControlInterface/ControlTypes.h"
#include "JetskiPageSettings/JetskiPageSettings.h"
#include "JetskiUserOptions/JetskiUserOptions.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"

// Retail never calls fptosi here: float->int is an inline cvt.w.s.
#pragma fast_fptosi on
// inline_depth is read when the deferred functions are generated, at the
// end of the TU, so one value applies to every function.
#pragma inline_depth(8)

extern "C" char *strcpy(char *dst, const char *src);
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" int strcasecmp(const char *a, const char *b);
extern "C" int rand(void);
extern "C" void *memset(void *dst, int c, unsigned int n);
void PSXprintf(const char *fmt, ...);

extern JetskiGame *LpGame;
extern PSXVideoCard *LpVideoCard;
extern StringTable *LpMaterialStringTable;

// ---- src/JetskiGameOptions/ctor.cpp


JetskiGameOptions::JetskiGameOptions()
{
    int i;
    int j;
    m_language = 0;
    m_x4A4C = 1;
    m_x834 = 3;
    m_difficulty = 2;
    m_x18 = 0;
    m_raceType = 2;
    m_x2C = 4;
    m_x30 = 1;
    m_numRacers = 5;
    m_numHumans = 1;
    m_x3C = 4;
    m_x40 = 0;
    m_x48 = 0;
    m_x58 = 1;
    m_x54 = 2;
    m_controllerPreset[0] = 1;
    m_controllerPreset[1] = 1;
    m_x86C = 1;
    m_x858 = 1;
    m_hudFlags[0] = 3;
    m_hudFlags[1] = 3;
    m_uiMode = 1;
    m_x28 = 1;
    m_x5FD0 = 1;
    m_x5FD4 = 1;
    m_careerMode = 0;
    m_x5FDC = 1;
    strcpy(m_creditsVideo, "Videos\\Credits.pss");
    m_x89C = 1;
    m_x878 = 1;
    m_screenShiftX = 0;
    m_screenShiftY = 0;
    m_savedViewport = 1;
    m_x50 = 1;
    m_savedSplitScreen = 0;
    m_splitScreen = 0;
    for (i = 0; i < 5; i++) {
        m_racerCharacter[i] = i;
        m_racerSkin[i] = 0;
        m_racerVehicle[i] = i;
        m_racerVehicleSkin[i] = 0;
    }
    SetPlayerTypes(0);
    // "ＳＰＬＡＳＨＤＯＷＮ" in Shift-JIS (split so "\x82" "c" is not one escape).
    m_memoryCard.SetGameTitle("\x82r\x82o\x82k\x82`\x82r\x82g\x82" "c\x82n\x82v\x82m", 0x14);
    m_memoryCard.SetContentDirectory("BASLUS-20223RBSplash");
    m_memoryCard.SetIconNames(1, "NORMAL.ICO", "NORMAL.ICO", "NORMAL.ICO");
    LoadGameProfileCardNo();
    m_team = 0;
    m_teamIndex = -1;
    m_career = 0;
    m_x5FC0 = -1;
    for (int k = 0; k < 9; k++) {
        m_secretSkinUnlocked[k][0] = 1;
        m_secretSkinUnlocked[k][1] = 1;
        m_secretSkinUnlocked[k][2] = 0;
        m_secretSkinUnlocked[k][3] = 0;
        m_secretSkinUnlocked[k][4] = 0;
        m_secretSkinUnlocked[k][5] = 0;
        m_secretSkinUnlocked[k][6] = 0;
        m_secretSkinUnlocked[k][7] = 0;
    }
    m_movieUnlocked[0] = 0;
    m_movieUnlocked[1] = 0;
    m_movieUnlocked[2] = 0;
    m_movieUnlocked[3] = 0;
    m_movieUnlocked[4] = 0;
    m_movieUnlocked[5] = 0;
    m_movieUnlocked[6] = 0;
    m_movieUnlocked[7] = 0;
    m_movieUnlocked[8] = 1;
    m_movieUnlocked[9] = 0;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 21; j++)
            m_trackUnlocked[i][j] = 0;
    }
    m_trackUnlocked[0][0] = 1;
    m_trackUnlocked[0][1] = 1;
    m_trackUnlocked[0][2] = 1;
    m_trackUnlocked[0][4] = 1;
    m_trackUnlocked[0][5] = 1;
    m_trackUnlocked[0][6] = 1;
    m_trackUnlocked[1][0] = 1;
    m_trackUnlocked[1][1] = 1;
    m_trackUnlocked[1][2] = 1;
    m_trackUnlocked[1][4] = 1;
    m_trackUnlocked[1][5] = 1;
    m_trackUnlocked[1][6] = 1;
    for (i = 0; i < 9; i++)
        m_characterUnlocked[i] = 0;
    m_characterUnlocked[0] = 1;
    m_characterUnlocked[1] = 1;
    m_characterUnlocked[2] = 1;
    m_characterUnlocked[3] = 1;
    for (i = 0; i < 21; i++) {
        m_x150[0][i] = 0;
        m_x150[1][i] = 0;
    }
    m_x150[0][0] = 1;
    m_x150[0][8] = 1;
    m_x150[0][5] = 1;
    m_x150[0][10] = 1;
    m_x150[0][17] = 1;
    m_x150[0][19] = 1;
    m_x150[1][6] = 1;
    m_x150[1][8] = 1;
    m_x150[1][10] = 1;
    m_x150[1][12] = 1;
    m_x150[1][17] = 1;
    m_x150[1][19] = 1;
    SetUpSkinIndices();
}


// ---- src/JetskiGameOptions/dtor.cpp
JetskiGameOptions::~JetskiGameOptions()
{
    delete m_pageSettings;
    m_pageSettings = 0;
}


// ---- src/JetskiGameOptions/SetPlayerTypes.cpp

void JetskiGameOptions::SetPlayerTypes(int allComputer)
{
    int i;
    if (allComputer) {
        for (i = 0; i < m_numRacers; i++)
            m_racerActive[i] = 1;
    } else {
        m_racerActive[0] = 0;
        if (m_splitScreen) {
            m_racerActive[1] = 0;
            for (i = 2; i < m_numRacers; i++)
                m_racerActive[i] = 1;
        } else if (m_raceType == 3 && m_x54 == 2) {
            m_racerActive[1] = 2;
            for (i = 2; i < m_numRacers; i++)
                m_racerActive[i] = 1;
        } else {
            for (i = 1; i < m_numRacers; i++)
                m_racerActive[i] = 1;
        }
    }
}


// ---- src/JetskiGameOptions/Create.cpp
void JetskiGameOptions::Create()
{
    m_pageSettings = new JetskiPageSettings;
    m_pageSettings->Create();
}


// ---- src/JetskiGameOptions/CheckForMemoryCard.cpp

int JetskiGameOptions::CheckForMemoryCard(int loadProfile)
{
    int status = m_memoryCard.DetectCard(0, 0);
    if (status == 1) {
        m_memoryCard.SetActivePort(0);
        if (loadProfile)
            LoadGameProfileCardYes();
        m_cardPresent = 1;
    } else if (status == 2) {
    } else if (status == 0) {
        m_cardPresent = 0;
    }
    return status;
}


// ---- src/JetskiGameOptions/GetFreeMemoryCardSpace.cpp
int JetskiGameOptions::GetFreeMemoryCardSpace()
{
    int space;
    m_memoryCard.DetectCard(0, &space);
    return space;
}




// ---- src/JetskiGameOptions/IsGameInstalled.cpp

int JetskiGameOptions::IsGameInstalled()
{
    if (m_memoryCard.DetectCard(0, 0) && m_memoryCard.GetContentDir())
        return 1;
    return 0;
}


// ---- src/JetskiGameOptions/FormatMemoryCard.cpp

// Only formats when DetectCard reports an unformatted card (-1 or -2).
int JetskiGameOptions::FormatMemoryCard()
{
    int result = m_memoryCard.DetectCard(0, 0);
    if (result == -2 || result == -1) {
        if (m_memoryCard.SetActivePort(0) && m_memoryCard.FormatActive())
            return 1;
    }
    return 0;
}


// Retail creates "\n" (@1072) here, long before GetCharacterName's inlined
// GetString ASSERTF uses it: a function with an ASSERT came here that
// nothing called, so retail's linker stripped it with its file name, message
// and statics. link_c leaves its
// sections out.
static void StrippedAssert(int ok)
{
    ASSERT(ok, "stripped");
}


// ---- src/JetskiGameOptions/LoadGameProfileCardNo.cpp


int JetskiGameOptions::LoadGameProfileCardNo()
{
    m_musicVolume = 50;
    m_x828 = 60;
    m_x82C = 80;
    m_x830 = 1;
    for (int i = 0; i < 5; i++)
        DefaultSaveSlot(i);
    PopulateNewHallOfFame(&m_hallOfFame);
    return 1;
}


// ---- src/JetskiGameOptions/LoadGameProfileCardYes.cpp
// CareerProgress::m_name is char[9]: retail's struct copy calls __copy for
// 9 bytes. mwcc copies the small int arrays through FP registers.



int JetskiGameOptions::LoadGameProfileCardYes()
{
    GameProfileSave save;
    m_x86C = 0;
    PSXprintf("Populate!!\n");
    if (m_memoryCard.GetContentDir()) {
        if (m_memoryCard.MUReadFile("BASLUS-20223RBSplash", &save, sizeof(save))) {
            m_profile = save;
        } else {
            return 0;
        }
        m_screenShiftX = m_profile.m_screenShiftX;
        m_screenShiftY = m_profile.m_screenShiftY;
        LpVideoCard->SetShiftX(m_screenShiftX);
        LpVideoCard->SetShiftY(m_screenShiftY);
        m_x834 = m_profile.m_x834;
        m_difficulty = m_profile.m_difficulty;
        m_musicVolume = m_profile.m_musicVolume;
        m_x828 = m_profile.m_x828;
        m_x82C = m_profile.m_x82C;
        m_x830 = m_profile.m_x830;
        m_x50 = m_profile.m_x50;
        m_x4A4C = m_profile.m_x4A4C;
        m_controllerPreset[0] = m_profile.m_controllerPreset[0];
        ChangeControllerPreset(m_controllerPreset[0], 0);
        m_controllerPreset[1] = m_profile.m_controllerPreset[1];
        ChangeControllerPreset(m_controllerPreset[1], 1);
        int i, j;
        for (i = 0; i < 9; i++)
            for (j = 0; j < 8; j++)
                m_secretSkinUnlocked[i][j] = m_profile.m_secretSkinUnlocked[i][j];
        for (i = 0; i < 10; i++)
            m_movieUnlocked[i] = m_profile.m_movieUnlocked[i];
        for (i = 0; i < 2; i++)
            for (j = 0; j < 21; j++)
                m_trackUnlocked[i][j] = m_profile.m_trackUnlocked[i][j];
        for (i = 0; i < 9; i++)
            m_characterUnlocked[i] = m_profile.m_characterUnlocked[i];
        if (m_memoryCard.MUReadFile("fame.dat", &m_hallOfFame, sizeof(m_hallOfFame))) {
            PSXprintf("Read HOF File Successful!!\n");
        } else {
            PSXprintf("Read HOF File Failed!!\n");
            return 0;
        }
    } else {
        return 0;
    }
    m_cardPresent = 1;
    return 1;
}


// ---- src/JetskiGameOptions/SaveAll.cpp

int JetskiGameOptions::SaveAll()
{
    m_memoryCard.SetContentDirectory("BASLUS-20223RBSplash");
    if (!IsGameInstalled())
        m_memoryCard.CreateContentDir();
    if (m_memoryCard.GetContentDir() && SaveStore_GameProfile(1) && SaveHallOfFame())
        return 1;
    return 0;
}



// ---- src/JetskiGameOptions/SaveHallOfFame.cpp


int JetskiGameOptions::SaveHallOfFame()
{
    if (m_memoryCard.MUDeleteFile("fame.dat"))
        PSXprintf("Old HOF File Deletion Successful!!\n");
    if (m_memoryCard.MUCreateFile("fame.dat", &m_hallOfFame, sizeof(m_hallOfFame))) {
        PSXprintf("Create HOF File Successful!!\n");
    } else {
        PSXprintf("Create HOF File Failed!!\n");
        return 0;
    }
    return 1;
}


// ---- src/JetskiGameOptions/SaveStore_GameProfile.cpp


// The game profile as saved to the memory card (+0x4A60, 0x1554 bytes).

// Copies the career profile into its save image and, if asked, writes it to the
// memory card.
int JetskiGameOptions::SaveStore_GameProfile(int save)
{
    int i, j;
    m_profile.m_screenShiftX = m_screenShiftX;
    m_profile.m_screenShiftY = m_screenShiftY;
    m_profile.m_x834 = m_x834;
    m_profile.m_difficulty = m_difficulty;
    m_profile.m_musicVolume = m_musicVolume;
    m_profile.m_x828 = m_x828;
    m_profile.m_x82C = m_x82C;
    m_profile.m_x830 = m_x830;
    m_profile.m_x50 = m_x50;
    m_profile.m_x4A4C = m_x4A4C;
    m_profile.m_controllerPreset[0] = m_controllerPreset[0];
    m_profile.m_controllerPreset[1] = m_controllerPreset[1];
    for (i = 0; i < 9; i++)
        for (j = 0; j < 8; j++)
            m_profile.m_secretSkinUnlocked[i][j] = m_secretSkinUnlocked[i][j];
    for (i = 0; i < 10; i++)
        m_profile.m_movieUnlocked[i] = m_movieUnlocked[i];
    for (i = 0; i < 2; i++)
        for (j = 0; j < 21; j++)
            m_profile.m_trackUnlocked[i][j] = m_trackUnlocked[i][j];
    for (i = 0; i < 9; i++)
        m_profile.m_characterUnlocked[i] = m_characterUnlocked[i];
    if (save) {
        if (m_memoryCard.MUDeleteFile("BASLUS-20223RBSplash"))
            PSXprintf("Old Careers File Deletion Successful!!\n");
        if (m_memoryCard.MUCreateFile("BASLUS-20223RBSplash", &m_profile, sizeof(m_profile))) {
            PSXprintf("Create File Successful!!\n");
        } else {
            PSXprintf("Create File Failed!!\n");
            return 0;
        }
    }
    return 1;
}

void JetskiGameOptions::SaveCurrentSplitScreenMode(int mode, ViewPortCode code)
{
    m_savedSplitScreen = m_splitScreen;
    m_savedViewport = m_x50;
    m_splitScreen = mode;
    m_x50 = code;
}


void JetskiGameOptions::RestoreSplitScreenMode()
{
    m_splitScreen = m_savedSplitScreen;
    m_x50 = m_savedViewport;
}



// ---- src/JetskiGameOptions/DefaultSaveSlot.cpp

// One difficulty's career progress (0x40 bytes).

// One saved career (0x3D0 bytes).

// Resets a save slot: every difficulty unlocked with no progress.
int JetskiGameOptions::DefaultSaveSlot(int slot)
{
    SaveSlot *save = &m_profile.m_slots[slot];
    save->m_x00 = 0;
    save->m_x08 = 0;
    CareerProgress *career;
    for (int i = 0; i < 5; i++) {
        career = &save->m_careers[i];
        switch (i) {
        case 0:
            strcpy(career->m_name, "Storm");
            break;
        case 1:
            strcpy(career->m_name, "Typhoon");
            break;
        case 2:
            strcpy(career->m_name, "Riptide");
            break;
        case 3:
            strcpy(career->m_name, "Neptune");
            break;
        case 4:
            strcpy(career->m_name, "Tsunami");
            break;
        }
        career->m_unlocked = 1;
        career->m_x10 = 0;
        career->m_x14 = 1;
        career->m_x18 = 0;
        for (int j = 0; j < 5; j++)
            career->m_x1C[j] = 0;
        career->m_x30 = 0;
        career->m_x34 = 0;
        career->m_x38 = 0;
        career->m_x3C = 0;
    }
    SetCareerPath(-1, slot);
    return 1;
}


// ---- src/JetskiGameOptions/PopulateNewHallOfFame.cpp
// Hall of fame: for each table, a record per track/event of the six best
// names and times.
// The tables are copied as raw words: retail's flat 8-byte copy loops. A
// HallOfFameTable assignment copies member by member instead.
struct Raw42 { int m_words[sizeof(HallOfFameTable42) / 4]; };
struct Raw21 { int m_words[sizeof(HallOfFameTable21) / 4]; };

// Fills the hall of fame with the default records. `list` is unused: the
// tables are always written to m_hallOfFame (the only caller passes it).
void JetskiGameOptions::PopulateNewHallOfFame(HallOfFameListStruct *list)
{
    HallOfFameTable42 track0 = {{
        {{"Jordan", "Adam", "Hiro", "Scott", "Jose M", "Luca"}, {33.42f, 34.14f, 36.23f, 40.42f, 42.33f, 47.65f}},
        {{"Adam", "Scott", "Hiro", "Jose M", "MarkD", "Luca"}, {100.99f, 101.24f, 112.23f, 118.98f, 120.7f, 122.21f}},
        {{"Bals", "MarkD", "Luca", "Michelle", "MarkD", "Bill"}, {58.38f, 59.01f, 60.88f, 61.79f, 63.66f, 66.85f}},
        {{"Adam", "Robb", "Matt", "Roman", "MarkD", "Scott"}, {175.89f, 178.59f, 183.54f, 184.4f, 187.73f, 189.32f}},
        {{"Scott", "Jordan", "Adam", "CBryan", "Bradicus", "CBryan"}, {62.93f, 63.45f, 64.77f, 65.88f, 68.01f, 69.82f}},
        {{"Scott", "Jordan", "Adam", "Bradicus", "Tim", "CBryan"}, {188.79f, 189.9f, 194.31f, 199.02f, 201.02f, 202.44f}},
        {{"Robb", "Hiro", "Hiro", "MarkM", "DLow", "Jose M"}, {82.23f, 83.93f, 85.55f, 87.84f, 88.21f, 91.85f}},
        {{"Robb", "Hiro", "MarkM", "MarkM", "Amy Lugo", "Amy Lugo"}, {246.81f, 248.01f, 252.33f, 257.78f, 258.9f, 259.1f}},
        {{"Bradicus", "Bradicus", "Robb", "Scott", "Jessica", "MikeChow"}, {65.25f, 65.96f, 67.75f, 68.36f, 70.98f, 72.06f}},
        {{"Bradicus", "Nick", "Nick", "Scott", "Jessica", "MikeChow"}, {195.9f, 201.03f, 202.22f, 203.46f, 206.16f, 206.88f}},
        {{"Robb", "Robb", "Matt", "Matt", "Roman", "Deborah"}, {65.99f, 66.65f, 67.56f, 69.87f, 70.84f, 74.95f}},
        {{"Robb", "Hiro", "Roman", "Matt", "Deborah", "Deborah"}, {198.02f, 199.4f, 201.22f, 214.4f, 215.01f, 221.15f}},
        {{"Scott", "Glenn", "Devin", "McNabb", "Caitlin", "Caitlin"}, {55.87f, 57.57f, 59.06f, 61.88f, 63.95f, 67.04f}},
        {{"Scott", "Glenn", "Devin", "Caitlin", "Glenn", "Caitlin"}, {169.91f, 171.01f, 172.88f, 174.34f, 179.1f, 187.33f}},
        {{"Amarinth", "Earl", "Amarinth", "JGLee", "Jallak", "Rogan"}, {62.61f, 64.12f, 66.14f, 68.92f, 70.84f, 71.32f}},
        {{"Amarinth", "Earl", "JGLee", "Jallak", "KC", "Piriel"}, {190.84f, 191.02f, 191.88f, 195.22f, 199.21f, 200.01f}},
        {{"MarkM", "Rafael", "MikeChow", "Bals", "Bill", "Jessica"}, {41.22f, 42.05f, 44.06f, 44.38f, 46.67f, 49.74f}},
        {{"MarkM", "Scott", "Scott", "Bill", "Bill", "MikeChow"}, {126.01f, 128.22f, 129.13f, 130.84f, 137.27f, 139.92f}},
        {{"Rafael", "Eric C", "Mickael", "Frank", "Mickael", "Adeline"}, {55.45f, 56.04f, 59.93f, 62.75f, 62.89f, 64.47f}},
        {{"Eric", "Olivier", "Olivier", "Mickael", "Adeline", "Frank"}, {168.22f, 169.22f, 174.24f, 177.98f, 179.34f, 183.28f}},
        {{"Jordan", "Jason", "Jason", "Andrew", "Andrew", "Rick"}, {60.4f, 64.43f, 66.84f, 67.14f, 69.84f, 71.46f}},
        {{"Jason", "Jason", "Houman", "Eric C", "Eric", "Rick"}, {183.2f, 182.82f, 188.3f, 188.99f, 190.11f, 192.29f}},
        {{"Rick", "Robb", "Robb", "Dan", "Tim", "Jeff"}, {82.52f, 84.95f, 85.64f, 86.12f, 89.89f, 92.14f}},
        {{"Rick", "Jeff", "Jeff", "Dan", "Jon", "Remy"}, {249.0f, 249.46f, 251.89f, 252.93f, 254.14f, 256.67f}},
        {{"Bradicus", "MarkD", "Jason", "Dan", "Dan", "DLow"}, {75.47f, 75.96f, 77.98f, 79.37f, 83.74f, 87.03f}},
        {{"Bradicus", "Bals", "MarkD", "Dan", "DLow", "Jason"}, {228.78f, 228.9f, 229.98f, 290.32f, 291.78f, 298.12f}},
        {{"Dennis", "Hiro", "McNabb", "Bals", "Bals", "McNabb"}, {94.02f, 98.33f, 99.24f, 100.35f, 101.31f, 104.84f}},
        {{"Bals", "Jose M", "McNabb", "Bals", "Shari", "Shari"}, {283.92f, 284.5f, 287.34f, 290.12f, 290.24f, 297.92f}},
        {{"Wil", "Rafael", "Pete", "Bill", "Jessica", "Horus"}, {75.07f, 75.97f, 76.89f, 78.04f, 80.99f, 83.03f}},
        {{"Wil", "Rafael", "Nick", "Nick", "Jessica", "Horus"}, {227.21f, 230.01f, 231.99f, 238.1f, 240.67f, 242.02f}},
        {{"Adam", "Houman", "MikeChow", "Rafael", "Rafael", "Shari"}, {60.48f, 61.99f, 62.44f, 64.5f, 66.85f, 67.43f}},
        {{"Adam", "Houman", "Rafael", "Eric", "MikeChow", "Bals"}, {184.44f, 186.02f, 189.23f, 191.78f, 196.22f, 199.02f}},
        {{"Rafael", "Rafael", "Don", "Adam", "Joe", "MarkM"}, {76.79f, 83.07f, 84.2f, 87.84f, 88.08f, 92.19f}},
        {{"Rafael", "Don", "Adam", "Joe", "MarkM", "MarkM"}, {238.1f, 241.12f, 242.01f, 248.1f, 249.03f, 256.5f}},
        {{"Scott", "Scott", "McNabb", "McNabb", "CBryan", "CBryan"}, {76.99f, 77.48f, 80.44f, 82.75f, 83.84f, 86.94f}},
        {{"Scott", "McNabb", "McNabb", "Nick", "Nick", "Jessica"}, {233.98f, 234.14f, 236.55f, 237.9f, 238.81f, 247.17f}},
        {{"Bradicus", "Bradicus", "Robb", "Tom", "Bals", "Roman"}, {117.85f, 119.3f, 122.44f, 123.04f, 124.63f, 126.06f}},
        {{"Bradicus", "Robb", "Bradicus", "Tom", "Roman", "Roman"}, {361.29f, 363.03f, 367.89f, 369.1f, 372.2f, 377.43f}},
        {{"Glenn", "MarkM", "MarkM", "Amy Lugo", "Bradicus", "Jose M"}, {75.22f, 76.07f, 78.33f, 80.44f, 82.87f, 86.03f}},
        {{"Glenn", "MarkM", "MarkM", "Amy Lugo", "Brandon", "Jose M"}, {228.27f, 232.03f, 233.3f, 234.03f, 237.1f, 237.98f}},
        {{"Wil", "Wil", "Bals", "Shari", "Shari", "Dennis"}, {78.07f, 80.22f, 82.44f, 84.47f, 85.55f, 89.04f}},
        {{"Wil", "Bals", "Dennis", "McNabb", "McNabb", "Dennis"}, {236.14f, 237.11f, 239.9f, 242.78f, 244.89f, 249.91f}},
    }};
    *(Raw42 *)&m_hallOfFame.m_table0 = *(Raw42 *)&track0;
    HallOfFameTable42 track1 = {{
        {{"Hiro", "Jordan", "Bradicus", "Bradicus", "Bradley", "Haily"}, {49.38f, 49.45f, 50.89f, 52.47f, 52.97f, 54.74f}},
        {{"Bradicus", "Haily", "Bradley", "Adam", "Adam", "Tara"}, {150.9f, 152.2f, 158.33f, 160.9f, 161.89f, 163.1f}},
        {{"MarkM", "Alison", "MarkM", "Adam", "Yukari", "Jason"}, {55.65f, 55.77f, 60.11f, 60.23f, 61.88f, 61.93f}},
        {{"Adam", "MarkM", "Adam", "Jason", "Jason", "Adam"}, {178.61f, 180.02f, 181.89f, 182.02f, 183.01f, 184.82f}},
        {{"Don", "Hiro", "Andrew", "Rafael", "Robb", "Adam"}, {73.66f, 73.97f, 74.45f, 76.85f, 76.93f, 80.69f}},
        {{"Don", "Don", "Andrew", "Eric C", "Adam", "Eric"}, {224.69f, 232.01f, 246.77f, 249.87f, 251.22f, 253.13f}},
        {{"Robb", "Hiro", "MattyHo", "JDub", "Kyoko H", "Zunma"}, {77.75f, 78.23f, 78.73f, 79.03f, 79.55f, 80.9f}},
        {{"Robb", "Hiro", "MattyHo", "Goat", "Goat", "Levi"}, {245.94f, 248.08f, 252.2f, 254.38f, 259.98f, 261.02f}},
        {{"Adam", "Akira", "Kawata", "Nick", "Jessica", "Adam"}, {62.74f, 63.42f, 63.95f, 64.02f, 67.04f, 69.65f}},
        {{"Adam", "Nick", "Nick", "Jessica", "Robb", "Bailey"}, {195.39f, 199.99f, 205.25f, 206.89f, 210.9f, 215.98f}},
        {{"Hiro", "Robb", "MikeChow", "Houman", "DLow", "Susan"}, {63.94f, 64.84f, 65.06f, 65.07f, 66.88f, 66.92f}},
        {{"Tom", "Robb", "Robb", "Houman", "DLow", "Susan"}, {205.72f, 207.78f, 210.89f, 211.9f, 215.82f, 218.73f}},
        {{"Jordan", "Hiro", "Brandon", "Heather", "Tamaki", "Akashi"}, {41.65f, 41.94f, 42.77f, 42.94f, 43.33f, 45.46f}},
        {{"Jordan", "Brandon", "Nuh", "Brandon", "Paddy", "Nuh"}, {136.14f, 138.21f, 140.9f, 141.27f, 150.9f, 155.22f}},
        {{"Robb", "Hiro", "Scott", "Matthew", "Koh", "Satoh"}, {61.46f, 61.66f, 62.91f, 63.84f, 65.12f, 66.44f}},
        {{"Scott", "Matthew", "Ryan", "Ryan", "Matthew", "Ryan"}, {190.06f, 195.11f, 196.62f, 197.92f, 200.77f, 201.82f}},
        {{"Hiro", "Yukari", "Robb", "Bradicus", "Bradicus", "Jose M"}, {34.85f, 35.88f, 36.96f, 37.21f, 38.93f, 40.76f}},
        {{"Hiro", "Robb", "Bradicus", "Bradicus", "Jose M", "Amy Lugo"}, {105.19f, 107.99f, 111.3f, 115.02f, 118.22f, 120.2f}},
        {{"Rafael", "Rafael", "Mickael", "Olivier", "Scott", "Scott"}, {56.89f, 57.01f, 57.87f, 59.28f, 60.85f, 61.03f}},
        {{"Rafael", "Adeline", "Mickael", "Eric", "Olivier", "Scott"}, {179.23f, 180.2f, 181.23f, 185.67f, 186.27f, 187.52f}},
        {{"Jose M", "Hiro", "Amy Lugo", "Amy Lugo", "Rick", "Evan"}, {57.99f, 58.95f, 59.83f, 60.84f, 62.65f, 63.5f}},
        {{"Jose M", "Amy Lugo", "Rick", "Rick", "Lila", "Evan"}, {182.11f, 183.02f, 186.44f, 190.03f, 191.12f, 193.77f}},
        {{"Wil", "Roman", "Jordan", "Jordan", "Bals", "Wil"}, {82.77f, 82.88f, 83.94f, 84.32f, 85.3f, 85.77f}},
        {{"Wil", "Roman", "Bals", "Wil", "Jordan", "Wil"}, {256.18f, 258.88f, 260.91f, 261.81f, 262.94f, 266.54f}},
        {{"Scott", "Hiro", "Scott", "MarkD", "MarkD", "Michelle"}, {72.13f, 72.93f, 73.74f, 75.44f, 75.81f, 77.57f}},
        {{"Scott", "Hiro", "Scott", "MarkD", "Luca", "Luca"}, {222.32f, 224.22f, 224.4f, 230.88f, 231.98f, 233.91f}},
        {{"Hiro", "Adam", "Robb", "Tadashi", "Scott", "Matthew"}, {93.38f, 94.93f, 95.12f, 95.98f, 96.42f, 97.48f}},
        {{"Adam", "Tom S", "Robb", "Scott", "Matthew", "Ryan"}, {285.25f, 289.91f, 291.34f, 294.94f, 299.3f, 301.01f}},
        {{"Hiro", "Adam", "Caitlin", "Glenn", "Glenn", "Devin"}, {78.99f, 79.55f, 80.83f, 81.88f, 83.84f, 84.91f}},
        {{"Glenn", "Adam", "Glenn", "Glenn", "Devin", "Caitlin"}, {250.44f, 251.65f, 253.1f, 255.91f, 260.17f, 261.81f}},
        {{"Hiro", "Robb", "Adam", "Robb", "Jeff", "Jeff"}, {57.9f, 58.73f, 59.8f, 60.02f, 62.13f, 64.84f}},
        {{"Robb", "Adam", "Jeff", "Jeff", "Adam", "Adam"}, {181.0f, 182.59f, 182.99f, 184.0f, 185.2f, 188.01f}},
        {{"Jordan", "Zach", "Kronk", "Heather", "Heather", "Carrie"}, {72.86f, 72.93f, 75.4f, 76.8f, 77.53f, 80.59f}},
        {{"Zach", "Jordan", "Brandon", "Brandon", "Carrie", "Brandon"}, {183.13f, 188.2f, 191.02f, 194.44f, 195.71f, 199.33f}},
        {{"Jun", "Amamiya", "Eric C", "Jon", "Wil", "Lisa"}, {67.4f, 67.93f, 68.42f, 69.56f, 71.44f, 72.01f}},
        {{"Wil", "Eric", "Eric C", "Jon", "Wil", "Wil"}, {210.71f, 212.1f, 215.91f, 218.14f, 220.99f, 222.12f}},
        {{"Jordan", "Bradicus", "Adam", "Adam", "Bradicus", "Adam"}, {109.99f, 110.44f, 111.42f, 112.02f, 115.24f, 117.4f}},
        {{"Adam", "Bradicus", "Yoshio", "Toryo", "Bradicus", "Glenn"}, {340.22f, 341.12f, 345.91f, 350.38f, 351.88f, 360.22f}},
        {{"Hiro", "Scott", "Robb", "Scott", "Scott", "MarkD"}, {72.81f, 72.92f, 74.75f, 75.08f, 76.29f, 77.01f}},
        {{"Scott", "Scott", "MarkD", "Scott", "Scott", "MarkD"}, {219.25f, 222.01f, 223.91f, 225.81f, 230.52f, 235.01f}},
        {{"Eebie", "Skeebie", "Adam", "Lisa", "Bals", "Shari"}, {74.01f, 74.07f, 75.12f, 75.14f, 77.48f, 78.69f}},
        {{"Kronk", "Adam", "Bals", "Eric C", "Shari", "Bals"}, {226.12f, 228.01f, 230.83f, 231.13f, 234.62f, 234.91f}},
    }};
    *(Raw42 *)&m_hallOfFame.m_table1 = *(Raw42 *)&track1;
    HallOfFameTable21 track2 = {{
        {{"Hiro", "Robb", "Glenn", "Glenn", "Glenn", "Devin"}, {33.59f, 34.02f, 36.33f, 36.89f, 38.02f, 39.01f}},
        {{"Glenn", "Robb", "Tayde", "Glenn", "Caitlin", "Devin"}, {57.32f, 58.34f, 59.01f, 61.55f, 62.77f, 64.92f}},
        {{"Scott", "Scott", "Robb", "Scott", "Jordan", "Tom"}, {62.02f, 63.45f, 65.45f, 67.02f, 67.88f, 69.4f}},
        {{"Hiro", "Adam", "Adam", "Adam", "Scott", "MarkM"}, {82.25f, 83.01f, 83.19f, 84.33f, 84.9f, 86.47f}},
        {{"Tom", "Tom", "Scott", "Rick", "Adam", "Rick"}, {65.22f, 65.97f, 67.33f, 68.9f, 70.02f, 73.04f}},
        {{"Adam", "Hiro", "Hiro", "Adam", "MarkM", "MarkM"}, {65.93f, 66.03f, 67.44f, 68.9f, 72.04f, 78.84f}},
        {{"Hiro", "Hiro", "MarkM", "Martha", "Adam", "Adam"}, {55.9f, 57.33f, 58.44f, 60.02f, 60.8f, 63.03f}},
        {{"Scott", "Scott", "Robb", "MarkM", "Jose M", "Adam"}, {62.31f, 64.03f, 64.9f, 65.74f, 66.03f, 66.34f}},
        {{"Robb", "Robb", "Hiro", "Rick", "Rick", "Bradicus"}, {41.49f, 43.33f, 44.32f, 46.9f, 46.98f, 49.14f}},
        {{"Bradicus", "Robb", "Adam", "Adam", "Bradicus", "Scott"}, {55.56f, 56.02f, 58.5f, 58.89f, 60.7f, 61.01f}},
        {{"Glenn", "Scott", "Glenn", "Devin", "Scott", "Caitlin"}, {60.42f, 61.11f, 63.01f, 63.9f, 66.77f, 69.04f}},
        {{"MarkM", "Glenn", "Tim", "Tim", "Bill", "Bill"}, {82.38f, 82.91f, 84.4f, 84.9f, 85.6f, 88.72f}},
        {{"Tom", "Hiro", "Hiro", "Tom", "Hiro", "Tom"}, {75.23f, 77.66f, 78.18f, 78.88f, 79.92f, 81.33f}},
        {{"Bradicus", "Bradicus", "MarkD", "Rick", "Tom", "Tom"}, {93.93f, 95.55f, 95.91f, 98.01f, 98.43f, 100.22f}},
        {{"Hiro", "Robb", "Adam", "Adam", "Robb", "Adam"}, {75.02f, 75.93f, 77.22f, 77.29f, 79.01f, 81.23f}},
        {{"Adam", "Adam", "Rick", "Brandon", "Jose M", "Glenn"}, {60.56f, 61.01f, 63.33f, 65.5f, 66.72f, 66.9f}},
        {{"Bill", "Rick", "Adam", "Glenn", "Glenn", "Glenn"}, {76.01f, 76.67f, 77.22f, 78.9f, 80.14f, 85.19f}},
        {{"Tim", "Rick", "Scott", "Scott", "Glenn", "Glenn"}, {76.8f, 77.21f, 78.18f, 79.3f, 79.34f, 86.12f}},
        {{"Jose M", "Brandon", "Bradicus", "Jon", "Jon", "Bradicus"}, {117.82f, 118.81f, 120.01f, 124.41f, 124.78f, 126.61f}},
        {{"Wil", "Roman", "Eric", "Wil", "Roman", "Roman"}, {74.98f, 75.21f, 75.88f, 77.14f, 77.38f, 82.52f}},
        {{"Moms", "Dads", "Hubbies", "Wives", "Kids", "Sig O"}, {78.7f, 82.34f, 82.44f, 84.4f, 85.15f, 85.55f}},
    }};
    *(Raw21 *)&m_hallOfFame.m_table2 = *(Raw21 *)&track2;
    HallOfFameTable21 track3 = {{
        {{"Hiro", "Dennis", "McNabb", "McNabb", "Jose M", "Dennis"}, {48.38f, 50.14f, 52.55f, 52.88f, 53.03f, 53.34f}},
        {{"Hiro", "Bals", "Bals", "Adam", "DLow", "Bals"}, {54.64f, 60.14f, 61.47f, 61.99f, 62.22f, 62.67f}},
        {{"Hiro", "Bals", "Glenn", "Glenn", "Dan", "DLow"}, {72.65f, 73.78f, 75.11f, 78.45f, 80.33f, 81.6f}},
        {{"Hiro", "Adam", "Glenn", "Glenn", "Scott", "Dan"}, {76.75f, 81.8f, 83.22f, 84.48f, 85.51f, 85.71f}},
        {{"Hiro", "Stephane", "Robb", "Stephane", "Dennis", "Jose M"}, {60.74f, 72.9f, 73.33f, 75.67f, 79.8f, 80.19f}},
        {{"Hiro", "Robb", "Adam", "MarkM", "DLow", "DLow"}, {63.04f, 65.69f, 67.11f, 69.29f, 70.39f, 71.45f}},
        {{"Hiro", "Dan", "Dave F", "Hiro", "Dan", "DLow"}, {40.66f, 59.19f, 60.22f, 61.23f, 61.91f, 63.49f}},
        {{"Hiro", "Sandy", "Earl", "Earl", "JGLee", "Sandy"}, {60.45f, 62.33f, 63.87f, 64.02f, 64.44f, 66.1f}},
        {{"Hiro", "Robb", "Robb", "Stephane", "Stephane", "Robb"}, {33.85f, 35.11f, 36.93f, 39.33f, 40.56f, 41.67f}},
        {{"Hiro", "Adam", "Stephane", "Robb", "Stephane", "Robb"}, {55.89f, 56.78f, 57.84f, 58.92f, 59.12f, 59.27f}},
        {{"Hiro", "McNabb", "Dennis", "Jose M", "Adam", "McNabb"}, {56.09f, 58.18f, 59.11f, 60.18f, 61.1f, 63.45f}},
        {{"Hiro", "Amy Lugo", "Glenn", "Jose M", "MikeChow", "Matt"}, {81.67f, 81.71f, 82.18f, 83.22f, 84.56f, 85.94f}},
        {{"Hiro", "Bals", "Eric C", "Tom", "Rick", "Houman"}, {71.73f, 73.74f, 74.98f, 75.44f, 76.9f, 80.12f}},
        {{"Hiro", "Glenn", "Roman", "Bradicus", "Scott", "Roman"}, {92.38f, 93.99f, 94.04f, 94.94f, 97.17f, 97.83f}},
        {{"Hiro", "Scott", "Robb", "MarkM", "MarkM", "Alison"}, {78.46f, 83.3f, 84.0f, 85.5f, 85.9f, 86.02f}},
        {{"Hiro", "Amarinth", "DLow", "Earl", "JGLee", "Jallak"}, {57.6f, 58.99f, 59.01f, 59.76f, 60.01f, 61.46f}},
        {{"Hiro", "Jordan", "Robb", "Robb", "Robb", "Jordan"}, {71.86f, 73.64f, 74.15f, 75.5f, 76.74f, 77.8f}},
        {{"Hiro", "Bradicus", "Bradley", "Roman", "Haily", "Haily"}, {65.4f, 74.9f, 75.12f, 76.33f, 78.22f, 78.93f}},
        {{"Hiro", "Robb", "Glenn", "Hiro", "Glenn", "Jose M"}, {108.75f, 110.2f, 112.8f, 115.92f, 118.3f, 120.44f}},
        {{"Hiro", "Robb", "Erin", "Raquel", "Robb", "Manini"}, {71.8f, 72.49f, 75.03f, 76.12f, 77.0f, 77.88f}},
        {{"Hiro", "Eric C", "Hiro", "Adam", "Tara", "Adam"}, {73.01f, 74.93f, 75.01f, 76.33f, 78.42f, 86.01f}},
    }};
    *(Raw21 *)&m_hallOfFame.m_table3 = *(Raw21 *)&track3;
    HallOfFameTable42 track4 = {{
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"Raquel", "Andrew", "Jordan", "Andrew", "Jason", "Elmo"}, {656.26f, 631.13f, 628.81f, 626.14f, 451.12f, 345.45f}},
        {{"Manini", "Andrew", "Andrew", "Jordan", "MarkM", "Elmo"}, {2.67648e-43f, 2.63444e-43f, 2.62043e-43f, 2.39622e-43f, 2.18603e-43f, 2.12997e-43f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"Jordan", "Andrew", "Andrew", "Joe", "Scott", "MarkM"}, {898.51f, 888.15f, 827.38f, 790.1f, 781.0f, 721.0f}},
        {{"Jordan", "Andrew", "Andrew", "Joe", "Scott", "MarkM"}, {4.93257e-43f, 4.90454e-43f, 4.38606e-43f, 4.03574e-43f, 3.79752e-43f, 3.25101e-43f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"Jordan", "Jordan", "Adam", "MarkM", "Rick", "Rick"}, {917.39f, 903.98f, 730.02f, 653.5f, 481.87f, 471.01f}},
        {{"Jordan", "Jordan", "Rick", "Adam", "Rick", "Brad"}, {4.90454e-43f, 2.45227e-43f, 2.12997e-43f, 2.07392e-43f, 1.84971e-43f, 1.69557e-43f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"Jordan", "Adam", "MarkM", "Eric", "Eric C", "Rick"}, {632.75f, 614.01f, 592.03f, 532.22f, 512.14f, 499.01f}},
        {{"Jordan", "Adam", "Mark", "Eric C", "Jose M", "Rick"}, {2.64845e-43f, 2.39622e-43f, 2.17201e-43f, 1.86373e-43f, 1.84971e-43f, 1.59748e-43f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"Jordan", "MarkM", "Scott", "Tim", "Roman", "Jason"}, {721.63f, 690.12f, 634.13f, 621.32f, 548.14f, 532.78f}},
        {{"Jordan", "MarkM", "Scott", "Tim", "Jose M", "Shari"}, {2.84464e-43f, 2.49431e-43f, 2.34017e-43f, 2.03188e-43f, 1.98984e-43f, 1.84971e-43f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"Jordan", "Andrew", "Andrew", "Jordan", "Andrew", "Rick"}, {680.67f, 666.14f, 647.34f, 640.71f, 619.28f, 376.89f}},
        {{"Jordan", "Jordan", "Andrew", "Andrew", "Andrew", "Rick"}, {2.76056e-43f, 2.70451e-43f, 2.69049e-43f, 2.64845e-43f, 2.57839e-43f, 1.68156e-43f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
        {{"", "", "", "", "", ""}, {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}},
    }};
    *(Raw42 *)&m_hallOfFame.m_table4 = *(Raw42 *)&track4;
    *(Raw42 *)&m_hallOfFame.m_table5 = *(Raw42 *)&track4;
}


// ---- src/JetskiGameOptions/SetUI.cpp

void JetskiGameOptions::SetUI(int a, int b)
{
    m_uiMode = a;
    m_x28 = b;
}


// ---- src/JetskiGameOptions/SetCareerPath.cpp
// Register colouring: i and round are shared by both loops, so mwcc's web splitting
// (IROUseDef) gives their second-loop live ranges fresh temps, in first-appearance order;
// round's dead initialiser makes it appear before i.




void JetskiGameOptions::SetCareerPath(int difficulty, int player)
{
    char name[0x20];
    int i;
    CareerPath *path = &m_profile.m_slots[player].m_careerPath;
    CareerRound *round = path->m_rounds;
    ParameterBlock *params = new ParameterBlock;
    switch (difficulty) {
    case 1:
        params->OpenFile("UI\\CEasy.dat", 0);
        break;
    case 2:
        params->OpenFile("UI\\CNormal.dat", 0);
        break;
    case 3:
        params->OpenFile("UI\\CHard.dat", 0);
        break;
    default:
        // No file: twenty default rounds. (The block is leaked.)
        path->m_numRounds = 20;
        for (i = 0; i < 20; i++) {
            round = &path->m_rounds[i];
            round->m_type = 6;
            round->m_scene = 1;
            round->m_pointsToPass = 0;
            round->m_qualifyTimes[0] = 0.0f;
            round->m_qualifyTimes[1] = 0.0f;
            round->m_qualifyTimes[2] = 0.0f;
            round->m_qualifyTimes[3] = 0.0f;
            round->m_qualifyTimes[4] = 0.0f;
        }
        return;
    }
    params->ReadParameterBlock("General");
    params->GetParameter("CareerLength", 12, &path->m_numRounds);
    if (path->m_numRounds > 20)
        path->m_numRounds = 20;
    for (i = 0; i < path->m_numRounds; i++) {
        round = &path->m_rounds[i];
        sprintf(name, "Round %d", i + 1);
        params->ReadParameterBlock(name);
        params->GetParameter("RoundType", "COMPETITIONTYPE_CAREER", name, -1);
        if (!strcasecmp(name, "COMPETITIONTYPE_CAREER"))
            round->m_type = 6;
        else
            round->m_type = 7;
        params->GetParameter("SceneNumber", 1, &round->m_scene);
        params->GetParameter("PointsToPass", 0, &round->m_pointsToPass);
        params->GetParameter("QualifyTime1", 0.0f, &round->m_qualifyTimes[0]);
        params->GetParameter("QualifyTime2", 0.0f, &round->m_qualifyTimes[1]);
        params->GetParameter("QualifyTime3", 0.0f, &round->m_qualifyTimes[2]);
        params->GetParameter("QualifyTime4", 0.0f, &round->m_qualifyTimes[3]);
        params->GetParameter("QualifyTime5", 0.0f, &round->m_qualifyTimes[4]);
    }
    delete params;
}


// ---- src/JetskiGameOptions/GetVehicleID.cpp

int JetskiGameOptions::GetVehicleID(int vehicle)
{
    int id = -1;
    switch (vehicle) {
    case 0: id = 1; break;
    case 4: id = 2; break;
    case 2: id = 3; break;
    case 3: id = 4; break;
    case 1: id = 5; break;
    case 5: id = 6; break;
    case 6: id = 7; break;
    case 7: id = 8; break;
    case 8: id = 9; break;
    }
    return id - 1;
}


// ---- src/JetskiGameOptions/SetUpSkinIndices.cpp


// Looks up the material indices of every character's rider and jetski skins.
void JetskiGameOptions::SetUpSkinIndices()
{
    int base;
    int character;
    int k;
    int j;
    for (character = 0; character < 9; character++) {
        for (k = 0; k < 2; k++) {
            switch (k) {
            case 0:
                for (j = 0; j < 8; j++) {
                    switch (character) {
                    case 0:
                        base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\DerrBody");
                        m_riderSkins[character][k][j] = base + j;
                        break;
                    case 4:
                        base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\JuanaBody");
                        m_riderSkins[character][k][j] = base + j;
                        break;
                    case 2:
                        base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\AndreasBody");
                        m_riderSkins[character][k][j] = base + j;
                        break;
                    case 3:
                        base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\UmiBody");
                        m_riderSkins[character][k][j] = base + j;
                        break;
                    case 1:
                        base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\BrittBody");
                        m_riderSkins[character][k][j] = base + j;
                        break;
                    case 5:
                        base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SebastianBody");
                        m_riderSkins[character][k][j] = base + j;
                        break;
                    case 6:
                        base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\JeremyBody");
                        m_riderSkins[character][k][j] = base + j;
                        break;
                    case 7:
                        base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\AmmanRaBody");
                        m_riderSkins[character][k][j] = base + j;
                        break;
                    case 8:
                        switch (j) {
                        case 1:
                            m_riderSkins[character][k][j] = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SteveBody_D2");
                            break;
                        case 7:
                            m_riderSkins[character][k][j] = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SteveBody_SWM");
                            break;
                        default:
                            m_riderSkins[character][k][j] = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SteveBody");
                            break;
                        }
                        break;
                    }
                }
                break;
            case 1:
                for (j = 0; j < 8; j++) {
                    switch (character) {
                    case 0:
                        base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\DerrFace");
                        m_riderSkins[character][k][j] = base + j;
                        break;
                    case 4:
                        base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\JuanaFace");
                        m_riderSkins[character][k][j] = base + j;
                        break;
                    case 2:
                        base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\AndreasFace");
                        m_riderSkins[character][k][j] = base + j;
                        break;
                    case 3:
                        base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\UmiFace");
                        m_riderSkins[character][k][j] = base + j;
                        break;
                    case 1:
                        base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\BrittFace");
                        m_riderSkins[character][k][j] = base + j;
                        break;
                    case 5:
                        base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SebastianFace");
                        m_riderSkins[character][k][j] = base + j;
                        break;
                    case 6:
                        base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\JeremyFace");
                        m_riderSkins[character][k][j] = base + j;
                        break;
                    case 7:
                        base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\AmmanRaFace");
                        m_riderSkins[character][k][j] = base + j;
                        break;
                    case 8:
                        switch (j) {
                        case 1:
                            m_riderSkins[character][k][j] = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SteveFace_D2");
                            break;
                        case 7:
                            m_riderSkins[character][k][j] = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SteveFace_SWM");
                            break;
                        default:
                            m_riderSkins[character][k][j] = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SteveFace");
                            break;
                        }
                        break;
                    }
                }
                break;
            }
        }
    }
    for (character = 0; character < 9; character++) {
        for (k = 0; k < 2; k++) {
            switch (k) {
            case 0:
                for (j = 0; j < 8; j++) {
                    switch (j) {
                    case 0:
                    case 7:
                        switch (character) {
                        case 0:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooRX_Derr_1");
                            break;
                        case 4:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooGTX_Juana_1");
                            break;
                        case 2:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooXP_Andreas_1");
                            break;
                        case 3:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooSPX_Umi_1");
                            break;
                        case 1:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooRX_Britt_1");
                            break;
                        case 5:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooGTX_Sebas_1");
                            break;
                        case 6:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooSPX_Jeremy_1");
                            break;
                        case 7:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooXP_AmmanRa_1");
                            break;
                        case 8:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SWC_1");
                            break;
                        }
                        m_jetskiSkins[character][k][j] = base;
                        break;
                    case 1:
                        switch (character) {
                        case 0:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooRX_Derr_1_D2");
                            break;
                        case 4:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooGTX_Juana_1_D2");
                            break;
                        case 2:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooXP_Andreas_1_D2");
                            break;
                        case 3:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooSPX_Umi_1_D2");
                            break;
                        case 1:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooRX_Britt_1_D2");
                            break;
                        case 5:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooGTX_Sebas_1_D2");
                            break;
                        case 6:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooSPX_Jeremy_1_D2");
                            break;
                        case 7:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooXP_AmmanRa_1_D2");
                            break;
                        case 8:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SWC_1_D2");
                            break;
                        }
                        m_jetskiSkins[character][k][j] = base;
                        break;
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                        switch (character) {
                        case 0:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooRX_1_Red");
                            break;
                        case 4:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooGTX_1_Red");
                            break;
                        case 2:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooXP_1_Red");
                            break;
                        case 3:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooSPX_1_Red");
                            break;
                        case 1:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooRX_1_Red");
                            break;
                        case 5:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooGTX_1_Red");
                            break;
                        case 6:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooSPX_1_Red");
                            break;
                        case 7:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooXP_1_Red");
                            break;
                        case 8:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SWC_1_Red");
                            break;
                        }
                        m_jetskiSkins[character][k][j] = base + (j - 2);
                        break;
                    }
                }
                break;
            case 1:
                for (j = 0; j < 8; j++) {
                    switch (j) {
                    case 0:
                    case 7:
                        switch (character) {
                        case 0:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooRX_Derr_2");
                            break;
                        case 4:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooGTX_Juana_2");
                            break;
                        case 2:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooXP_Andreas_2");
                            break;
                        case 3:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooSPX_Umi_2");
                            break;
                        case 1:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooRX_Britt_2");
                            break;
                        case 5:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooGTX_Sebas_2");
                            break;
                        case 6:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooSPX_Jeremy_2");
                            break;
                        case 7:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooXP_AmmanRa_2");
                            break;
                        case 8:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SWC_2");
                            break;
                        }
                        m_jetskiSkins[character][k][j] = base;
                        break;
                    case 1:
                        switch (character) {
                        case 0:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooRX_Derr_2_D2");
                            break;
                        case 4:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooGTX_Juana_2_D2");
                            break;
                        case 2:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooXP_Andreas_2_D2");
                            break;
                        case 3:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooSPX_Umi_2_D2");
                            break;
                        case 1:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooRX_Britt_2_D2");
                            break;
                        case 5:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooGTX_Sebas_2_D2");
                            break;
                        case 6:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooSPX_Jeremy_2_D2");
                            break;
                        case 7:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooXP_AmmanRa_2_D2");
                            break;
                        case 8:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SWC_2_D2");
                            break;
                        }
                        m_jetskiSkins[character][k][j] = base;
                        break;
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                        switch (character) {
                        case 0:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooRX_2_Red");
                            break;
                        case 4:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooGTX_2_Red");
                            break;
                        case 2:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooXP_2_Red");
                            break;
                        case 3:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooSPX_2_Red");
                            break;
                        case 1:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooRX_2_Red");
                            break;
                        case 5:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooGTX_2_Red");
                            break;
                        case 6:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooSPX_2_Red");
                            break;
                        case 7:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SeaDooXP_2_Red");
                            break;
                        case 8:
                            base = LpMaterialStringTable->GetIndex("Scenes\\Geometry\\Textures\\SWC_2_Red");
                            break;
                        }
                        m_jetskiSkins[character][k][j] = base + (j - 2);
                        break;
                    }
                }
                break;
            }
        }
    }
}


// ---- src/JetskiGameOptions/GetCharacterName.cpp






// Localized name of a selectable character.
void JetskiGameOptions::GetCharacterName(int character, char *name)
{
    switch (character) {
    case 0:
        strcpy(name, LpGame->m_text->GetString(14));
        break;
    case 4:
        strcpy(name, LpGame->m_text->GetString(15));
        break;
    case 2:
        strcpy(name, LpGame->m_text->GetString(16));
        break;
    case 3:
        strcpy(name, LpGame->m_text->GetString(17));
        break;
    case 1:
        strcpy(name, LpGame->m_text->GetString(18));
        break;
    case 5:
        strcpy(name, LpGame->m_text->GetString(19));
        break;
    case 6:
        strcpy(name, LpGame->m_text->GetString(20));
        break;
    case 7:
        strcpy(name, LpGame->m_text->GetString(21));
        break;
    case 8:
        strcpy(name, LpGame->m_text->GetString(22));
        break;
    }
}


// ---- src/JetskiGameOptions/ChangeControllerPreset.cpp

// Button for each of the 23 actions under the four controller presets.
int JetskiGameOptions::ChangeControllerPreset(int preset, int pad)
{
    switch (preset) {
    case 1:
        m_controllerPreset[pad] = 1;
        LpGame->m_controls->m_keyLookupTable->MapKeys(DEVICE_JOYSTICK, pad, 15, 13, 12, 14, 2, 3, 0, 1, 9, 8, 6, 7, 4, 5,
                                             11, 10, 2, 3, 0, 6, 4, 7, 5, -1);
        break;
    case 2:
        m_controllerPreset[pad] = 2;
        LpGame->m_controls->m_keyLookupTable->MapKeys(DEVICE_JOYSTICK, pad, 15, 13, 12, 14, 2, 3, 0, 1, 9, 8, 6, 7, 4, 5,
                                             11, 10, 2, 7, 5, 6, 3, 0, 1, -1);
        break;
    case 3:
        m_controllerPreset[pad] = 3;
        LpGame->m_controls->m_keyLookupTable->MapKeys(DEVICE_JOYSTICK, pad, 15, 13, 12, 14, 2, 3, 0, 1, 9, 8, 6, 7, 4, 5,
                                             11, 10, 5, 4, 7, 6, 0, 3, 2, -1);
        break;
    case 4:
        m_controllerPreset[pad] = 4;
        LpGame->m_controls->m_keyLookupTable->MapKeys(DEVICE_JOYSTICK, pad, 15, 13, 12, 14, 2, 3, 0, 1, 9, 8, 6, 7, 4, 5,
                                             11, 10, 7, 3, 0, 2, 6, 4, 5, -1);
        break;
    default:
        return 0;
    }
    return 1;
}


// ---- src/JetskiGameOptions/CheatUnlockEverything.cpp

void JetskiGameOptions::CheatUnlockEverything()
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 8; j++)
            m_secretSkinUnlocked[i][j] = 1;
    }
    CheatUnlockAllMovies();
    CheatUnlockAllTracks();
    CheatUnlockAllCharacters();
}


// ---- src/JetskiGameOptions/CheatUnlockAllTracks.cpp
void JetskiGameOptions::CheatUnlockAllTracks()
{
    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < 21; i++)
            m_trackUnlocked[j][i] = 1;
    }
}


// ---- src/JetskiGameOptions/CheatUnlockAllCharacters.cpp

void JetskiGameOptions::CheatUnlockAllCharacters()
{
    for (int i = 0; i < 9; i++)
        m_characterUnlocked[i] = 1;
}


// ---- src/JetskiGameOptions/CheatUnlockAllMovies.cpp
void JetskiGameOptions::CheatUnlockAllMovies()
{
    for (int i = 0; i < 10; i++)
        m_movieUnlocked[i] = 1;
}


// ---- src/JetskiGameOptions/CheatFullPerformance.cpp



void JetskiGameOptions::CheatFullPerformance()
{
    LpGame->m_cheats->fullPerformance = 1;
}


// ---- src/JetskiGameOptions/CheatAllOutAI.cpp



void JetskiGameOptions::CheatAllOutAI()
{
    LpGame->m_cheats->allOutAI = 1;
}


// ---- src/JetskiGameOptions/CheatHardToNormalAI.cpp



void JetskiGameOptions::CheatHardToNormalAI()
{
    LpGame->m_cheats->hardToNormalAI = 1;
}


// ---- src/JetskiGameOptions/CheatNoJoust.cpp



void JetskiGameOptions::CheatNoJoust()
{
    LpGame->m_cheats->noJoust = 1;
}


// ---- src/JetskiGameOptions/SetupAIRacers.cpp
// Fills the racers after the human players with random unused characters.
// Character 8 is only picked 5% of the time it comes up.
void JetskiGameOptions::SetupAIRacers(int numHumans)
{
    JetskiGameOptions *options = LpGame->m_gameOptions;
    int used[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    if (options->m_difficulty != 3 || !options->m_characterUnlocked[8])
        used[8] = 1;
    if (numHumans > 0) {
        used[options->m_racerCharacter[0]] = 1;
        if (numHumans > 1)
            used[options->m_racerCharacter[1]] = 1;
    }
    int racer;
    int retry;
    int character;
    int i;
    for (i = 0; i < 5 - numHumans; i++) {
        retry = 1;
        racer = numHumans + i;
        do {
            character = 9.0f * (rand() * 4.656613e-10f);
            if (!used[character]) {
                // The redundant `character == 8` test is in retail.
                if (character != 8 || (character == 8 && (int)(100.0f * (rand() * 4.656613e-10f)) < 5)) {
                    used[character] = 1;
                    retry = 0;
                    options->SetRacer(racer, character, 7.0f * (rand() * 4.656613e-10f));
                }
            }
        } while (retry);
    }
}


// ---- src/JetskiUserOptions/ctor.cpp
JetskiUserOptions::JetskiUserOptions()
{
    m_x10 = 1;
}


// ---- src/JetskiUserOptions/dtor.cpp
JetskiUserOptions::~JetskiUserOptions()
{
}


// ---- src/sinit/JetskiGameOptions.cpp
// Static initializer of JetskiGameOptions.cpp: only the axis vectors from the shared header.

