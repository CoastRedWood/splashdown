// VirtualDataDevice: one entry of DataAccess's device table
// (DataAccess::m_devices), 0x28 bytes. Names are the most common among
// DataAccess's methods (LoadDiskFile names +0x14..+0x20).
#ifndef VIRTUALDATADEVICE_VIRTUALDATADEVICE_H
#define VIRTUALDATADEVICE_VIRTUALDATADEVICE_H

struct FILE_HANDLE_STRUCT;

struct VirtualDataDevice {
    int m_id;                           // +0x00
    char *m_name;                       // +0x04
    int m_x08;                          // +0x08
    unsigned int m_flags;               // +0x0C
    FILE_HANDLE_STRUCT *m_file;         // +0x10
    int m_size;                         // +0x14
    int m_dataOffset;                   // +0x18
    int m_tableOffset;                  // +0x1C
    int m_tableSize;                    // +0x20
    unsigned char m_x24;                // +0x24
};

#endif
