// LightEmitter: a light in the scene (LightManager keeps the list). 0xE0
// bytes (`new LightEmitter`). Names follow LightEmitter's own methods
// (src/LightEmitter/), otherwise the most common name among the local copies.
//
// Retail builds it as `GameObject, SearchableObject`: the 4-byte
// SearchableObject sits at +0x30 (the ctor inlines its ctor before the vptr
// store), m_type at +0x34, and the spatial search returns lights as
// SearchableObject * (LightManager::GetThreeLights).
//
// +0x38..+0xA0 is a Direct3D D3DLIGHT8 (Type, Diffuse, Specular, Ambient,
// Position, Direction, Range, Falloff, Attenuation0-2, Theta, Phi), kept as
// the flat members LightEmitter's own methods use (Create keeps an axis in
// the attenuation slots).
#ifndef LIGHTEMITTER_LIGHTEMITTER_H
#define LIGHTEMITTER_LIGHTEMITTER_H

#include "GameObject/GameObject.h"
#include "SearchableObject/SearchableObject.h"
#include "d3dtypes.h"
#include "LightEmitter/_D3DCOLORVALUE.h"

class LensFlare;
class LightFlare;
class SearchNode;
class SearchPlacement;
struct LightFlicker;
#ifndef LIGHTEMITTERTYPE_DEFINED
#define LIGHTEMITTERTYPE_DEFINED
enum LightEmitterType {};      // also in LightEmitter.h / LightManager.h
#endif

class LightEmitter : public GameObject, public SearchableObject {
public:
    LightEmitter(int enabled);
    LightEmitter *Create(LightEmitterType type, _D3DCOLORVALUE color, _D3DVECTOR *position, _D3DVECTOR *direction,
                         _D3DVECTOR *axis, float range, int a, int b, char *name, int c);
    float CalcLuminance();
    unsigned int GetColor();
    void SetColor(_D3DCOLORVALUE color);
    void SetPosition(_D3DVECTOR &position);
    void SetDirection(_D3DVECTOR &direction);
    void SetRange(float range);
    void AddSearchPlacement(SearchNode *node, unsigned int flags);
    void RemoveFromSearch();
    void UpdateSearchPlacement();

    // --- virtuals in retail slot order ---
    virtual ~LightEmitter();
    virtual int Tick(float dt);

    int m_type;                         // +0x34, a LightEmitterType
    int m_mode;                         // +0x38, D3DLIGHT8 Type (3 from the ctor)
    _D3DCOLORVALUE m_diffuse;           // +0x3C
    _D3DCOLORVALUE m_specular;          // +0x4C
    _D3DCOLORVALUE m_ambient;           // +0x5C
    float m_pos[3];                     // +0x6C, D3DLIGHT8 Position (SetPosition)
    float m_dir[3];                     // +0x78, D3DLIGHT8 Direction (SetDirection)
    float m_range;                      // +0x84
    float m_falloff;                    // +0x88, D3DLIGHT8 Falloff (the search radius)
    float m_axis[3];                    // +0x8C (Create, 1, 0, 0 from the ctor), D3DLIGHT8 Attenuation0-2
    float m_halfAngle;                  // +0x98, Theta
    float m_angle;                      // +0x9C, Phi
    _D3DVECTOR m_position;              // +0xA0
    _D3DVECTOR m_direction;             // +0xB0
    LensFlare *m_lensFlare;             // +0xC0
    LightFlare *m_lightFlare;           // +0xC4
    int m_xC8;                          // +0xC8, -1 from the ctor
    float m_luminance;                  // +0xCC (CalcLuminance), -1 from the ctor
    SearchPlacement *m_placement;       // +0xD0
    SearchNode *m_searchNode;           // +0xD4
    LightFlicker *m_flicker;            // +0xD8 (Tick; deleted by the dtor)
};

#endif
