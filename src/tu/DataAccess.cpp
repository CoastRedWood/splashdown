// DataAccess.cpp  @ 0x002BC740 (text 0x2BC740-0x2C24C0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// DataAccess as one translation unit without a static initializer: the virtual
// file system over the game's data devices. Its .rodata runs from the
// "Critical failure in DataAccess::Initialize" string (labels from @498, a
// restart after TrackType.cpp's @1021) to the DataAccess/BaseObject RTTI. The
// file name is a guess.


#include "DataAccess/DataAccess.h"
#include "VirtualDataDevice/VirtualDataDevice.h"
#include "soultree_assert_block.h"
#include "VirtualFile/VirtualFile.h"

// Defined here for its retail .bss slot.
DataAccess *LpDataAccess;


// ---- src/DataAccess/ctor.cpp
void PSXprintf(const char *fmt, ...);


struct VirtualDataDevice;
struct VirtualFile;


DataAccess::DataAccess()
{
    m_saveTag = 0;
    m_nextDevice = -1;
    m_numDevices = 0;
    m_devices = 0;
    m_freeFile = -1;
    m_numVirtualFiles = 0;
    m_virtualFiles = 0;
    m_lastFile = -1;
    m_lastVirtualFile = -1;
    m_stow = &m_stowBuffer[0];
    m_stowEnd = &m_stowBuffer[3];
    m_x110 = -1;
    m_useFileSystem = 1;
    m_cacheSize = 0;
    m_cache = 0;
    m_cacheOwner = -1;
    m_cacheFirst = -1;
    m_cacheLast = -1;
    m_blockSizes = 0;
    for (int i = 0; i < 8; i++) {
        m_caches[i].m_data = 0;
        m_caches[i].m_file = -2;
        m_caches[i].m_x10 = -0x6000;
    }
    if (!Initialize(1, 10, 0x100000))
        PSXprintf("WARNING!! Critical failure in DataAccess::Initialize. Disk access will not be functional.\n");
}


// ---- src/DataAccess/dtor.cpp

DataAccess::~DataAccess()
{
    FreeSystemResources();
}


// ---- src/DataAccess/FreeSystemResources.cpp
extern "C" void free(void *);

struct FILE_HANDLE_STRUCT;
void FREAD_ASYNC_ABORT(FILE_HANDLE_STRUCT *f);



struct VirtualFile;


void DataAccess::FreeSystemResources()
{
    if (m_devices && m_virtualFiles)
        DropAllDevices(1);
    if (m_stowBuffer)               // always true; retail tests the array address
        ResetStow();
    if (m_devices) {
        for (int i = 0; i < m_numDevices; i++)
            FreeDevice(&m_devices[i]);
        free(m_devices);
        m_devices = 0;
    }
    if (m_virtualFiles) {
        for (int i = 0; i < m_numVirtualFiles; i++)
            FreeFile(i);
        free(m_virtualFiles);
        m_virtualFiles = 0;
    }
    // Retail reads the device list it has just freed (and nulled).
    if (m_cacheOwner != -1 && m_devices[m_cacheOwner].m_file)
        FREAD_ASYNC_ABORT(m_devices[m_cacheOwner].m_file);
    if (m_cache) {
        free(m_cache);
        m_cache = 0;
        m_cacheSize = 0;
    }
    if (m_blockSizes) {
        free(m_blockSizes);
        m_blockSizes = 0;
    }
    for (int i = 0; i < 8; i++) {
        if (m_caches[i].m_data) {
            free(m_caches[i].m_data);
            m_caches[i].m_data = 0;
        }
        m_caches[i].m_file = -2;
        m_caches[i].m_x10 = -0x6000;
    }
}


// ---- src/DataAccess/Initialize.cpp
extern "C" void *malloc(unsigned int size);
void *malloc_align(unsigned int align, unsigned int size);
void PSXprintf(const char *fmt, ...);

#define MAX(a, b) ((a) > (b) ? (a) : (b))

struct FILE_HANDLE_STRUCT;





int DataAccess::Initialize(int numDevices, int numFiles, unsigned int cacheSize)
{
    int ok = 1;
    int i;
    FreeSystemResources();
    m_numDevices = numDevices;
    m_devices = (VirtualDataDevice *)malloc(m_numDevices * sizeof(VirtualDataDevice));
    m_nextDevice = 0;
    m_x13C = 0;
    if (!m_devices) {
        m_numDevices = ok;
        m_devices = (VirtualDataDevice *)malloc(m_numDevices * sizeof(VirtualDataDevice));
    }
    if (!m_devices) {
        m_numDevices = 0;
        m_nextDevice = -1;
        ok = 0;
    } else {
        for (i = 0; i < m_numDevices; i++) {
            m_devices[i].m_flags = 1;
            m_devices[i].m_name = 0;
            m_devices[i].m_x08 = 0;
            m_devices[i].m_size = 0;
            m_devices[i].m_file = 0;
            m_devices[i].m_id = i;
            m_devices[i].m_dataOffset = 0;
            m_devices[i].m_tableSize = 0;
            m_devices[i].m_tableOffset = 0;
            m_devices[i].m_x24 = 0;
        }
    }
    m_numVirtualFiles = numFiles;
    m_virtualFiles = (VirtualFile *)malloc(m_numVirtualFiles * sizeof(VirtualFile));
    m_freeFile = 0;
    if (!m_virtualFiles) {
        m_numVirtualFiles = 1;
        m_virtualFiles = (VirtualFile *)malloc(m_numVirtualFiles * sizeof(VirtualFile));
    }
    if (!m_virtualFiles) {
        m_numVirtualFiles = 0;
        m_freeFile = -1;
        ok = 0;
    } else {
        for (i = 0; i < m_numVirtualFiles; i++) {
            m_virtualFiles[i].m_flags = 1;
            m_virtualFiles[i].m_name = 0;
            m_virtualFiles[i].m_device = 0;
            m_virtualFiles[i].m_offset = 0;
            m_virtualFiles[i].m_position = 0;
            m_virtualFiles[i].m_length = 0;
            m_virtualFiles[i].m_cache = 0;
            m_virtualFiles[i].m_data = 0;
        }
    }
    m_lastFile = -1;
    m_lastVirtualFile = -1;
    m_cacheSize = MAX(cacheSize, 0xC000);
    m_cache = (unsigned char *)malloc_align(0x40, m_cacheSize);
    if (!m_cache) {
        // Halve the request until it fits (down to one 0x6000 slot).
        do {
            m_cacheSize >>= 1;
            m_cache = (unsigned char *)malloc_align(0x40, m_cacheSize);
        } while (!m_cache && m_cacheSize > 0x6000);
        if (!m_cache) {
            m_cacheSize = 0;
            ok = 0;
        }
    }
    int failures;
    int freeCache = -1;
    failures = 0;
    for (i = 0; i < 8; i++) {
        m_caches[i].m_data = (unsigned char *)malloc_align(0x40, 0x6000);
        if (!m_caches[i].m_data) {
            m_caches[i].m_file = -2;
            failures++;
        } else {
            m_caches[i].m_file = -1;
            freeCache = i;
        }
        m_caches[i].m_x10 = -0x6000;
    }
    if (8 - failures < 2) {
        PSXprintf("Virtual file cache attempted %d slots, however %d allocations FAILED.\n", 8, failures);
        ok = 0;
    }
    m_x110 = freeCache;
    ResetStow();
    return ok;
}


// ---- src/DataAccess/LoadResourceFile.cpp

extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
extern "C" void *malloc(unsigned int size);
extern "C" void *realloc(void *p, unsigned int size);
extern "C" int strcasecmp(const char *a, const char *b);

struct FILE_HANDLE_STRUCT;
FILE_HANDLE_STRUCT *FOPEN(char *name, const char *mode);
int FCLOSE(FILE_HANDLE_STRUCT *file);
long FTELL(FILE_HANDLE_STRUCT *file);
int FSEEK(FILE_HANDLE_STRUCT *file, long offset, int whence);
unsigned int FREAD(void *data, unsigned int size, FILE_HANDLE_STRUCT *file);
void SPLIT_PATH(const char *path, char *drive, char *dir, char *fname, char *ext);


// One step of the file-name hash that seeds the encryption key.
static inline int Mix(unsigned char c, char k) { return (unsigned char)(c ^ (unsigned char)(k + 3)); }

// The encryption key: a hash of the file's name and extension.
static inline unsigned char FileNameKey(char *fileName, char *drive)
{
    char dir[0x100];
    char fname[0x110];
    char ext[0x100];
    unsigned char *p;
    unsigned char hash;
    int i;
    int len;
    SPLIT_PATH(fileName, drive, dir, fname, ext);
    strcat(fname, ext);
    p = (unsigned char *)fname;
    hash = 0xFA;
    len = strlen(fname);
    for (i = 0; i < len; i++) {
        hash += Mix(*p, hash);
        p++;
    }
    return hash;
}

// Undoes SaveResourceFile's XOR encryption.
static inline void DecryptData(unsigned char *data, unsigned int size, unsigned char key)
{
    unsigned char *last = &data[size - 1];
    if (size > 1) {
        do {
            unsigned char c = *data;
            *data = (char)c ^ (char)key;
            key += c;
            data++;
        } while (data != last);
    }
    *data = (char)*data ^ (char)key;
}

int DataAccess::LoadResourceFile(char *name, int activate, int encrypted, unsigned int *type, void *userData,
                                 unsigned int userDataMax, unsigned int *userDataSize, int keepOld)
{
    int result = 1;
    int slot;
    int dataStart;
    int tablePos;
    FILE_HANDLE_STRUCT *file;
    unsigned int version;
    int ok;
    int namesSize;
    unsigned char *cacheBuf;
    unsigned char *last;
    char nameBuf[0x110];
    unsigned int header[2];
    unsigned int info[2];
    unsigned int dirHeader[2];
    unsigned int tableSize;
    unsigned int nameLen;
    int offset;
    unsigned int length;
    char drive[4];
    unsigned int fileSize;
    unsigned int flags;
    unsigned char key;
    int device;
    int remaining;
    unsigned int *dataType;
    unsigned int numFiles;
    int i;
    unsigned char *end;
    unsigned char *p;
    int more;
    unsigned int carry;

    tableSize = 0;
    fileSize = 0;
    slot = -1;
    flags = 0;
    dataStart = 0;
    tablePos = 0;
    key = 0;
    if (!keepOld && m_numDevices > 1) {
        VirtualDataDevice *d = m_devices;
        VirtualDataDevice *last = &m_devices[m_numDevices - 1];
        do {
            if (d->m_name && !strcasecmp(d->m_name, name)) {
                if (d->m_flags & 0x20) {
                    DropDevice(d, 1);
                    d = last;   // (retail then steps past last)
                } else {
                    return 0;
                }
            }
            d++;
        } while (d != last);
    }
    if (m_x110 != -1) {
        slot = m_x110;
        m_caches[m_x110].m_file = -1;
        m_caches[m_x110].m_x10 = -0x6000;
        m_x110 = -1;
    } else {
        int found = 0;
        int n = 0;
        do {
            DeviceCache *c = &m_caches[n];
            if (c->m_file != -2) {
                if (c->m_file == -1 || !(m_virtualFiles[(int)c->m_file].m_flags & 4)) {
                    c->m_x10 = -0x6000;
                    found = 1;
                    c->m_file = -1;
                    slot = n;
                }
            }
            n++;
        } while (n < 8 && !found);
        if (!found) {
            slot = -1;
            PSXprintf("NOTICE: Insufficient slots in virtual file cache. Please close unused files.\n");
        }
    }
    if (m_nextDevice == -1) {
        m_numDevices++;
        m_devices = (VirtualDataDevice *)realloc(m_devices, m_numDevices * sizeof(VirtualDataDevice));
        if (!m_devices) {
#line 457 "DataAccess.cpp"
            ASSERT(0, "LoadResourceFile: realloc of DeviceList failed");
            return 0;
        }
        device = m_numDevices - 1;
        m_nextDevice = -1;
        VirtualDataDevice *d = &m_devices[device];
        d->m_flags = 1;
        d->m_file = 0;
        d->m_id = device;
        d->m_name = 0;
        d->m_x08 = 0;
        d->m_size = 0;
        d->m_x24 = 0;
    } else {
#line 479 "DataAccess.cpp"
        ASSERT(m_nextDevice < m_numDevices, "LoadResourceFile: NextAvailableDevice >= NumberOfDevicesInList");
        device = m_nextDevice;
        m_nextDevice = -1;
        int next = device;
        if (m_numDevices > 1) {
            int searching = 1;
            do {
                next++;
                if (!(next < m_numDevices))
                    next = 0;
                if (next == device) {
                    searching = 0;
                } else if (m_devices[next].m_flags & 1) {
                    m_nextDevice = next;
                    searching = 0;
                }
            } while (searching);
        }
#line 501 "DataAccess.cpp"
        ASSERT(m_nextDevice < m_numDevices, "LoadResourceFile: NextAvailableDevice >= NumberOfDevicesInList");
    }
    file = FOPEN(name, "rb");
    if (!file) {
        result = 0;
        goto done;
    }
    FSEEK(file, 0, 0);
    if (FREAD(header, 8, file) != 8) {
        PSXprintf("Could not load resource file (%s) - read error.\n", name);
        result = 0;
        goto close;
    }
    version = header[0];
    remaining = header[1];
    if (version < 2 || version > 3) {
        PSXprintf("Could not load resource file (%s) - wrong file version.\n", name);
        result = 0;
        goto close;
    }
    flags = 0;
    dataType = 0;   // (retail never uses the type parameter; this local stays null)
    if (version == 2) {
        if (dataType) {
            FREAD(dataType, 4, file);
            remaining -= 4;
        }
    } else {
        FREAD(info, 8, file);
        flags = info[0];
        remaining -= 8;
        if (dataType)
            *dataType = info[1];
    }
    if (encrypted) {
        if (!(flags & 0x80))
            result = 0;
        else if (flags & 0x400)
            key = FileNameKey(name, drive);
        else
            result = 0;
    }
    if (!result)
        goto close;
    if (flags & 0x40) {
        FREAD(&tableSize, 4, file);
        remaining -= 4;
        tablePos = FTELL(file);
        FSEEK(file, tableSize, 1);
        remaining -= tableSize;
    }
    if (userDataMax) {
        userDataMax = (int)userDataMax < remaining ? userDataMax : remaining;
        if ((int)userDataMax > 0) {
            FREAD(userData, userDataMax, file);
            *userDataSize = userDataMax;
        } else {
            *userDataSize = 0;
        }
        remaining -= userDataMax;
    }
    if (remaining)
        FSEEK(file, (unsigned int)remaining, 1);
    FREAD(dirHeader, 8, file);
    numFiles = dirHeader[0];
#line 622 "DataAccess.cpp"
    ASSERT(numFiles != 0, "LoadResourceFile: numberOfItemsToAdd == 0");
    namesSize = dirHeader[1];
    dataStart = (unsigned int)namesSize + FTELL(file);
    nameLen = 0;
    offset = 0;
    length = 0;
    if (slot != -1) {
        if ((unsigned int)namesSize < 0x6000) {
            unsigned char *buf;
            int k;
            buf = (unsigned char *)m_caches[slot].m_data;
            FREAD(buf, namesSize, file);
            k = 0;
            ok = 1;
            do {
                memcpy(&nameLen, buf, 4);
                buf += 4;
                memcpy(nameBuf, buf, nameLen);
                buf += nameLen;
                nameBuf[nameLen] = 0;
                if (flags & 0x400)
                    DecryptData((unsigned char *)nameBuf, nameLen, key);
                memcpy(&offset, buf, 4);
                buf += 4;
                memcpy(&length, buf, 4);
                buf += 4;
                if (!AddFile(nameBuf, device, offset, length, 0, numFiles - k))
                    ok = 0;
                k++;
            } while (k != numFiles);
        } else {
            ok = 1;
            i = 0;
            carry = 0;
            cacheBuf = (unsigned char *)m_caches[slot].m_data;
            end = cacheBuf + 0x6000;
            do {
                int chunk = 0x6000 - (carry + 1);
                chunk = namesSize < chunk ? namesSize : chunk;
                namesSize -= chunk;
                p = cacheBuf;
                FREAD(cacheBuf + carry, chunk, file);
                more = 1;
                do {
                    last = p;
                    if (end - p > 4) {
                        memcpy(&nameLen, p, 4);
                        p += 4;
                        if ((int)(end - p) > (int)(nameLen + 8)) {
                            memcpy(nameBuf, p, nameLen);
                            p += nameLen;
                            nameBuf[nameLen] = 0;
                            if (flags & 0x400)
                                DecryptData((unsigned char *)nameBuf, nameLen, key);
                            memcpy(&offset, p, 4);
                            p += 4;
                            memcpy(&length, p, 4);
                            p += 4;
                            if (!AddFile(nameBuf, device, offset, length, 0, numFiles - i))
                                ok = 0;
                            i++;
                        } else {
                            more = 0;
                        }
                    } else {
                        more = 0;
                    }
                } while (more && i != numFiles);
                carry = end - last - 1;
                memcpy(cacheBuf, last, carry);
            } while (i != numFiles);
        }
    } else {
        ok = 1;
        i = 0;
        do {
            FREAD(&nameLen, 4, file);
            FREAD(nameBuf, nameLen, file);
            nameBuf[nameLen] = 0;
            if (flags & 0x400)
                DecryptData((unsigned char *)nameBuf, nameLen, key);
            FREAD(&offset, 4, file);
            FREAD(&length, 4, file);
            if (!AddFile(nameBuf, device, offset, length, 0, numFiles - i))
                ok = 0;
            i++;
        } while (i != numFiles);
    }
    if (!ok) {
        result = 0;
        for (int f = 0; f < m_numVirtualFiles; f++) {
            if (m_virtualFiles[f].m_device == device)
                FreeFile(f);
        }
    }
    if (version == 2) {
        for (int f = 0; f < m_numVirtualFiles; f++) {
            if (m_virtualFiles[f].m_device == device)
                m_virtualFiles[f].m_offset -= dataStart;
        }
    }
close:
    FSEEK(file, 0, 2);
    fileSize = FTELL(file);
    FCLOSE(file);
done:
    m_x110 = slot;
    if (result) {
        VirtualDataDevice *d = &m_devices[device];
#line 830 "DataAccess.cpp"
        ASSERT(d->m_name == 0, "LoadResourceFile: currentDevice->fileName != NULL");
        ASSERT(d->m_flags & 1, "LoadResourceFile: currentDevice->flagData doesn't include DATAACCESS_FLAG_AVAILABLE");
        d->m_flags = flags | 2;
        d->m_file = 0;
        d->m_name = (char *)malloc(strlen(name) + 1);
        strcpy(d->m_name, name);
        d->m_x08 = 0;
        d->m_size = fileSize;
        d->m_dataOffset = dataStart;
        d->m_tableSize = tableSize;
        d->m_tableOffset = tablePos;
        d->m_x24 = key;
        if (activate) {
            ActivateDevice(d, 0);
            UpdateDeviceCache(d, 0);
        }
    } else {
        FreeDevice(&m_devices[device]);
        m_nextDevice = device;
    }
    return result;
}


// ---- src/DataAccess/LoadDiskFile.cpp

extern "C" void *realloc(void *p, unsigned int size);
extern "C" void *malloc(unsigned int size);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);

struct FILE_HANDLE_STRUCT;
FILE_HANDLE_STRUCT *FOPEN(char *name, const char *mode);
int FCLOSE(FILE_HANDLE_STRUCT *file);
long FTELL(FILE_HANDLE_STRUCT *file);
int FSEEK(FILE_HANDLE_STRUCT *file, long offset, int whence);



// An inlined strchr.
static inline const char *FindChar(const char *s, char c)
{
    while (*s && *s != c)
        s++;
    return *s == c ? s : 0;
}

// Opens a file on disk as a new virtual data device holding one file.
int DataAccess::LoadDiskFile(char *name, const char *mode, long *index)
{
    char openMode[0x10];
    int device;
    int ok;
    int size;
    FILE_HANDLE_STRUCT *file;
    ok = 1;
    size = 0;

    if (m_nextDevice == -1) {
        m_numDevices++;
        m_devices = (VirtualDataDevice *)realloc(m_devices, m_numDevices * sizeof(VirtualDataDevice));
        if (!m_devices) {
#line 895 "DataAccess.cpp"
            ASSERT(0, "LoadDiskFile: realloc of DeviceList failed");
            return 0;
        }
        device = m_numDevices - 1;
        m_nextDevice = -1;
        VirtualDataDevice *d = &m_devices[device];
        d->m_flags = 1;
        d->m_file = 0;
        d->m_id = device;
        d->m_name = 0;
        d->m_x08 = 0;
        d->m_size = 0;
        d->m_x24 = 0;
    } else {
#line 917 "DataAccess.cpp"
        ASSERT(m_nextDevice < m_numDevices, "LoadDiskFile: NextAvailableDevice >= NumberOfDevicesInList");
        device = m_nextDevice;
        m_nextDevice = -1;
        int next = device;
        if (m_numDevices > 1) {
            int searching = 1;
            do {
                next++;
                if (next >= m_numDevices)
                    next = 0;
                if (next == device) {
                    searching = 0;
                } else if (m_devices[next].m_flags & 1) {
                    m_nextDevice = next;
                    searching = 0;
                }
            } while (searching);
        }
#line 939 "DataAccess.cpp"
        ASSERT(m_nextDevice < m_numDevices, "LoadDiskFile: NextAvailableDevice >= NumberOfDevicesInList");
    }
    strcpy(openMode, mode);
    if (!FindChar(openMode, 'b'))
        strcat(openMode, "b");
    file = FOPEN(name, openMode);
    if (!file) {
        ok = 0;
    } else {
        FSEEK(file, 0, 2);
        size = FTELL(file);
        if (!AddFile(name, device, 0, size, index, 5)) {
            ok = 0;
            FCLOSE(file);
        }
    }
    if (ok) {
        VirtualDataDevice *d = &m_devices[device];
#line 974 "DataAccess.cpp"
        ASSERT(d->m_name == 0, "LoadDiskFile: currentDevice->fileName != NULL");
        ASSERT(d->m_flags & 1, "LoadDiskFile: currentDevice->flagData doesn't include DATAACCESS_FLAG_AVAILABLE");
        d->m_flags = 4;
        d->m_file = file;
        d->m_x08 = 0;
        d->m_size = size;
        d->m_name = (char *)malloc(strlen(name) + 1);
        strcpy(d->m_name, name);
        if (FindChar(mode, 'w') || FindChar(mode, 'a') || FindChar(mode, '+'))
            d->m_flags |= 0x10;
        d->m_dataOffset = 0;
        d->m_tableSize = 0;
        d->m_tableOffset = 0;
        d->m_x24 = 0;
        d->m_id = device;
    } else {
        FreeDevice(&m_devices[device]);
        m_nextDevice = device;
    }
    return ok;
}


// ---- src/DataAccess/AddFile.cpp

extern "C" int strcasecmp(const char *a, const char *b);
extern "C" void *realloc(void *p, unsigned int size);
extern "C" void *malloc(unsigned int size);
extern "C" char *strcpy(char *dst, const char *src);



int DataAccess::AddFile(char *name, int device, int size, unsigned int x14, long *index, int grow)
{
    int i;
    for (i = 0; i < m_numVirtualFiles; i++) {
        if (m_virtualFiles[i].m_name && !strcasecmp(name, m_virtualFiles[i].m_name))
            return 1;
    }
    if (m_freeFile == -1) {
        grow = (grow > 1) ? grow : 1;
        i = m_numVirtualFiles + grow;
        m_virtualFiles = (VirtualFile *)realloc(m_virtualFiles, i * sizeof(VirtualFile));
        if (!m_virtualFiles)
            return 0;
        m_freeFile = m_numVirtualFiles;
        do {
            m_virtualFiles[m_numVirtualFiles].m_flags = 1;
            m_virtualFiles[m_numVirtualFiles].m_name = 0;
            m_virtualFiles[m_numVirtualFiles].m_device = 0;
            m_virtualFiles[m_numVirtualFiles].m_offset = 0;
            m_virtualFiles[m_numVirtualFiles].m_position = 0;
            m_virtualFiles[m_numVirtualFiles].m_length = 0;
            m_virtualFiles[m_numVirtualFiles].m_cache = 0;
            m_virtualFiles[m_numVirtualFiles].m_data = 0;
            m_numVirtualFiles++;
        } while (m_numVirtualFiles != i);
    }
    int file = m_freeFile;
    m_freeFile = -1;
    // Find the next free entry for the following AddFile.
    int next = file + 1;
    if (m_numVirtualFiles > 1) {
        int searching = 1;
        if (next == m_numVirtualFiles)
            next = 0;
        do {
            if (m_virtualFiles[next].m_flags & 1) {
                m_freeFile = next;
                searching = 0;
            } else {
                next++;
                if (next == m_numVirtualFiles)
                    next = 0;
                if (next == file)
                    searching = 0;
            }
        } while (searching);
    }
    VirtualFile *f = &m_virtualFiles[file];
    f->m_flags = 0;
#line 1094 "DataAccess.cpp"
    ASSERTF(f->m_name == 0, ("AddFile: currentFile->fileName (%s) != NULL", f->m_name));
    f->m_name = (char *)malloc(strlen(name) + 1);
    strcpy(f->m_name, name);
    f->m_device = device;
    f->m_offset = size;
    f->m_position = 0;
    f->m_length = x14;
    f->m_cache = 0;
    f->m_data = 0;
    if (index)
        *index = file;
    return 1;
}


// ---- src/DataAccess/DropDevice.cpp
extern "C" int strcasecmp(const char *a, const char *b);



// Drops the device with this name (the last device is tested outside the loop,
// as in DropAllDevices).
int DataAccess::DropDevice(char *name, int force)
{
    VirtualDataDevice *device = m_devices;
    if (!device)
        return 0;
    VirtualDataDevice *last = &m_devices[m_numDevices - 1];
    if (m_numDevices > 1) {
        do {
            if (device->m_name && !strcasecmp(device->m_name, name))
                return DropDevice(device, force);
            device++;
        } while (device != last);
    }
    if (device->m_name && !strcasecmp(device->m_name, name))
        return DropDevice(device, force);
    return 0;
}



// ---- src/DataAccess/DropDevice_002BE630.cpp



// Free the device's files; the device itself goes only if none is in use
// (unless forced).
int DataAccess::DropDevice(VirtualDataDevice *device, int force)
{
    int canFree = 1;
    device->m_flags |= 0x20;
    // retail tests the count only after the first file (a do-while)
    int i = 0;
    do {
        VirtualFile *file = &m_virtualFiles[i];
        if (file->m_device == device->m_id) {
            if (force)
                FreeFile(i);
            else if (file->m_data || (file->m_flags & 4))
                canFree = 0;
            else
                FreeFile(i);
        }
        i++;
    } while (i < m_numVirtualFiles);
    if (canFree)
        FreeDevice(device);
    return 1;
}


// ---- src/DataAccess/DropAllDevices.cpp


// Drops every device (the last one outside the loop); returns 0 if any
// device was still in use.
int DataAccess::DropAllDevices(int force)
{
    int ok = 1;
    VirtualDataDevice *device = m_devices;
    VirtualDataDevice *last = &m_devices[m_numDevices - 1];
    if (m_numDevices > 1) {
        do {
            if (!DropDevice(device, force))
                ok = 0;
            device++;
        } while (device != last);
    }
    if (!DropDevice(device, force))
        ok = 0;
    ResetStow();
    return ok;
}



// ---- src/DataAccess/ActivateDevice.cpp
// Block-form ASSERT (no do/while(0)) gives retail's loop-only label padding.


struct FILE_HANDLE_STRUCT;
FILE_HANDLE_STRUCT *FOPEN(char *name, const char *mode);
int FCLOSE(FILE_HANDLE_STRUCT *file);



inline int DataAccess::ActivateDevice(VirtualDataDevice *device, int arg)
{
#line 1259 "DataAccess.cpp"
    ASSERT(device, "ActivateDevice: device == NULL");
    int found;
    int *p = m_stow;
    if (*p != device->m_id) {
        found = 0;
        do {
            p++;
            if (*p == device->m_id) {
                do {
                    p[0] = p[-1];
                    p--;
                } while (p > m_stow);
                *m_stow = device->m_id;
                found = 1;
                p = m_stowEnd;
            }
        } while (p != m_stowEnd);
        if (!found) {
            int last = *m_stowEnd;
            if (last != -1) {
                VirtualDataDevice *old = &m_devices[last];
                if (!(old->m_flags & 0x10) && old->m_file) {
                    FCLOSE(old->m_file);
                    old->m_file = 0;
                }
            }
            p = m_stowEnd;
            do {
                p[0] = p[-1];
                p--;
            } while (p > m_stow);
            *m_stow = device->m_id;
        }
        if (device->m_flags & 0x100)
            ClaimDeviceCache(device->m_id);
    }
    if (!device->m_file) {
        device->m_file = FOPEN(device->m_name, "rb");
        if (!device->m_file)
            return 0;
    }
    device->m_x08 = arg;
    return 1;
}


// ---- src/DataAccess/FreeFile.cpp
extern "C" void free(void *);




void DataAccess::FreeFile(long file)
{
    VirtualFile *f = &m_virtualFiles[(int)file];
    if (f->m_cache != -1) {
        if (m_caches[f->m_cache].m_file == file) {
            m_caches[f->m_cache].m_file = -1;
            m_x110 = f->m_cache;
        }
    }
    if (f->m_name) {
        free(f->m_name);
        f->m_name = 0;
    }
    f->m_cache = 0;
    f->m_position = 0;
    f->m_device = 0;
    f->m_flags = 1;
    f->m_data = 0;
    f->m_length = 0;
    f->m_offset = 0;
    m_freeFile = file;
}


// ---- src/DataAccess/FreeDevice.cpp
struct FILE_HANDLE_STRUCT;
int FCLOSE(FILE_HANDLE_STRUCT *file);
extern "C" void free(void *p);



// Releases a device: its slot in the stow list is refilled with the topmost
// stowed device id, then the device's file, name and state are cleared.
void DataAccess::FreeDevice(VirtualDataDevice *device)
{
    int found = 0;
    int *p = m_stow;
    do {
        if (*p == device->m_id) {
            if (*m_stowEnd != -1) {
                *p = *m_stowEnd;
                found = 1;
                *m_stowEnd = -1;
            } else {
                int *q = m_stowEnd;
                do {
                    q--;
                    if (*q != -1) {
                        *p = *q;
                        found = 1;
                        *q = -1;
                    }
                } while (q != p && !found);
            }
        }
        p++;
    } while (p <= m_stowEnd && !found);

    if (device->m_id == m_cacheOwner)
        ClaimDeviceCache(-1);
    m_nextDevice = device->m_id;
    if (device->m_file) {
        FCLOSE(device->m_file);
        device->m_file = 0;
    }
    if (device->m_name) {
        free(device->m_name);
        device->m_name = 0;
    }
    device->m_flags = 1;
    device->m_x08 = 0;
    device->m_size = 0;
    device->m_dataOffset = 0;
    device->m_tableSize = 0;
    device->m_tableOffset = 0;
    device->m_x24 = 0;
}


// ---- src/DataAccess/GetStatus.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off




int DataAccess::GetStatus(long dataFile, long *modifyTime, long *size)
{
#line 1473 "DataAccess.cpp"
    ASSERT(dataFile >= 0, "GetStatus: dataFile < 0");
    ASSERTF(dataFile < m_numVirtualFiles, ("GetStatus: dataFile (%d) >= NumberOfFilesInList", dataFile));
    if (!modifyTime && !size)
        return -1;
    if (modifyTime)
        PSXprintf("WARNING: GetStatus 'last modify time' not implemented!\n");
    // Retail writes both even when one pointer is null.
    *modifyTime = 0;
    *size = (unsigned int)m_virtualFiles[(int)dataFile].m_length;
    return 0;
}
#pragma alignlabel reset


// ---- src/DataAccess/FGets.cpp

struct FILE_HANDLE_STRUCT;
FILE_HANDLE_STRUCT *FOPEN(char *name, const char *mode);
int FCLOSE(FILE_HANDLE_STRUCT *file);
long FTELL(FILE_HANDLE_STRUCT *file);
int FSEEK(FILE_HANDLE_STRUCT *file, long offset, int whence);
unsigned int FWRITE(void *data, unsigned int size, unsigned int count, FILE_HANDLE_STRUCT *file);

#define MIN(a, b) ((a) < (b) ? (a) : (b))






// Reads one line (at most size - 2 characters) and leaves the file just
// past it; a "\r\n" ending comes back as "\n".
char *DataAccess::FGets(long file, char *buffer, int size)
{
#line 1554 "DataAccess.cpp"
    ASSERT(file >= 0, "FGets: dataFile < 0");
    ASSERTF(file < m_numVirtualFiles, ("FGets: dataFile (%d) >= NumberOfFilesInList", file));
    VirtualFile *f = &m_virtualFiles[(int)file];
    int remaining = f->m_length - f->m_position;
    if (remaining <= 0) {
        *buffer = 0;
        return 0;
    }
    VirtualDataDevice *device = &m_devices[f->m_device];
    if (ActivateDevice(device, f->m_offset + f->m_position)) {
        remaining = MIN(remaining, size - 2);
        long n = remaining;
        if (!ReadData(device, f->m_cache, buffer, n)) {
            *buffer = 0;
            return 0;
        }
        buffer[n] = 0;
        char *p = buffer;
        while (*p && *p != '\n')
            p++;
        p = *p == '\n' ? p : 0;
        if (p) {
            n = p - buffer + 1;
            if (p > buffer && p[-1] == '\r') {
                p--;
                *p = '\n';
            }
            p++;
            *p = 0;
        }
        f->m_position += n;
        return buffer;
    }
    return 0;
}


// ---- src/DataAccess/FFlush.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off


struct FILE_HANDLE_STRUCT;
int FFLUSH(FILE_HANDLE_STRUCT *f);




int DataAccess::FFlush(long dataFile)
{
#line 1626 "DataAccess.cpp"
    ASSERT(dataFile >= 0, "FFlush: dataFile < 0");
    ASSERTF(dataFile < m_numVirtualFiles, ("FFlush: dataFile (%d) >= NumberOfFilesInList", dataFile));
    FILE_HANDLE_STRUCT *f = m_devices[m_virtualFiles[(int)dataFile].m_device].m_file;
    if (f)
        return FFLUSH(f);
    return 0;
}
#pragma alignlabel reset


// ---- src/DataAccess/FEof.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off




int DataAccess::FEof(long dataFile)
{
#line 1647 "DataAccess.cpp"
    ASSERT(dataFile >= 0, "FEof: dataFile < 0");
    ASSERTF(dataFile < m_numVirtualFiles, ("FEof: dataFile (%d) >= NumberOfFilesInList", dataFile));
    VirtualFile *f = &m_virtualFiles[(int)dataFile];
    if ((unsigned int)f->m_position >= (unsigned int)f->m_length)
        return -1;
    return 0;
}
#pragma alignlabel reset


// ---- src/DataAccess/FTell.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off




long DataAccess::FTell(long dataFile)
{
#line 1666 "DataAccess.cpp"
    ASSERT(dataFile >= 0, "FTell: dataFile < 0");
    ASSERTF(dataFile < m_numVirtualFiles, ("FTell: dataFile (%d) >= NumberOfFilesInList", dataFile));
    return m_virtualFiles[(int)dataFile].m_position;
}
#pragma alignlabel reset


// ---- src/DataAccess/FSeek.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off




int DataAccess::FSeek(long dataFile, long offset, int origin)
{
#line 1682 "DataAccess.cpp"
    ASSERT(dataFile >= 0, "FSeek: dataFile < 0");
    ASSERTF(dataFile < m_numVirtualFiles, ("FSeek: dataFile (%d) >= NumberOfFilesInList", dataFile));
    VirtualFile *f = &m_virtualFiles[(int)dataFile];
    if (origin == 0)            // SEEK_SET
        f->m_position = 0;
    else if (origin == 2)       // SEEK_END
        f->m_position = f->m_length;
    f->m_position += offset;
    return 0;
}
#pragma alignlabel reset


// ---- src/DataAccess/FWrite.cpp

struct FILE_HANDLE_STRUCT;
FILE_HANDLE_STRUCT *FOPEN(char *name, const char *mode);
int FCLOSE(FILE_HANDLE_STRUCT *file);
long FTELL(FILE_HANDLE_STRUCT *file);
int FSEEK(FILE_HANDLE_STRUCT *file, long offset, int whence);
unsigned int FWRITE(void *data, unsigned int size, unsigned int count, FILE_HANDLE_STRUCT *file);





unsigned int DataAccess::FWrite(long file, void *data, unsigned int size, unsigned int count)
{
#line 1716 "DataAccess.cpp"
    ASSERT(file >= 0, "FWrite: dataFile < 0");
    ASSERTF(file < m_numVirtualFiles, ("FWrite: dataFile (%d) >= NumberOfFilesInList", file));
    VirtualFile *f = &m_virtualFiles[(int)file];
    VirtualDataDevice *device = &m_devices[f->m_device];
    if (ActivateDevice(device, f->m_offset + f->m_position)) {
        if (device->m_x08 != FTELL(device->m_file))
            FSEEK(device->m_file, device->m_x08, 0);
        unsigned int written = FWRITE(data, size, count, device->m_file);
        f->m_position += written * size;
        device->m_x08 += written * size;
        return written;
    }
    return 0;
}


// ---- src/DataAccess/FRead.cpp

struct FILE_HANDLE_STRUCT;
FILE_HANDLE_STRUCT *FOPEN(char *name, const char *mode);
int FCLOSE(FILE_HANDLE_STRUCT *file);
long FTELL(FILE_HANDLE_STRUCT *file);
int FSEEK(FILE_HANDLE_STRUCT *file, long offset, int whence);
unsigned int FWRITE(void *data, unsigned int size, unsigned int count, FILE_HANDLE_STRUCT *file);

#define MIN(a, b) ((a) < (b) ? (a) : (b))






unsigned int DataAccess::FRead(long file, void *buffer, unsigned int size, unsigned int count)
{
#line 1746 "DataAccess.cpp"
    ASSERT(file >= 0, "FRead: dataFile < 0");
    ASSERTF(file < m_numVirtualFiles, ("FRead: dataFile (%d) >= NumberOfFilesInList", file));
    ASSERT(m_virtualFiles[(int)file].m_flags & 4, "FRead: File not open!");
    VirtualFile *f = &m_virtualFiles[(int)file];
    VirtualDataDevice *device = &m_devices[f->m_device];
    if (ActivateDevice(device, f->m_offset + f->m_position)) {
        int left = f->m_length - f->m_position;
        // MIN, not an if: the if fills the branch's delay slot, retail doesn't
        long n = MIN((int)(size * count), left);
        if (n <= 0)
            return 0;
        long read = ReadData(device, f->m_cache, buffer, n);
        f->m_position += read;
        if (read == n)
            goto done;
        if (read == 0)
            goto none;
        return n / read;    // retail divides by the bytes read, not by size
    done:
        return count;
    }
none:
    return 0;
}


// ---- src/DataAccess/FRead_002BFE40.cpp

struct FILE_HANDLE_STRUCT;
FILE_HANDLE_STRUCT *FOPEN(char *name, const char *mode);
int FCLOSE(FILE_HANDLE_STRUCT *file);
long FTELL(FILE_HANDLE_STRUCT *file);
int FSEEK(FILE_HANDLE_STRUCT *file, long offset, int whence);
unsigned int FWRITE(void *data, unsigned int size, unsigned int count, FILE_HANDLE_STRUCT *file);

#define MIN(a, b) ((a) < (b) ? (a) : (b))






unsigned int DataAccess::FRead(long file, void *buffer, unsigned int size)
{
#line 1782 "DataAccess.cpp"
    ASSERT(file >= 0, "FRead: dataFile < 0");
    ASSERTF(file < m_numVirtualFiles, ("FRead: dataFile (%d) >= NumberOfFilesInList", file));
    ASSERT(m_virtualFiles[(int)file].m_flags & 4, "FRead: File not open!");
    VirtualFile *f = &m_virtualFiles[(int)file];
    VirtualDataDevice *device = &m_devices[f->m_device];
    if (ActivateDevice(device, f->m_offset + f->m_position)) {
        size = MIN((int)size, f->m_length - f->m_position);
        long n = (int)size;
        if (n <= 0)
            return 0;
        unsigned long read = ReadData(device, f->m_cache, buffer, n);
        f->m_position += read;
        return read;
    }
    return 0;
}


// ---- src/DataAccess/UpdateDeviceCache.cpp
struct FILE_HANDLE_STRUCT;
void FREAD_ASYNC_ABORT(FILE_HANDLE_STRUCT *file);
int FSEEK(FILE_HANDLE_STRUCT *file, long offset, int whence);
unsigned int FREAD(void *data, unsigned int size, FILE_HANDLE_STRUCT *file);
extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);
extern "C" void *memset(void *p, int c, unsigned int n);



inline void DataAccess::UpdateDeviceCache(VirtualDataDevice *device, int block)
{
    FREAD_ASYNC_ABORT(device->m_file);
    int compressed = device->m_flags & 0x40;
    int extra = 0;
    if (device->m_flags & 0x200)
        extra = 1;
    if (m_cacheFirst == -1 && compressed) {
        if (m_blockSizes)
            free(m_blockSizes);
        m_blockSizes = (unsigned short *)malloc(device->m_tableSize);
        FSEEK(device->m_file, device->m_tableOffset, 0);
        FREAD(m_blockSizes, device->m_tableSize, device->m_file);
    }
    if (extra)
        memset(m_cache, 0, m_cacheSize);
    int offset = device->m_dataOffset;
    if (compressed) {
        // Guarded do-while: a plain for loop gives slt/bnez instead of blez/bne.
        if (block > 0) {
            unsigned short *sizes = m_blockSizes;
            int i = 0;
            do {
                offset += *sizes + extra;
                i++;
                sizes++;
            } while (i != block);
        }
    } else {
        offset += block * (extra + 0x6000);
    }
    FSEEK(device->m_file, offset, 0);
    int size = device->m_size - offset;
    int cacheSize = m_cacheSize;
    size = size < cacheSize ? size : cacheSize;
    int got;
    if (extra) {
        m_x13C = 1;
        got = FREAD(m_cache, size, device->m_file);
    } else {
        got = FREAD(m_cache, size, device->m_file);
    }
    m_cacheFirst = block;
    if (compressed) {
        m_cacheLast = m_cacheFirst;
        unsigned short *p = &m_blockSizes[m_cacheFirst];
        do {
            got -= *p++ + extra;
            if (*p < got)
                m_cacheLast++;
        } while (got > 0);
    } else {
        m_cacheLast = m_cacheFirst + got / (extra + 0x6000);
        if (got % (extra + 0x6000))
            m_cacheLast--;
    }
}


// ---- src/DataAccess/ReadData.cpp

struct FILE_HANDLE_STRUCT;
void FREAD_ASYNC_ABORT(FILE_HANDLE_STRUCT *file);
int FREAD_ASYNC_STATUS(FILE_HANDLE_STRUCT *file, unsigned int *read, int *error);
int FSEEK(FILE_HANDLE_STRUCT *file, long offset, int whence);
unsigned int FREAD(void *data, unsigned int size, FILE_HANDLE_STRUCT *file);
extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);
extern "C" void *memset(void *p, int c, unsigned int n);
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
void *malloc_align(unsigned int size, unsigned int align);
int ZLibDecompress(void *dst, void *src, int size);

#define MIN(a, b) ((a) < (b) ? (a) : (b))



// UpdateDeviceCache (matched on its own in UpdateDeviceCache.cpp) is defined
// just before ReadData in retail's DataAccess.cpp and inlined into it twice;
// `inline` reproduces that. The cacheSize local is needed for the inlined
// copies' register order (the out-of-line copy matches either way).

// Undoes SaveResourceFile's XOR encryption: the running key advances by each
// encrypted byte.

unsigned long DataAccess::ReadData(VirtualDataDevice *device, int cache, void *buffer, unsigned int size)
{
    char *dest = (char *)buffer;
    DeviceCache *c;
    long pos;
    long n;
    long count;
    int left;
    if (!(device->m_flags & 0x100)) {
        if (cache == -1 || size >= 0x6000) {
            FSEEK(device->m_file, device->m_dataOffset + device->m_x08, 0);
            if (FREAD(dest, size, device->m_file) != size)
                return 0;
            device->m_x08 += size;
            return size;
        }
        DeviceCache *dc = &m_caches[cache];
        pos = device->m_x08 - dc->m_x10;
        if (pos < 0 || pos >= 0x6000) {
            dc->m_x10 = device->m_x08;
            left = device->m_size - (device->m_dataOffset + device->m_x08);
            n = left > 0x6000 ? 0x6000 : left;
            FSEEK(device->m_file, device->m_dataOffset + device->m_x08, 0);
            if (FREAD(dc->m_data, n, device->m_file) != (unsigned int)n)
                return 0;
            pos = 0;
        }
        count = MIN((int)size, (int)(0x6000 - pos));
        memcpy(dest, dc->m_data + pos, count);
        device->m_x08 += count;
        if ((unsigned int)count == size)
            return size;
        dest += count;
        dc->m_x10 = device->m_x08;
        left = device->m_size - (device->m_dataOffset + device->m_x08);
        n = left > 0x6000 ? 0x6000 : left;
        FSEEK(device->m_file, device->m_dataOffset + device->m_x08, 0);
        if (FREAD(dc->m_data, n, device->m_file) != (unsigned int)n)
            return count;
        count = size - count;
        memcpy(dest, dc->m_data, count);
        device->m_x08 += count;
        return size;
    }
    if (cache < 0) {
        c = (DeviceCache *)malloc_align(0x40, 0x18);
        if (!c)
            return 0;
        c->m_file = -2;
        c->m_x10 = -0x6000;
        c->m_data = (unsigned char *)malloc(0x6000);
        if (!c->m_data) {
            free(c);
            return 0;
        }
    } else {
        c = &m_caches[cache];
    }
    unsigned int done = 0;
    do {
        pos = device->m_x08 - c->m_x10;
        if (pos < 0 || pos >= 0x6000) {
            int block = device->m_x08 / 0x6000;
            if (block < m_cacheFirst || block > m_cacheLast)
                UpdateDeviceCache(device, block);
            int skip = block - m_cacheFirst;
            int offset = 0;
            unsigned short csize;
            if (device->m_flags & 0x40) {
                int extra = 0;
                if (device->m_flags & 0x200)
                    extra = 1;
                unsigned short *p = &m_blockSizes[m_cacheFirst];
                unsigned short *end = p + skip;
                if (skip > 0) {
                    do {
                        offset += *p + extra;
                        p++;
                    } while (p != end);
                }
                csize = *p;
            } else {
                if (device->m_flags & 0x200)
                    offset += skip * 0x6001;
                else
                    offset += skip * 0x6000;
                csize = 0x6000;
            }
            if (device->m_flags & 0x200) {
                int error = 0;
                unsigned int got = 0;
                if (m_x13C) {
                    while (m_x13C && got < offset + csize)
                        m_x13C = FREAD_ASYNC_STATUS(device->m_file, &got, &error);
                }
                if (error) {
                    PSXprintf("CRITICAL DISK READ ERROR IN DataAccess::ReadData, retrying...\n");
                    int ok;
                    do {
                        ok = 1;
                        UpdateDeviceCache(device, block);
                        got = 0;
                        offset = 0;
                        m_x13C = FREAD_ASYNC_STATUS(device->m_file, &got, &error);
                        if (m_x13C) {
                            while (m_x13C && got < offset + csize)
                                m_x13C = FREAD_ASYNC_STATUS(device->m_file, &got, &error);
                        }
                        if (error)
                            ok = 0;
                    } while (!ok);
                }
                unsigned char *check = m_cache + offset + csize;
#line 2154 "DataAccess.cpp"
                ASSERTF(*check == 255, ("ReadData: (*checkAddress [%d]) != 255", *check));
            }
            c->m_x10 = block * 0x6000;
            if (device->m_flags & 0x40) {
                if (csize != 0x6000)
                    ZLibDecompress(c->m_data, m_cache + offset, 0x6000);
                else
                    memcpy(c->m_data, m_cache + offset, 0x6000);
            } else {
                memcpy(c->m_data, m_cache + offset, 0x6000);
            }
            if ((device->m_flags & 0x80) && (device->m_flags & 0x400))
                DecryptData(c->m_data, 0x6000, (device->m_x24 + (unsigned int)block) % 255);
            pos = device->m_x08 - c->m_x10;
        }
        count = MIN((int)(size - done), (int)(0x6000 - pos));
        memcpy(dest, c->m_data + pos, count);
        done += count;
        device->m_x08 += count;
        dest += count;
    } while (done != size);
    if (c->m_file == -2) {
        free(c->m_data);
        free(c);
    }
    return done;
}


// ---- src/DataAccess/FClose.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off


struct FILE_HANDLE_STRUCT;
int FCLOSE(FILE_HANDLE_STRUCT *file);





int DataAccess::FClose(long file)
{
#line 2228 "DataAccess.cpp"
    ASSERT(file >= 0, "FClose: dataFile < 0");
    ASSERTF(file < m_numVirtualFiles, ("FClose: dataFile (%d) >= NumberOfFilesInList", file));
    VirtualFile *f = &m_virtualFiles[(int)file];
    if (!(f->m_flags & 4))
        return 0;
    f->m_position = 0;
    f->m_flags &= ~4;
    if (m_caches[f->m_cache].m_file == file)
        m_x110 = f->m_cache;
    VirtualDataDevice *device = &m_devices[f->m_device];
    if (!(device->m_flags & 2)) {
        if (device->m_file) {
            FCLOSE(device->m_file);
            device->m_file = 0;
        }
        if (!f->m_data && !(f->m_flags & 8)) {
            FreeDevice(device);
            FreeFile(file);
        }
    }
    return 1;
}
#pragma alignlabel reset


// ---- src/DataAccess/FOpen.cpp

// An inlined strchr.
static inline const char *FindChar2(const char *s, char c)
{
    while (*s && *s != c)
        s++;
    if (*s == c)
        return s;
    return 0;
}

long DataAccess::FOpen(char *name, const char *mode)
{
    long file = FindVirtualFile(name);
    if (file == -1) {
        // Not a virtual file: without the file system, read-only opens ("r" without "+") fail.
        if (m_useFileSystem || !FindChar2(mode, 'r') || FindChar2(mode, '+')) {
            if (!LoadDiskFile(name, mode, &file))
                return -1;
        } else {
            return -1;
        }
    }
    OpenVirtualFile(file);
    return file;
}


// ---- src/DataAccess/OpenVirtualFile.cpp




void DataAccess::OpenVirtualFile(long file)
{
#line 2341 "DataAccess.cpp"
    ASSERT(!(m_virtualFiles[(int)file].m_flags & 4), "OpenVirtualFile: CAN'T OPEN A FILE THAT IS ALREADY OPEN!!");
    VirtualFile *f = &m_virtualFiles[(int)file];
    f->m_flags |= 4;
    f->m_position = 0;
    if (m_caches[f->m_cache].m_file != file) {
        if (m_x110 != -1) {
            f->m_cache = m_x110;
            m_caches[m_x110].m_file = file;
            m_caches[m_x110].m_x10 = -0x6000;
            m_x110 = -1;
        } else {
            int found = 0;
            int i = 0;
            do {
                DeviceCache *c = &m_caches[i];
                if (c->m_file != -2) {
                    if (c->m_file == -1 || !(m_virtualFiles[(int)c->m_file].m_flags & 4)) {
                        f->m_cache = i;
                        c->m_x10 = -0x6000;
                        found = 1;
                        c->m_file = file;
                    }
                }
                i++;
            } while (i < 8 && !found);
            if (!found) {
                f->m_cache = -1;
                PSXprintf("NOTICE: Insufficient slots in virtual file cache. Please close unused files.\n");
            }
        }
    } else if (m_x110 == f->m_cache) {
        m_x110 = -1;
    }
    if (m_saveTag)
        f->m_flags |= 8;
}


// ---- src/DataAccess/FindVirtualFile.cpp
extern "C" int strcasecmp(const char *a, const char *b);



long DataAccess::FindVirtualFile(char *name)
{
    m_lastVirtualFile++;
    if (m_lastVirtualFile == m_numVirtualFiles)
        m_lastVirtualFile = 0;
    int i = m_lastVirtualFile;
    do {
        if (m_virtualFiles[i].m_name && strcasecmp(name, m_virtualFiles[i].m_name) == 0) {
            return m_lastVirtualFile = i;
        }
        i++;
        if (i == m_numVirtualFiles)
            i = 0;
    } while (i != m_lastVirtualFile);
    return -1;
}


// ---- src/DataAccess/ResetStow.cpp

void DataAccess::ResetStow()
{
    int *p = m_stow;
    do {
        *p = -1;
        p++;
    } while (p <= m_stowEnd);
}


// ---- src/DataAccess/FileExists.cpp
struct FILE_HANDLE_STRUCT;
FILE_HANDLE_STRUCT *FOPEN(char *name, const char *mode);
int FCLOSE(FILE_HANDLE_STRUCT *file);


int DataAccess::FileExists(char *name)
{
    // A one-case switch (retail: beq; nop; b), not an if.
    switch (FindVirtualFile(name)) {
    case -1:
        if (m_useFileSystem) {
            FILE_HANDLE_STRUCT *f = FOPEN(name, "rb");
            if (!f)
                return 0;
            FCLOSE(f);
            return 1;
        }
        return 0;
    }
    return 1;
}



// ---- src/DataAccess/SaveResourceFile.cpp

extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strcat(char *dst, const char *src);
extern "C" void *memset(void *p, int c, unsigned int n);
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);

struct FILE_HANDLE_STRUCT;
FILE_HANDLE_STRUCT *FOPEN(char *name, const char *mode);
int FCLOSE(FILE_HANDLE_STRUCT *file);
long FTELL(FILE_HANDLE_STRUCT *file);
int FSEEK(FILE_HANDLE_STRUCT *file, long offset, int whence);
unsigned int FWRITE(void *data, unsigned int size, unsigned int count, FILE_HANDLE_STRUCT *file);
void SPLIT_PATH(const char *path, char *drive, char *dir, char *fname, char *ext);
int ZLibCompress(void *src, void *dst, int size);



// SaveResourceFile's small locals. Retail keeps them below the big buffers
// and treats the two groups as separate objects (its scheduler moves loads of
// namesSize past stores to nameLen), which these two 16+-byte structs
// reproduce; as separate scalars mwcc would place them after the buffers.
struct ResourceHeader {
    int version;
    unsigned int flags;
    int numFiles;
    int tableSize;
    int headerSize;
    int nameLen;
    unsigned int fileLen;
    int offset;
};

struct ResourceLocals {
    int namesSize;
    char drive[6];
    short zero;
    unsigned short packedSize;
    char pad;
    unsigned char marker;
};

// XORs a block with a running key (the key advances by each encrypted byte).
static inline void EncryptData(unsigned char *data, unsigned int size, unsigned char key)
{
    unsigned char *last = &data[size - 1];
    if (size > 1) {
        do {
            key += (*data ^= key);
            data++;
        } while (data != last);
    }
    *data ^= key;
}


// The encryption key: a hash of the file's name and extension.

int DataAccess::SaveResourceFile(char *fileName, int blockMarkers, int compress, int encrypt,
                                 unsigned int userDataType, void *userData, unsigned int userDataSize, int untag)
{
    int counter = 0;
    unsigned int method = 0x400;    // XOR encryption; a variable, so `flags |= method` is li + or
    ResourceHeader header;
    ResourceLocals entry;
    char name[0x110];
    unsigned char block[0x6000];
    unsigned char packed[0xC000];
    FILE_HANDLE_STRUCT *file;
    int blocks;
    int markers;
    int blocked;
    int tablePos;
    unsigned char key;
    unsigned int blockIndex;
    long source;
    int remaining;
    int size;
    unsigned int pos;

    key = FileNameKey(fileName, entry.drive);
    if (!userData) {
        if (userDataSize)
            return 0;
    } else {
        if (!userDataSize)
            return 0;
        if (!userDataType) {
            PSXprintf("RESOURCE FILE NOT SAVED!! If including user data, please specify a user data type other than 0.\n");
            return 0;
        }
    }
    file = FOPEN(fileName, "wb");
    if (!file)
        return 0;
    header.version = 3;
    FWRITE(&header.version, 4, 1, file);
    header.flags = 0;
    markers = 0;
    blocked = 0;
    if (blockMarkers) {
        blocked = markers = 1;
        header.flags |= 0x100;
        header.flags |= 0x200;
    }
    if (compress) {
        header.flags |= 0x40;
        blocked = 1;
        header.flags |= 0x100;
    }
    if (encrypt) {
        header.flags |= 0x80;
        blocked = 1;
        header.flags |= 0x100;
        header.flags |= method;
    }
    header.numFiles = 0;
    blocks = 0;
    do {
        if (m_virtualFiles[counter].m_flags & 8) {
            blocks += (unsigned int)m_virtualFiles[counter].m_length / 0x6000;
            if ((unsigned int)m_virtualFiles[counter].m_length % 0x6000)
                blocks++;
            header.numFiles++;
        }
        counter++;
    } while (counter < m_numVirtualFiles);
    header.tableSize = (blocks + 1) * 2;
    header.headerSize = userDataSize + 8;
    if (compress)
        header.headerSize += header.tableSize + 4;
    FWRITE(&header.headerSize, 4, 1, file);
    FWRITE(&header.flags, 4, 1, file);
    FWRITE(&userDataType, 4, 1, file);
    tablePos = 0;
    if (compress) {
        FWRITE(&header.tableSize, 4, 1, file);
        tablePos = FTELL(file);
        for (counter = 0; counter <= blocks; counter++) {
            entry.zero = 0;
            FWRITE(&entry.zero, 2, 1, file);
        }
    }
    if (userDataSize)
        FWRITE(userData, userDataSize, 1, file);
    FWRITE(&header.numFiles, 4, 1, file);
    header.nameLen = 0;
    header.fileLen = 0;
    header.offset = 0;
    entry.namesSize = 0;
    counter = 0;
    do {
        if (m_virtualFiles[counter].m_flags & 8) {
#line 2791 "DataAccess.cpp"
            ASSERT(m_virtualFiles[counter].m_name, "SaveResourceFile: FileList[counter].fileName==NULL");
            header.nameLen = strlen(m_virtualFiles[counter].m_name);
            entry.namesSize += 4;
            entry.namesSize += header.nameLen;
            entry.namesSize += 4;
            entry.namesSize += 4;
        }
        counter++;
    } while (counter < m_numVirtualFiles);
    FWRITE(&entry.namesSize, 4, 1, file);
    header.offset = 0;
    counter = 0;
    do {
        if (m_virtualFiles[counter].m_flags & 8) {
#line 2813 "DataAccess.cpp"
            ASSERT(m_virtualFiles[counter].m_name, "SaveResourceFile: FileList[counter].fileName==NULL");
            strcpy(name, m_virtualFiles[counter].m_name);
            header.nameLen = strlen(m_virtualFiles[counter].m_name);
            header.fileLen = m_virtualFiles[counter].m_length;
            if (encrypt) {
                if (method & 0x400)
                    EncryptData((unsigned char *)name, header.nameLen, key);
            }
            FWRITE(&header.nameLen, 4, 1, file);
            FWRITE(name, header.nameLen, 1, file);
            FWRITE(&header.offset, 4, 1, file);
            FWRITE(&header.fileLen, 4, 1, file);
            if (blocked) {
                int n = (unsigned int)m_virtualFiles[counter].m_length / 0x6000;
                if ((unsigned int)m_virtualFiles[counter].m_length % 0x6000)
                    n++;
                header.offset += n * 0x6000;
            } else {
                header.offset += header.fileLen;
            }
        }
        counter++;
    } while (counter < m_numVirtualFiles);
    blockIndex = 0;
    counter = 0;
    do {
        if (m_virtualFiles[counter].m_flags & 8) {
            entry.packedSize = 0;
            FClose(counter);
            source = FOpen(m_virtualFiles[counter].m_name, "rb");
            remaining = m_virtualFiles[counter].m_length;
            do {
                memset(block, 0, 0x6000);
                size = remaining > 0x6000 ? 0x6000 : remaining;
                if (FRead(source, block, size) != size) {
                    FClose(source);
                    FCLOSE(file);
                    return 0;
                }
                if (encrypt) {
                    if (method & 0x400) {
                        EncryptData(block, 0x6000, (key + blockIndex) % 255);
                    } else {
                        FClose(source);
                        FCLOSE(file);
                        return 0;
                    }
                }
                if (compress) {
                    entry.packedSize = ZLibCompress(block, packed, 0x6000);
                    if (entry.packedSize >= 0x6000) {
                        entry.packedSize = 0x6000;
                        memcpy(packed, block, 0x6000);
                    }
                } else {
                    memcpy(packed, block, 0x6000);
                    entry.packedSize = 0x6000;
                }
                if (compress) {
                    pos = FTELL(file);
                    FSEEK(file, tablePos, 0);
                    FWRITE(&entry.packedSize, 2, 1, file);
                    tablePos = FTELL(file);
                    FSEEK(file, pos, 0);
                }
                if (blocked)
                    FWRITE(packed, entry.packedSize, 1, file);
                else
                    FWRITE(packed, size, 1, file);
                blockIndex++;
                if (markers) {
                    entry.marker = 0xFF;
                    FWRITE(&entry.marker, 1, 1, file);
                }
                remaining -= size;
            } while (remaining);
            FClose(source);
            if (untag)
                m_virtualFiles[counter].m_flags &= ~8;
        }
        counter++;
    } while (counter < m_numVirtualFiles);
    FCLOSE(file);
    return 1;
}


// ---- src/DataAccess/TagForSave.cpp

void DataAccess::TagForSave(int tag)
{
    m_saveTag = tag;
}


// ---- src/DataAccess/AttachObject.cpp
// ASSERT contains inline asm; match retail's label alignment for asm functions.
#pragma alignlabel off




int DataAccess::AttachObject(long dataFile, void *object)
{
#line 3022 "DataAccess.cpp"
    ASSERT(dataFile >= 0, "AttachObject: dataFile < 0");
    ASSERTF(dataFile < m_numVirtualFiles, ("AttachObject: dataFile (%d) >= NumberOfFilesInList", dataFile));
    VirtualFile *entry = &m_virtualFiles[(int)dataFile];
    entry->m_data = object;
    return 1;
}
#pragma alignlabel reset


// ---- src/DataAccess/DetachObject.cpp


void DataAccess::DetachObject(void *object)
{
    VirtualFile *p = m_virtualFiles;
    VirtualFile *last = &m_virtualFiles[m_numVirtualFiles - 1];
    do {
        if (p->m_data == object) {
            p->m_data = 0;
            return;
        }
        p++;
    } while (p != last);
    // The loop stops short of the last entry, which is checked separately.
    if (p->m_data == object)
        p->m_data = 0;
}


// ---- src/DataAccess/GetDataOrFileHandle.cpp
extern "C" int strcasecmp(const char *a, const char *b);



int DataAccess::GetDataOrFileHandle(char *name, const char *mode, long *file, void **data, int keepOpen)
{
    int found = 0;
    if (m_lastFile != -1 && m_virtualFiles[(int)m_lastFile].m_name &&
        strcasecmp(m_virtualFiles[(int)m_lastFile].m_name, name) == 0)
        found = 1;
    if (!found) {
        m_lastFile = FindVirtualFile(name);
        if (m_lastFile == -1) {
            if (m_useFileSystem) {
                if (!LoadDiskFile(name, mode, &m_lastFile))
                    return 0;
            } else {
                return 0;
            }
        }
    }
    VirtualFile *f = &m_virtualFiles[(int)m_lastFile];
    if (f->m_data) {
        if (keepOpen)
            *file = m_lastFile;
        else
            *file = -1;
        *data = f->m_data;
    } else {
        OpenVirtualFile(m_lastFile);
        *file = m_lastFile;
        *data = 0;
    }
    return 1;
}


// ---- src/DataAccess/ClaimDeviceCache.cpp
struct FILE_HANDLE_STRUCT;

void FREAD_ASYNC_ABORT(FILE_HANDLE_STRUCT *file);



void DataAccess::ClaimDeviceCache(int device)
{
    if (m_cacheOwner != device) {
        if (m_cacheOwner != -1) {
            if (m_devices[m_cacheOwner].m_file)
                FREAD_ASYNC_ABORT(m_devices[m_cacheOwner].m_file);
        }
        m_cacheFirst = -1;
        m_cacheLast = -1;
        m_cacheOwner = device;
    }
}

