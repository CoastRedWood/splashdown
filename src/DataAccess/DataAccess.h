// DataAccess: the file layer (LpDataAccess). Virtual devices (resource
// archives or the disk), virtual files inside them, a block cache per
// device and a "stow" buffer. A BaseObject, 0x140 bytes (`new` in
// Game's ctor). Names come from the class's own methods.
#ifndef DATAACCESS_DATAACCESS_H
#define DATAACCESS_DATAACCESS_H

#include "GameObject/GameObject.h"

struct VirtualDataDevice;       // 0x28 bytes each
struct VirtualFile;             // 0x20 bytes each

// One device's block cache (0x18 bytes).
struct DeviceCache {
    unsigned char *m_data;              // +0x00
    char pad04[0x8 - 0x4];
    long m_file;                        // +0x08 (64-bit), -1 free, -2 unusable
    int m_x10;                          // +0x10
    char pad14[0x18 - 0x14];
};

class DataAccess : public BaseObject {
public:
    DataAccess();
    int Initialize(int numDevices, int numFiles, unsigned int cacheSize);
    void FreeSystemResources();
    int ActivateDevice(VirtualDataDevice *device, int arg);
    int DropDevice(char *name, int force);
    int DropDevice(VirtualDataDevice *device, int force);
    int DropAllDevices(int force);
    void FreeDevice(VirtualDataDevice *device);
    void ClaimDeviceCache(int device);
    void UpdateDeviceCache(VirtualDataDevice *device, int block);
    unsigned long ReadData(VirtualDataDevice *device, int cache, void *buffer, unsigned int size);
    int LoadResourceFile(char *name, int a, int b, unsigned int *c, void *d, unsigned int e, unsigned int *f, int g);
    int SaveResourceFile(char *fileName, int blockMarkers, int compress, int encrypt, unsigned int userDataType,
                         void *userData, unsigned int userDataSize, int untag);
    int AddFile(char *name, int device, int size, unsigned int x14, long *index, int grow);
    long FindVirtualFile(char *name);
    void OpenVirtualFile(long file);
    int LoadDiskFile(char *name, const char *mode, long *index);
    void FreeFile(long file);
    int FileExists(char *name);
    int GetDataOrFileHandle(char *name, const char *mode, long *file, void **data, int keepOpen);
    int GetStatus(long dataFile, long *modifyTime, long *size);
    long FOpen(char *name, const char *mode);
    int FClose(long file);
    unsigned int FRead(long file, void *buffer, unsigned int size, unsigned int count);
    unsigned int FRead(long file, void *buffer, unsigned int size);
    unsigned int FWrite(long file, void *data, unsigned int size, unsigned int count);
    char *FGets(long file, char *buffer, int size);
    int FSeek(long dataFile, long offset, int origin);
    long FTell(long dataFile);
    int FEof(long dataFile);
    int FFlush(long dataFile);
    int AttachObject(long dataFile, void *object);
    void DetachObject(void *object);
    void ResetStow();
    void TagForSave(int tag);

    virtual ~DataAccess();

    int m_nextDevice;                   // +0x10
    VirtualDataDevice *m_devices;       // +0x14
    int m_numDevices;                   // +0x18
    char pad1C[0x20 - 0x1C];
    int m_stowBuffer[4];                // +0x20
    int *m_stow;                        // +0x30
    int *m_stowEnd;                     // +0x34
    unsigned char *m_cache;             // +0x38, cache buffer
    int m_cacheOwner;                   // +0x3C
    unsigned int m_cacheSize;           // +0x40
    int m_cacheFirst;                   // +0x44
    int m_cacheLast;                    // +0x48
    unsigned short *m_blockSizes;       // +0x4C
    DeviceCache m_caches[8];            // +0x50
    int m_x110;                         // +0x110, current cache
    int m_freeFile;                     // +0x114
    VirtualFile *m_virtualFiles;        // +0x118
    int m_numVirtualFiles;              // +0x11C
    int m_saveTag;                      // +0x120
    char pad124[0x128 - 0x124];
    long m_lastFile;                    // +0x128 (64-bit)
    long m_lastVirtualFile;             // +0x130 (64-bit)
    int m_useFileSystem;                // +0x138
    int m_x13C;                         // +0x13C
};

#endif
