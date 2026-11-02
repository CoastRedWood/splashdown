// JetskiUserOptions: a BaseObject (JetskiGame::m_userOptions, `new`ed by
// JetskiGame::CreateScene). 0x18 bytes; the ctor sets +0x10 to 1.
#ifndef JETSKIUSEROPTIONS_JETSKIUSEROPTIONS_H
#define JETSKIUSEROPTIONS_JETSKIUSEROPTIONS_H

#include "GameObject/GameObject.h"

class JetskiUserOptions : public BaseObject {
public:
    JetskiUserOptions();

    // --- virtuals in retail slot order ---
    virtual ~JetskiUserOptions();

    int m_x10;                          // +0x10, 1 from the ctor
    char pad14[0x18 - 0x14];
};

#endif
