// Template members the mwcc build got from another TU's instantiation
// (mwcc emitted every member of ContainerList<char *> where
// ContainerListMembers.h was included; clang only emits what a TU uses).
#include "soultree_assert.h"
#define CONTAINERLIST_MEMBERS_ALL
#include "ContainerList/ContainerList.h"
#include "ContainerList/ContainerListMembers.h"

template class ContainerList<char *>;
