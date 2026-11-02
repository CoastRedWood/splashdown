// InstanceMasterList: the named instance masters (LpInstanceMasterList), a
// DynamicArray of InstanceMasterType; Get looks one up by name.
#ifndef INSTANCEMASTERLIST_INSTANCEMASTERLIST_H
#define INSTANCEMASTERLIST_INSTANCEMASTERLIST_H

#include "DynamicArray/DynamicArray.h"

struct InstanceMasterType {
    char m_name[0x104];                 // +0x000
    void *m_master;                     // +0x104
};

class InstanceMasterList : public DynamicArray<InstanceMasterType> {
public:
    void *Get(char *name);
};

extern InstanceMasterList *LpInstanceMasterList;

#endif
