// TerrainQuality: one row of the terrain's graphic-quality table
// (Terrain::SetGraphicQuality indexes LpTerrainQuality by quality; row 9 is
// the detail-off LOD distance).
#ifndef TERRAINQUALITY_TERRAINQUALITY_H
#define TERRAINQUALITY_TERRAINQUALITY_H

struct TerrainQuality {
    int m_tolerance;                    // +0x0
    int m_lodDistance;                  // +0x4
    int m_8;                            // +0x8
    int m_renderDetail;                 // +0xC
};

extern TerrainQuality *LpTerrainQuality;
extern TerrainQuality TerrainQualityHardware[];

#endif
