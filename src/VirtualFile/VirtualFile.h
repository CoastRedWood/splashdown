// VirtualFile: one entry of DataAccess's virtual file table
// (DataAccess::m_virtualFiles), 0x20 bytes. Names are the most common
// among DataAccess's methods.
#ifndef VIRTUALFILE_VIRTUALFILE_H
#define VIRTUALFILE_VIRTUALFILE_H

struct VirtualFile {
    int m_device;                       // +0x00
    int m_cache;                        // +0x04
    char *m_name;                       // +0x08 (FreeFile/FClose/OpenVirtualFile: void *m_data)
    int m_offset;                       // +0x0C, within the device
    int m_position;                     // +0x10
    int m_length;                       // +0x14 (FEof and GetStatus compare it unsigned)
    unsigned int m_flags;               // +0x18, bit 0: free
    void *m_data;                       // +0x1C, the file's in-memory data (AttachObject; GetDataOrFileHandle returns it)
};

#endif
