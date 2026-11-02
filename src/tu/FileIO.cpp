// FileIO.cpp  @ 0x00353FD0 (text 0x353FD0-0x3551C0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// The disc file functions (FILE_HANDLE_STRUCT, FOPEN .. SPLIT_PATH) as one
// translation unit without a static initializer, in
// JetskiPracticeCompetition.cpp's tu_map row. Its ASSERTs name the file; its
// .rodata restarts at @467 after the JetskiPracticeCompetition RTTI, its .data
// is FirstSectorInBuffer and LastSectorInBuffer, and its .bss (SectorBuffer,
// the profiler statics) follows JetskiPracticeCompetition.cpp's axis vectors.
// The disc map (mapFile) is the data-only TU's.



#include "FILE_HANDLE_STRUCT/FILE_HANDLE_STRUCT.h"
#include "soultree_assert_block.h"

extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" int printf(const char *fmt, ...);
extern "C" int strcasecmp(const char *a, const char *b);
extern "C" unsigned int strlen(const char *s);
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);

struct sceCdRMode {
    unsigned char trycount;
    unsigned char spindlctrl;
    unsigned char datapattern;
    unsigned char pad;
};

extern "C" int sceCdDiskReady(int mode);
extern "C" int sceCdRead(unsigned int lsn, unsigned int sectors, void *buf, sceCdRMode *mode);
extern "C" int sceCdSync(int mode);
extern "C" int sceCdGetError(void);
extern "C" unsigned int sceCdGetReadPos(void);
extern "C" void sceCdBreak(void);

int FEOF(FILE_HANDLE_STRUCT *f);
int FREAD(void *buffer, unsigned int size, unsigned int count, FILE_HANDLE_STRUCT *f);

#define SECTOR_SIZE 2048
#define BUFFER_SECTORS 0x80

unsigned int FirstSectorInBuffer = 0xFFFFFFFF;
unsigned int LastSectorInBuffer = 0xFFFFFFFF;
char SectorBuffer[BUFFER_SECTORS * SECTOR_SIZE];

FILE_HANDLE_STRUCT::FILE_HANDLE_STRUCT()
{
    position = 0;
    size = 0;
}

FILE_HANDLE_STRUCT::~FILE_HANDLE_STRUCT()
{
}

// One entry of the disc's file map.
struct MapFileEntry {
    char *name;              // +0x00
    int unk04;
    unsigned int lsn;        // +0x08
    int unk0C[3];
    unsigned int size;       // +0x18
    int unk1C;
    unsigned int length;     // +0x20
    int unk24;
};

extern MapFileEntry mapFile[];

FILE_HANDLE_STRUCT *FOPEN(char *name, const char *mode)
{
    sceCdlFILE file;
    char path[256];
    int i;
    int found;
    MapFileEntry *entry;

    sprintf(path, "\\%s;1", name);
    entry = mapFile;
    found = 0;
    i = 0;
    while (!found && i < 0x551) {
        if (strcasecmp(entry->name, path) == 0) {
            file.lsn = entry->lsn;
            file.size = entry->size;
            int j = strlen(path) - 1;
            while (path[j] != '\\')
                j--;
            for (int k = 0; k < 16; k++) {
                if (path[j + k + 1] != ';') {
                    file.name[k] = path[j + k + 1];
                } else {
                    file.name[k] = 0;
                    k = 16;
                }
            }
            found = 1;
        } else {
            entry++;
            i++;
        }
    }
    FILE_HANDLE_STRUCT *f = new FILE_HANDLE_STRUCT;
    if (found) {
        f->file = file;
        f->position = 0;
        f->size = mapFile[i].length;
        return f;
    }
    delete f;
    return 0;
}

// Reads whole sectors covering the request into a stack buffer, retrying
// until the drive reports no error, then copies out the requested bytes.
int FREAD(void *buffer, unsigned int size, unsigned int count, FILE_HANDLE_STRUCT *f)
{
    static int fileReadMtr = -1;    // unused profiler ID
    char readBuffer[0x40000];
    sceCdRMode mode;
    int lsn;
    int sectors;
    int result;
    int done;
    int error;
    int bytes;

    if (FEOF(f))
        return 0;
    lsn = f->file.lsn + (int)f->position / SECTOR_SIZE;
    sectors = (int)(size * count + SECTOR_SIZE - 1) / SECTOR_SIZE;
    sectors++;
    mode.trycount = 0xFF;
    mode.spindlctrl = 1;
    mode.datapattern = 0;
    done = 0;
    while (!done) {
        static int event = -1;      // unused profiler ID
        sceCdDiskReady(0);
        result = sceCdRead(lsn, sectors, readBuffer, &mode);
        sceCdSync(0);
        error = sceCdGetError();
        switch (error) {
        case -1:
#line 231 "FileIO.cpp"
            ASSERT(0, "SCECdErFAIL - CdGetError() failed!\n");
            break;
        case 0:
            done = 1;
            break;
        case 0x32:
#line 237 "FileIO.cpp"
            ASSERT(0, "SCECdErEOM - Outermost track reached!\n");
            break;
        case 0x31:
            printf("SCECdErTRMOPN - CD tray open - please close!\n");
            break;
        case 0x30:
            printf("SCECdErREAD - Trying again...\n");
            break;
        case 0x14:
#line 246 "FileIO.cpp"
            ASSERT(0, "SCECdErCUD - Not appropriate for disc in drive!\n");
            break;
        case 0x13:
            printf("SCECdErNORDY - CD Drive not ready...\n");
            break;
        case 0x12:
#line 253 "FileIO.cpp"
            ASSERT(0, "SCECdErNODISC - No disc in drive!\n");
            break;
        case 1:
            printf("SCECdErABRT - Aborted, trying again...\n");
            break;
        default:
#line 259 "FileIO.cpp"
            ASSERTF(0, ("%d\n", error));
            break;
        }
    }
    bytes = size * count;
    if (f->position + bytes >= f->size)
        bytes -= f->position + bytes - f->size;
    memcpy(buffer, readBuffer + (int)f->position % SECTOR_SIZE, bytes);
    if (!result) {
        printf("sceCdRead error\n");
        while (1)
            ;
    }
    f->position += bytes;
    return bytes / size;
}


// Buffered read: serves the request from the sector cache when it can,
// otherwise reads ahead into the cache (up to 127 sectors at the start of a
// file) and copies out of it.
int FREAD(void *buffer, unsigned int size, FILE_HANDLE_STRUCT *f)
{
    static int fileReadMtr2 = -1;   // unused profiler ID
    sceCdRMode mode;
    unsigned int lsn;
    int offset;
    int done;
    int error;
    int count;
    int result;
    int sectors;
    int skipped;
    int total;
    int bytes;
    int fileSectors;

    if (FEOF(f))
        return 0;
    lsn = f->file.lsn + (int)f->position / SECTOR_SIZE;
    sectors = (int)(size + SECTOR_SIZE - 1) / SECTOR_SIZE;
    offset = 0;
    skipped = 0;
    if (lsn >= FirstSectorInBuffer && lsn + sectors <= LastSectorInBuffer) {
        memcpy(buffer, &SectorBuffer[(int)f->position % SECTOR_SIZE + (lsn - FirstSectorInBuffer) * SECTOR_SIZE], size);
        f->position += size;
        return size;
    }
    if (lsn >= FirstSectorInBuffer && lsn <= LastSectorInBuffer) {
        skipped = LastSectorInBuffer + 1 - lsn;
        offset = LastSectorInBuffer + 1 - FirstSectorInBuffer;
        lsn = LastSectorInBuffer + 1;
        sectors -= skipped;
    } else if (f->position == 0) {
        fileSectors = (f->size + SECTOR_SIZE - 1) / SECTOR_SIZE;
        if (fileSectors >= 0x7F)
            sectors = 0x7F;
    }
    mode.trycount = 0xFF;
    sectors++;
    total = 0;
    mode.spindlctrl = 1;
    mode.datapattern = 0;
    do {
        count = BUFFER_SECTORS - offset;
        done = 0;
        count = (sectors < count) ? sectors : count;
        sectors -= count;
        if (count == 0) {
            result = 1;
        } else {
            while (!done) {
                static int event = -1;  // unused profiler ID
                sceCdDiskReady(0);
                result = sceCdRead(lsn, count, SectorBuffer + offset * SECTOR_SIZE, &mode);
                sceCdSync(0);
                FirstSectorInBuffer = lsn - offset;
                LastSectorInBuffer = lsn + count - 1;
                error = sceCdGetError();
                switch (error) {
                case -1:
#line 396 "FileIO.cpp"
                    ASSERT(0, "SCECdErFAIL - CdGetError() failed!\n");
                    break;
                case 0:
                    done = 1;
                    break;
                case 0x32:
#line 402 "FileIO.cpp"
                    ASSERT(0, "SCECdErEOM - Outermost track reached!\n");
                    break;
                case 0x31:
                    printf("SCECdErTRMOPN - CD tray open - please close!\n");
                    break;
                case 0x30:
                    printf("SCECdErREAD - Trying again...\n");
                    break;
                case 0x14:
#line 411 "FileIO.cpp"
                    ASSERT(0, "SCECdErCUD - Not appropriate for disc in drive!\n");
                    break;
                case 0x13:
                    printf("SCECdErNORDY - CD Drive not ready...\n");
                    break;
                case 0x12:
#line 417 "FileIO.cpp"
                    ASSERT(0, "SCECdErNODISC - No disc in drive!\n");
                    break;
                case 1:
                    printf("SCECdErABRT - Aborted, trying again...\n");
                    break;
                default:
#line 423 "FileIO.cpp"
                    ASSERTF(0, ("%d\n", error));
                    break;
                }
            }
        }
        if (sectors != 0)
            bytes = (count + skipped) * SECTOR_SIZE - (int)f->position % SECTOR_SIZE;
        else
            bytes = size;
        size -= bytes;
        if (f->position + bytes >= f->size) {
            bytes -= f->position + bytes - f->size;
            size = 0;
        }
        memcpy(buffer, &SectorBuffer[(int)f->position % SECTOR_SIZE + (offset - skipped) * SECTOR_SIZE], bytes);
        buffer = (char *)buffer + bytes;
        offset = 0;
        skipped = 0;
        if (!result) {
            printf("sceCdRead error\n");
            while (1)
                ;
        }
        f->position += bytes;
        total += bytes;
        lsn = f->file.lsn + (int)f->position / SECTOR_SIZE;
    } while ((int)size > 0);
    return total;
}

void FREAD_ASYNC_ABORT(FILE_HANDLE_STRUCT *)
{
    if (sceCdSync(1)) {
        if (sceCdGetReadPos())
            sceCdBreak();
    }
}

int FREAD_ASYNC_STATUS(FILE_HANDLE_STRUCT *, unsigned int *bytesRead, int *error)
{
    *error = 0;
    int status = sceCdSync(1);
    *bytesRead = (int)sceCdGetReadPos() / 0x8000;
    return status;
}

int FWRITE(void *, unsigned int, unsigned int, FILE_HANDLE_STRUCT *)
{
    return 0;
}

int FSEEK(FILE_HANDLE_STRUCT *f, long offset, int whence)
{
    if (f == 0)
        return -1;
    if (whence == 2)
        f->position = f->size;
    else if (whence == 0)
        f->position = 0;
    if (offset)
        f->position += offset;
    return 0;
}

// lwu: long is 64-bit under mwccps2.
unsigned long FTELL(FILE_HANDLE_STRUCT *f)
{
    return f->position;
}

int FGETC(FILE_HANDLE_STRUCT *f)
{
    char c;

    if (f == 0)
        return -1;
    return (char)(FREAD(&c, 1, 1, f) ? c : 0);
}

int FEOF(FILE_HANDLE_STRUCT *f)
{
    if (f == 0)
        return 0;
    return f->position >= f->size;
}

int FFLUSH(FILE_HANDLE_STRUCT *)
{
    return 0;
}

int FCLOSE(FILE_HANDLE_STRUCT *f)
{
    if (f) {
        f->size = 0;
        f->position = 0;
        delete f;
    }
    return -1;
}

// Stubbed: output is discarded.
int FPRINTF(FILE_HANDLE_STRUCT *, const char *, ...)
{
    return 0;
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))

// _splitpath: drive ("C:"), directory (up to the last '/' or '\'), file
// name and extension (from the last '.'); each part is limited to 255
// characters and any part may be NULL.
void SPLIT_PATH(const char *path, char *drive, char *dir, char *fname, char *ext)
{
    const char *p;
    const char *dirEnd;
    const char *dot = 0;
    int len;

    if (strlen(path) && path[1] == ':') {
        if (drive) {
            strncpy(drive, path, 2);
            drive[2] = 0;
        }
        path += 2;
    } else if (drive) {
        *drive = 0;
    }
    dirEnd = 0;
    for (p = path; *p; p++) {
        char c = *p;
        if (c == '/' || c == '\\')
            dirEnd = p + 1;
        else if (c == '.')
            dot = p;
    }
    if (dirEnd) {
        if (dir) {
            len = MIN(dirEnd - path, 255);
            strncpy(dir, path, len);
            dir[len] = 0;
        }
        path = dirEnd;
    } else if (dir) {
        *dir = 0;
    }
    if (dot && dot >= path) {
        if (fname) {
            len = MIN(dot - path, 255);
            strncpy(fname, path, len);
            fname[len] = 0;
        }
        if (ext) {
            len = MIN(p - dot, 255);
            strncpy(ext, dot, len);
            ext[len] = 0;
        }
    } else {
        if (fname) {
            len = MIN(p - path, 255);
            strncpy(fname, path, len);
            fname[len] = 0;
        }
        if (ext)
            *ext = 0;
    }
}
