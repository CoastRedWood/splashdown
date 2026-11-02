// PSXMemoryCard: the PS2 memory card (JetskiGameOptions::m_memoryCard).
// An empty MemoryCard base, then the active port, the save's title, content
// directory and icon.sys; 0x460 bytes. Names come from the class's own
// methods.
#ifndef PSXMEMORYCARD_PSXMEMORYCARD_H
#define PSXMEMORYCARD_PSXMEMORYCARD_H

// Sony libmc's icon.sys, 0x3C4 bytes.
typedef int iconIVECTOR[4];
typedef float iconFVECTOR[4];

struct sceMcIconSys {
    unsigned char Head[4];          // +0x000
    unsigned short Reserv1;         // +0x004
    unsigned short OffsLF;          // +0x006
    unsigned int Reserv2;           // +0x008
    unsigned int TransRate;         // +0x00C
    iconIVECTOR BgColor[4];         // +0x010
    iconFVECTOR LightDir[3];        // +0x050
    iconFVECTOR LightColor[3];      // +0x080
    iconFVECTOR Ambient;            // +0x0B0
    unsigned char TitleName[68];    // +0x0C0
    unsigned char FnameView[64];    // +0x104
    unsigned char FnameCopy[64];    // +0x144
    unsigned char FnameDel[64];     // +0x184
    unsigned char Reserve3[512];    // +0x1C4
};

class MemoryCard {
public:
    MemoryCard();
    ~MemoryCard();
};

class PSXMemoryCard : public MemoryCard {
public:
    PSXMemoryCard();
    ~PSXMemoryCard();
    int DetectCard(int port, int *freeClusters);
    int SetActivePort(int port);
    int FormatActive();
    int SetContentDirectory(char *dir);
    int SetGameTitle(char *title, int titleBreak);
    int SetIconNames(int count, char *normal, char *copying, char *deleting);
    int GetContentDir();
    int CreateContentDir();
    int MUCreateFile(char *name, void *data, int size);
    int MUReadFile(char *name, void *data, int size);
    int MUDeleteFile(char *name);
    int AllGood();

    int m_port;                     // +0x000, 99 = none selected
    char m_title[0x40];             // +0x004
    // Offset of the title's second line: SetGameTitle stores a word,
    // CreateContentDir reads it back as a short (lh).
    int m_titleBreak;               // +0x044
    char m_contentDir[0x20];        // +0x048
    sceMcIconSys m_iconSys;         // +0x068
    int m_cardChecked;              // +0x42C
    char m_iconNormal[14];          // +0x430
    char m_iconCopy[14];            // +0x43E
    char m_iconDelete[16];          // +0x44C
    int m_iconCount;                // +0x45C
};

#endif
