// GameText: one localization table (a .loc index and a language's .bin
// strings). No vtable. `new GameText` allocates 0x24 bytes (JetskiGame), so
// the name buffer that Create copies into is 0x12 bytes, not the 0x100 some
// copies declared. Data from GameText's own methods (src/GameText/) and the
// inline GetString the other files declared (Localization.h in retail).
#ifndef GAMETEXT_GAMETEXT_H
#define GAMETEXT_GAMETEXT_H

// GetString asserts. The files that use it define ASSERTF either way (do/while
// in soultree_assert.h or a block in soultree_assert_block.h); both match.
#ifndef ASSERTF
#include "soultree_assert_block.h"
#endif

struct FILE_HANDLE_STRUCT;
enum Language {};

#include "LocalizedString/LocalizedString.h"

class GameText {
public:
    GameText();
    ~GameText();
    int Create(char *name, Language language);
    int LoadGameText(Language language);
    FILE_HANDLE_STRUCT *OpenLanguageFile(Language language, char *mode);
    int GetMaxStringLength(int id);  // int: callers use $v0 as is

    char *GetString(int id)
    {
#line 55 "Localization.h"
        ASSERTF(id >= 0 && id < m_numStrings, ("*** Localization ERROR! ***  ID #%d is out of range...", id));
        return m_strings[id].m_string;
    }

    char *m_data;                   // +0x00, the language's strings
    LocalizedString *m_strings;     // +0x04
    char pad08[0x10 - 0x08];
    short m_numStrings;             // +0x10
    char m_name[0x12];              // +0x12
};

#endif
