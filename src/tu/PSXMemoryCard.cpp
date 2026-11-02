// PSXMemoryCard.cpp  @ 0x002FCF90 (text 0x2FCF90-0x2FDDE0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's PSXMemoryCard.cpp as one translation unit: PSXMemoryCard's methods
// (libmc), a TU without an initializer in ParticleEmitter.cpp's tu_map row,
// between SearchNode.cpp and SearchPlacement.cpp. The C-mode
// SetContentDirectory and SetGameTitle are members again.



#include "PSXMemoryCard/PSXMemoryCard.h"
#include "DataAccess/DataAccess.h"

extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);
extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
extern "C" int sceMcGetInfo(int port, int slot, int *type, int *free, int *format);
extern "C" int sceMcSync(int mode, int *cmd, int *result);
extern "C" int sceMcFormat(int port, int slot);
extern "C" int sceMcChdir(int port, int slot, const char *newDir, char *curDir);
extern "C" int sceMcMkdir(int port, int slot, const char *name);
extern "C" int sceMcOpen(int port, int slot, char *name, int mode);   // non-const: gives retail's argument load order
extern "C" int sceMcWrite(int fd, const void *buf, int size);
extern "C" int sceMcRead(int fd, void *buf, int size);
extern "C" int sceMcClose(int fd);
extern "C" int sceMcDelete(int port, int slot, const char *name);
void PSXprintf(const char *fmt, ...);

extern DataAccess *LpDataAccess;
// ---- src/PSXMemoryCard/ctor.cpp


PSXMemoryCard::PSXMemoryCard()
{
    m_port = 99;                // no port selected
    m_cardChecked = 0;
    strcpy(m_iconNormal, "");
    strcpy(m_iconCopy, "");
    strcpy(m_iconDelete, "");
    strcpy(m_title, "");
    m_iconCount = 0;
}


// ---- src/PSXMemoryCard/dtor.cpp


PSXMemoryCard::~PSXMemoryCard()
{
}


// ---- src/PSXMemoryCard/DetectCard.cpp


// Returns 1 for a usable formatted card, 2 if the card was swapped since the
// last check, -1/-2 for an unformatted card, 0 for no (PS2) card.
int PSXMemoryCard::DetectCard(int port, int *freeClusters)
{
    int type, free, format, result;
    sceMcGetInfo(port, 0, &type, &free, &format);
    sceMcSync(0, 0, &result);
    switch (type) {
    case 1:
        PSXprintf("15 Block Memory Card\n");
        return 0;
    case 2:
        PSXprintf("8000 Block Memory Card\n");
        if (!format) {
            if (result < 0) {
                PSXprintf("New UnFormatted Card Detected in Port %d\n", port);
                return -2;
            }
            PSXprintf("UnFormatted Card Detected in Port %d\n", port);
            return -1;
        }
        if (freeClusters)
            *freeClusters = free;
        if (result < 0 && m_cardChecked) {
            PSXprintf("Memory Card Has Been Switched since last check\n");
            return 2;
        }
        if (result == 0 || !m_cardChecked) {
            PSXprintf("Same or New Memory Card Has Been Detected\n");
            m_cardChecked = 1;
            return 1;
        }
        PSXprintf("\nPort %d Card Detected:\n", port);
        if (format)
            PSXprintf("Formatted\n");
        else
            PSXprintf("%d Clusters Free\n", free);
        return 1;
    case 3:
        PSXprintf("15 Block Pocket Station\n");
        return 0;
    default:
        PSXprintf("No Card Detected in Port %d\n", port);
        return 0;
    }
}


// ---- src/PSXMemoryCard/SetActivePort.cpp


int PSXMemoryCard::SetActivePort(int port)
{
    if (DetectCard(port, 0)) {
        PSXprintf("Attempting to Set Active MemoryCard to Port %d......Successful!!\n", port);
        m_port = port;
    } else {
        PSXprintf("Attempting to Set Active MemoryCard to Port %d......Failed!!\n", port);
        return 0;
    }
    return 1;
}


// ---- src/PSXMemoryCard/FormatActive.cpp


int PSXMemoryCard::FormatActive()
{
    if (AllGood()) {
        int error = sceMcFormat(m_port, 0);
        int result;
        sceMcSync(0, 0, &result);
        if (error == 0 && result == 0) {
            PSXprintf("Attempting to Format Port %d......Successful!!\n", m_port);
            return 1;
        }
        PSXprintf("Attempting to Format Port %d......Failed!!\n", m_port);
        return 0;
    }
    return 0;
}


// ---- src/PSXMemoryCard/SetContentDirectory.cpp

int PSXMemoryCard::SetContentDirectory(char *dir)
{
    if (*dir) {
        strcpy(m_contentDir, dir);
        return 1;
    }
    return 0;
}

// ---- src/PSXMemoryCard/SetGameTitle.cpp

int PSXMemoryCard::SetGameTitle(char *title, int titleBreak)
{
    if (*title != 0) {
        strcpy(m_title, title);
        m_titleBreak = titleBreak;
        return 1;
    }
    return 0;
}

// ---- src/PSXMemoryCard/SetIconNames.cpp


int PSXMemoryCard::SetIconNames(int count, char *normal, char *copying, char *deleting)
{
    if (*normal && *copying && *deleting && count > 0) {
        strcpy(m_iconNormal, normal);
        strcpy(m_iconCopy, copying);
        strcpy(m_iconDelete, deleting);
        m_iconCount = count;
        return 1;
    }
    return 0;
}


// Retail creates GetContentDir's three listing messages (@438-@440) before
// its "*" template and "/" (@474, @490): a function printing them came here
// that nothing called, which retail's linker stripped. link_c leaves its
// section out.
static void StrippedListing(int port, int result, char *name)
{
    if (result >= 0) {
        PSXprintf("Attempting to Read From Port %d......Successful!!  %d Entries Read!!\n", port, result);
        PSXprintf("%d: '%s'\n", result, name);
    } else {
        PSXprintf("Attempting to Read From Port %d......Failed!!\n", port);
    }
}


// ---- src/PSXMemoryCard/GetContentDir.cpp
// GetContentDir__13PSXMemoryCardFv  @ 0x002FD4A0


struct sceMcTblGetDir {
    char pad0[0x20];
    char EntryName[0x20];       // +0x20
};

// The table's real pointer type matters: with `void *` mwcc loads the table
// address before the 99.
extern "C" int sceMcGetDir(int port, int slot, const char *name, unsigned int mode, int maxEnt, sceMcTblGetDir *table);


// Retail's frame keeps 0x30 bytes between the table and path. A separate
// unused local is dropped, so the table sits in a struct with trailing space.
struct McDirListing {
    sceMcTblGetDir table[99];
    char pad[0x30];
};

// Lists the content directory on the active card.
int PSXMemoryCard::GetContentDir()
{
    McDirListing dir;
    char path[32];
    int result;

    if (AllGood()) {
        strcpy(path, "/");
        strcat(path, m_contentDir);
        sceMcChdir(m_port, 0, path, 0);
        sceMcSync(0, 0, &result);
        if (result < -9)
            return -1;
        if (result == 0) {
            char pattern[31] = "*";
            sceMcGetDir(m_port, 0, pattern, 0, 99, dir.table);
            sceMcSync(0, 0, &result);
            if (result < -9)
                return -1;
            if (result >= 0) {
                PSXprintf("Attempting to Read From Port %d......Successful!!  %d Entries Read!!\n", m_port, result);
                for (int i = 0; i < result; i++)
                    PSXprintf("%d: '%s'\n", i, dir.table[i].EntryName);
                return 1;
            }
            PSXprintf("Attempting to Read From Port %d......Failed!!\n", m_port);
            return 1;
        }
        PSXprintf("Chdir......Failed!!\n");
    }
    return 0;
}


// ---- src/PSXMemoryCard/CreateContentDir.cpp


// libmc's icon.sys layout.
// Creates the save directory with its icon.sys and copies the icon files
// into it.
int PSXMemoryCard::CreateContentDir()
{
    if (AllGood()) {
        sceMcMkdir(m_port, 0, m_contentDir);
        int result;
        sceMcSync(0, 0, &result);
        if (result == 0) {
            iconIVECTOR bgcolor[4] = {
                { 32, 49, 72, 0 },
                { 109, 68, 15, 0 },
                { 127, 106, 0, 0 },
                { 0, 0, 0, 0 },
            };
            iconFVECTOR lightdir[3] = {
                { 0.5f, 0.5f, 0.5f, 0.0f },
                { 0.0f, -0.4f, -0.1f, 0.0f },
                { -0.5f, -0.5f, 0.5f, 0.0f },
            };
            iconFVECTOR lightcol[3] = {
                { 0.48f, 0.48f, 0.03f, 0.0f },
                { 0.5f, 0.33f, 0.2f, 0.0f },
                { 0.14f, 0.14f, 0.38f, 0.0f },
            };
            iconFVECTOR ambient = { 0.5f, 0.5f, 0.5f, 0.0f };
            if (m_iconNormal[0] == 0 || !m_iconNormal[0]) {     // (sic) the same test twice
                PSXprintf("Icon Names have not been assigned!!\n");
                return 0;
            }
            char *iconFiles[3] = { m_iconNormal, m_iconCopy, m_iconDelete };
            GetContentDir();
            memset(&m_iconSys, 0, sizeof(m_iconSys));
            strcpy((char *)m_iconSys.Head, "PS2D");
            if (!m_title[0]) {
                PSXprintf("Game Title has not been assigned!!\n");
                return 0;
            }
            strcpy((char *)m_iconSys.TitleName, m_title);
            m_iconSys.OffsLF = m_titleBreak;
            m_iconSys.TransRate = 0x60;
            memcpy(m_iconSys.BgColor, bgcolor, sizeof(bgcolor));
            memcpy(m_iconSys.LightDir, lightdir, sizeof(lightdir));
            memcpy(m_iconSys.LightColor, lightcol, sizeof(lightcol));
            memcpy(m_iconSys.Ambient, ambient, sizeof(ambient));
            strcpy((char *)m_iconSys.FnameView, iconFiles[0]);
            strcpy((char *)m_iconSys.FnameCopy, iconFiles[1]);
            strcpy((char *)m_iconSys.FnameDel, iconFiles[2]);
            MUCreateFile("icon.sys", &m_iconSys, sizeof(m_iconSys));
            for (int i = 0; i < m_iconCount; i++) {
                long size;
                char *data;
                long file;
                int fd;
                file = LpDataAccess->FOpen(iconFiles[i], "r");
                if (file == -1) {
                    PSXprintf(" can't open icon file \n");
                    return 0;
                }
                LpDataAccess->FSeek(file, 0, 2);
                size = LpDataAccess->FTell(file);
                data = new char[size];
                LpDataAccess->FSeek(file, 0, 0);
                LpDataAccess->FRead(file, data, size, 1);
                LpDataAccess->FClose(file);
                int ret;
                sceMcOpen(m_port, 0, iconFiles[i], 0x203);
                sceMcSync(0, 0, &ret);
                fd = ret;
                if (fd < 0) {
                    PSXprintf(" Open Failed: %d \n", fd);
                    return 0;
                }
                sceMcWrite(fd, data, size);
                sceMcSync(0, 0, &ret);
                if (ret < 0) {
                    PSXprintf(" Write Failed: %d \n", ret);
                    break;
                }
                if (data)
                    delete[] data;
                sceMcClose(fd);
                sceMcSync(0, 0, &ret);
                if (ret < 0)
                    break;
            }
            PSXprintf("End Populate\n");
        }
    }
    return 1;
}


// ---- src/PSXMemoryCard/MUCreateFile.cpp


int PSXMemoryCard::MUCreateFile(char *name, void *data, int size)
{
    int fd;
    int result;
    if (AllGood()) {
        int status = sceMcOpen(m_port, 0, name, 0x202);
        if (status) {
            PSXprintf("McOpen Process Failed!! result: %d\n", status);
            return 0;
        }
        sceMcSync(0, 0, &fd);
        if (fd >= 0) {
            PSXprintf("File Open Succesful!!\n");
            PSXprintf("Attempting to write '%s',size:%d\n", name, size);
            sceMcWrite(fd, data, size);
            sceMcSync(0, 0, &result);
            if (result > 0)
                PSXprintf("File Write Successful!! Bytes Written: %d\n", result);
            else
                PSXprintf("File Write Failed!! Return Code %d\n", result);
            sceMcClose(fd);
            sceMcSync(0, 0, &result);
            if (result) {
                // Retail prints the (zero) open status here, not the close result.
                PSXprintf("McClose Process Failed!! result: %d\n", status);
                return 0;
            }
            return 1;
        }
        PSXprintf("File Open Failed!! Return Code %d\n", fd);
    }
    return 0;
}


// ---- src/PSXMemoryCard/MUReadFile.cpp


int PSXMemoryCard::MUReadFile(char *name, void *data, int size)
{
    int fd;
    int result;
    if (AllGood()) {
        int status = sceMcOpen(m_port, 0, name, 1);
        if (status) {
            PSXprintf("McOpen Process Failed!! result: %d\n", status);
            return 0;
        }
        sceMcSync(0, 0, &fd);
        if (fd >= 0) {
            PSXprintf("File Open Succesful!!\n");
            PSXprintf("Attempting to read %d bytes\n", size);
            sceMcRead(fd, data, size);
            sceMcSync(0, 0, &result);
            if (result > 0) {
                PSXprintf("File '%s' Read Successful!! Bytes Read: %d\n", name, result);
            } else {
                // Retail returns without closing the file.
                PSXprintf("File '%s' Read Failed!! Return Code %d\n", name, result);
                return 0;
            }
            sceMcClose(fd);
            sceMcSync(0, 0, &result);
            if (result) {
                // Retail prints the (zero) open status here, not the close result.
                PSXprintf("McClose Process Failed!! result: %d\n", status);
                return 0;
            }
            return 1;
        }
        PSXprintf("File '%s' Open Failed!! Return Code %d\n", name, fd);
    }
    return 0;
}


// ---- src/PSXMemoryCard/AllGood.cpp

int PSXMemoryCard::AllGood()
{
    if (m_port == 99)
        return 0;
    return DetectCard(m_port, 0) != 0;
}


// ---- src/PSXMemoryCard/MUDeleteFile.cpp


int PSXMemoryCard::MUDeleteFile(char *name)
{
    if (AllGood()) {
        sceMcDelete(m_port, 0, name);
        int result;
        sceMcSync(0, 0, &result);
        if (result == 0) {
        PSXprintf("File '%s' Deleted!! \n", name);
            return 1;
        }
    }
    return 0;
}

