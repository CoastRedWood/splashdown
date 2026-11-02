// AnimTextureMap: an animated texture, one texture map per frame, named
// after its resource (0x130 bytes).
#ifndef ANIMTEXTUREMAP_H
#define ANIMTEXTUREMAP_H

#include "TextureMap/TextureMap.h"

class AnimTextureMap : public BaseTextureMap {
public:
    AnimTextureMap();
    AnimTextureMap *Create(char *name, int a, int c, unsigned int d);
    void ReleaseList();

    // --- virtuals in retail slot order ---
    virtual ~AnimTextureMap();
    virtual void *GetData(int index);   // the frame, its m_usage raised to ours
    virtual void GetReportData(TextureReportData *report);

    int m_numFrames;                    // +0x20
    BaseTextureMap **m_frames;          // +0x24 (new[]'d; each released by ReleaseList)
    char m_name[0x104];                 // +0x28 (also the DataAccess device name; retail news 0x130)
};

#endif
