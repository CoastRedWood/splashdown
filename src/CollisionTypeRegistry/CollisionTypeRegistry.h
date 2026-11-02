// CollisionTypeRegistry: hands out one bit per collision type name
// (LpCollisionTypeRegistry; every TU's static initializer registers its
// COLLISION_* types with GetType). Up to 32 names of 0x20 bytes.
#ifndef COLLISIONTYPEREGISTRY_COLLISIONTYPEREGISTRY_H
#define COLLISIONTYPEREGISTRY_COLLISIONTYPEREGISTRY_H

class CollisionTypeRegistry {
public:
    CollisionTypeRegistry();
    int GetType(char *name);

    int m_count;                        // +0x0
    char m_names[32][0x20];             // +0x4
};

extern CollisionTypeRegistry *LpCollisionTypeRegistry;

#endif
