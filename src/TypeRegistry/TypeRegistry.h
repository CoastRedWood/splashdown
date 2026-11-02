// TypeRegistry: maps type names to small IDs (GetTypeID registers a name
// the first time it sees it); 8 bytes.
#ifndef TYPEREGISTRY_TYPEREGISTRY_H
#define TYPEREGISTRY_TYPEREGISTRY_H

// One registered name, 0x80 bytes.
struct TypeName {
    char m_name[0x80];
};

class TypeRegistry {
public:
    TypeRegistry();
    ~TypeRegistry();
    unsigned char GetTypeID(char *name);

    int m_numTypes;                     // +0x0
    TypeName *m_types;                  // +0x4
};

#endif
