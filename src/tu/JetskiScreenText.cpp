// JetskiScreenText.cpp  @ 0x003CBB50 (text 0x3CBB50-0x3CC110)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 2 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiScreenText.cpp as one translation unit: a TU without a
// static initializer inside BoatProcedural.cpp's tu_map row. Its compiler
// labels restart (@648.. for SetText's strings, right after BoatProcedural's
// RTTI records) and it was built with -sdatathreshold 2 (SetText's 2-byte "@"
// template is a $gp object), so it is not BoatProcedural.cpp.



#include "soultree_assert_block.h"
#include "JetskiScreenText/JetskiScreenText.h"
#include "ScreenTextManager/ScreenTextManager.h"
#include "ScreenText/ScreenText.h"
#include "GameText/GameText.h"
#include "JetskiGame/JetskiGame.h"

// The old SetText.cpp had inline_depth(5) (a TU-wide setting).
#pragma inline_depth(5)

extern "C" void free(void *p);
extern "C" void *realloc(void *p, unsigned int n);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strtok(char *s, const char *delim);

extern JetskiGame *LpGame;

// ---- src/JetskiScreenTextParams/ctor.cpp

JetskiScreenTextParams::JetskiScreenTextParams()
{
    m_justification = ScreenTextManager::LeftJustification;
    m_color = -1;
    m_x = 0;
    m_y = 0;
    m_lineSpacing = 0;
    m_scale = 1.0f;
    m_font = 0;
}


// ---- src/JetskiScreenText/ctor.cpp

JetskiScreenText::JetskiScreenText()
{
    m_texts = 0;
    m_x20 = 0;
    m_numTexts = 0;
}


// ---- src/JetskiScreenText/dtor.cpp

JetskiScreenText::~JetskiScreenText()
{
    if (m_texts) {
        for (int i = 0; i < m_numTexts; i++)
            delete m_texts[i];
        free(m_texts);
        m_texts = 0;
    }
}


// ---- src/JetskiScreenText/SetText.cpp
// Split the localized string into lines, one ScreenText each.
// Built with -sdatathreshold 2: delim's "@" template is a 2-byte $gp object.
void JetskiScreenText::SetText(int id, int ui)
{
    int i;
    char *token;
    char buffer[0x1EC];
    int line;
    JetskiScreenText *self;     // `this` through a named local: retail's $s order
    self = this;
    if (self->m_texts) {
        for (i = 0; i < self->m_numTexts; i++)
            delete self->m_texts[i];
        free(self->m_texts);
        self->m_texts = 0;
    }
    if (ui)
        strcpy(buffer, LpGame->m_uiText->GetString(id));
    else
        strcpy(buffer, LpGame->m_text->GetString(id));
    char delim[] = "@";             // the game text's line separator (retail's 2-byte template)
    self->m_texts = 0;
    self->m_numTexts = 0;
    line = -1;
    token = strtok(buffer, delim);
    while (token) {
        line++;
        self->m_numTexts++;
        self->m_texts = (ScreenText **)realloc(self->m_texts, self->m_numTexts * sizeof(ScreenText *));
        self->m_texts[line] = new ScreenText(0x28, 0);
        self->m_texts[line]->Create(strlen(token), token);
        self->m_texts[line]->SetScale(self->m_params.m_scale);
        {
            // Through locals, pointer first: retail's setter temporaries.
            ScreenText *text = self->m_texts[line];
            int justification = self->m_params.m_justification;
            text->SetJustification(justification);
        }
        self->m_texts[line]->SetColor(self->m_params.m_color);
        self->m_texts[line]->SetPosition(self->m_params.m_x, self->m_params.m_y + self->m_params.m_lineSpacing * line);
        self->m_texts[line]->SetFont(self->m_params.m_font);
        token = strtok(0, delim);
    }
}


// ---- src/JetskiScreenText/AddText.cpp

void JetskiScreenText::AddText(ScreenTextManager *manager)
{
    if (manager && m_texts) {
        for (int i = 0; i < m_numTexts; i++) {
            if (m_texts[i]) {
                manager->PositionText(m_texts[i]);
                manager->AddText(m_texts[i], 0);
            }
        }
    }
}


