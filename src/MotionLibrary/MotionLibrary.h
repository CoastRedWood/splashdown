// MotionLibrary: the shared store of loaded Motions (no vtable). 0x1C bytes;
// the ctor zeroes +0x00..+0x18 and sets +0x14 to 1.
#ifndef MOTIONLIBRARY_MOTIONLIBRARY_H
#define MOTIONLIBRARY_MOTIONLIBRARY_H

class Character;
class Motion;

class MotionLibrary {
public:
    MotionLibrary();
    ~MotionLibrary();
    void AddMotion(Motion *motion);
    void ExportMotion(char *name, Motion *motion);
    Motion *LoadMotion(Character *character, char *name);
    int LoadMotionFromMOT(char *name, Motion *motion);
    int LoadMotionFromVUE(Character *character, char *name, Motion *motion);
    Motion *MotionOfName(char *name, int warn);
    void ReleaseMotion(Motion *motion);

    int m_numMotions;                   // +0x00
    int m_memoryUsed;                   // +0x04
    int m_x08;                          // +0x08
    int m_x0C;                          // +0x0C
    int m_keepMotions;                  // +0x10
    int m_exportEnabled;                // +0x14, 1 from the ctor
    Motion **m_motions;                 // +0x18
};

#endif
