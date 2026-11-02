// Localization.cpp  @ 0x00395380 (text 0x395380-0x395CA0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's Localization.cpp as one translation unit: GameText's methods
// (their ASSERTs' __FILE__ is "Localization.cpp"), a TU without a static
// initializer inside ParticleVUPhysics.cpp's tu_map row, after
// ParticleVUPhysics.cpp and before LoadingIcon.cpp. GameText's ctor was a
// C-mode stub in src/JetskiTraining/UnloadIntroDialog.cpp.



#include "soultree_assert_block.h"
#include "GameText/GameText.h"
#include "RenderTarget/RenderTarget.h"

extern "C" void free(void *p);
extern "C" void *malloc(unsigned int size);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" int sprintf(char *buf, const char *fmt, ...);
void PSXprintf(const char *fmt, ...);

FILE_HANDLE_STRUCT *FOPEN(char *name, const char *mode);
int FSEEK(FILE_HANDLE_STRUCT *f, long offset, int whence);
unsigned long FTELL(FILE_HANDLE_STRUCT *f);
int FREAD(void *buf, unsigned int size, FILE_HANDLE_STRUCT *f);
int FCLOSE(FILE_HANDLE_STRUCT *f);

extern char RootDirectory[];
extern char LocalizationContentDirectory[];
extern RenderTarget *LpRenderTarget;

// strchr-style helper; called with c == 0 it finds the terminator.
static inline char *FindChar(char *s, char c)
{
    while (*s && *s != c)
        s++;
    return *s == c ? s : 0;
}

GameText::GameText()
{
    m_data = 0;
    m_strings = 0;
    m_numStrings = 0;
}

GameText::~GameText()
{
    if (m_data)
        free(m_data);
    if (m_strings)
        delete[] m_strings;
}

// An entry of the .loc index as read from the file.
struct GameTextString {
    PS2_PTR(char) m_text;           // +0x0 (a file field: 32 bits)
    short m_length;                 // +0x4
};

// Reads the string table (count, then a length per ID) of a .loc file.
int GameText::Create(char *name, Language language)
{
    char path[0x10C];
    int offset;

    strcpy(m_name, name);
    if (language < 0) {
        sprintf(path, "%s%s%s.loc", RootDirectory, LocalizationContentDirectory, m_name);
    } else {
        const char *dir;
        if (language == 2)
            dir = "German";
        else if (language == 3)
            dir = "Italian";
        else if (language == 1)
            dir = "French";
        else if (language == 4)
            dir = "Spanish";
        else if (language == 5)
            dir = "British";
        else
            dir = "English";
        sprintf(path, "%s%s%s\\%s.loc", RootDirectory, LocalizationContentDirectory, dir, m_name);
    }

    FILE_HANDLE_STRUCT *f;
    if (!(f = FOPEN(path, "rb"))) {
        PSXprintf("*** ERROR *** - Could not open '%s' localization file.", name);
        return 0;
    }
    FSEEK(f, 0x1A, 0);
    int ok = FREAD(&m_numStrings, 2, f);
    if (m_numStrings <= 0 || !ok) {
        FCLOSE(f);
#line 61 "Localization.cpp"
        ASSERTF(0, ("Error reading locfile (%s)\n", path));
        return 0;
    }
    FSEEK(f, 8, 0);
    if (!FREAD(&offset, 4, f)) {
        FCLOSE(f);
#line 71 "Localization.cpp"
        ASSERTF(0, ("Error reading locfile (%s)\n", path));
        return 0;
    }
    int entrySize = sizeof(GameTextString);
    int size = m_numStrings * entrySize;  // a variable: retail multiplies (mult), no shift
    GameTextString *table = (GameTextString *)malloc(size);
    FSEEK(f, offset, 0);
    if (!FREAD(table, size, f)) {
        free(table);
        FCLOSE(f);
#line 84 "Localization.cpp"
        ASSERTF(0, ("Error reading locfile (%s)\n", path));
        return 0;
    }
    if (m_strings)
        delete[] m_strings;
    m_strings = new LocalizedString[m_numStrings];
    for (int i = 0; i < m_numStrings; i++) {
        m_strings[i].m_maxLength = table[i].m_length;
#line 97 "Localization.cpp"
        ASSERT(m_strings[i].m_maxLength <= 480, "MAX_LOCALIZED_TEXT_LENGTH defined too small!");
    }
    free(table);
    FCLOSE(f);
    return 1;
}


// Loads the language file: a block of NUL-separated strings, one per ID.
int GameText::LoadGameText(Language language)
{
    char name[32];

    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    FILE_HANDLE_STRUCT *f;
    if (!(f = OpenLanguageFile(language, "rb"))) {
        PSXprintf("*** ERROR *** - Could not open %d language file.", language);
        return 0;
    }
    FSEEK(f, 0, 2);
    long size = FTELL(f);
    if (size <= 0) {
        FCLOSE(f);
        return 0;
    }
    if (m_data)
        free(m_data);
    m_data = (char *)malloc(size);
    m_data[size - 1] = 0;
    FSEEK(f, 0, 0);
    FREAD(m_data, size, f);
    FCLOSE(f);

    char *p = m_data;
    int i = 0;
    while (i < m_numStrings) {
        if (!(p < m_data + size)) {
            PSXprintf("*** LOCALIZATION ERROR? *** - More IDs than strings in language file!");
            break;
        }
        m_strings[i++].m_string = p;
        p = FindChar(p, 0) + 1;
    }

    const char *s;
    if (language == 2)
        s = "GERMAN";
    else if (language == 3)
        s = "ITALIAN";
    else if (language == 1)
        s = "FRENCH";
    else if (language == 4)
        s = "SPANISH";
    else if (language == 5)
        s = "BRITISH";
    else
        s = "ENGLISH";
    strcpy(name, s);
    PSXprintf("\n>>> GAME TEXT LOADED...");
    PSXprintf("\n>>> %d strings found in", m_numStrings);
    PSXprintf("\n>>> '%s' text : %s language.\n", m_name, name);
    return m_numStrings;
}


int GameText::GetMaxStringLength(int i)
{
    return m_strings[i].m_maxLength;
}

FILE_HANDLE_STRUCT *GameText::OpenLanguageFile(Language language, char *mode)
{
    char path[0x110];

    switch (language) {
    case 2:
        sprintf(path, "%s%sGerman\\L_%s_GER.bin", RootDirectory, LocalizationContentDirectory, m_name);
        break;
    case 3:
        sprintf(path, "%s%sItalian\\L_%s_ITA.bin", RootDirectory, LocalizationContentDirectory, m_name);
        break;
    case 1:
        sprintf(path, "%s%sFrench\\L_%s_FRE.bin", RootDirectory, LocalizationContentDirectory, m_name);
        break;
    case 4:
        sprintf(path, "%s%sSpanish\\L_%s_SPA.bin", RootDirectory, LocalizationContentDirectory, m_name);
        break;
    case 5:
        sprintf(path, "%s%sBritish\\L_%s_BRI.bin", RootDirectory, LocalizationContentDirectory, m_name);
        break;
    default:
        sprintf(path, "%s%sEnglish\\L_%s_ENG.bin", RootDirectory, LocalizationContentDirectory, m_name);
        break;
    }
    return FOPEN(path, mode);
}
