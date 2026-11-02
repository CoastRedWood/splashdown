// Camera: the engine's view (PSXCamera derives from it). A GameObject plus
// a 4-byte searchable part at +0x30 (see below), then the matrices and view
// state. Names follow Camera's own methods (src/Camera/), otherwise the most
// common name among the files that declared Camera locally.
//
// Camera is `GameObject, SearchableObject`: the 4-byte SearchableObject
// base sits at +0x30 (retail converts Camera * to SearchableObject * by
// adding 0x30 in Tick and AddSearchPlacement, its ctor inlines
// SearchableObject's before storing the vptr, and Camera's RTTI lists both
// bases), so Camera's own data starts at +0x34.
//
// Camera's data ends at +0x248; PSXCamera's first known member is at
// +0x250 (src/PSXCamera, Camera/CameraVU0.h), so Camera is padded to 0x250.
#ifndef CAMERA_CAMERA_H
#define CAMERA_CAMERA_H

#include "GameObject/GameObject.h"
#include "SearchableObject/SearchableObject.h"
#include "d3dtypes.h"
#include "_RECT/_RECT.h"

class SearchNode;
class SearchPlacement;
struct FILE_HANDLE_STRUCT;


extern int RenderSearchedObjects;

inline float Max(float a, float b)
{
    float r;
    asm("max.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

inline float Min(float a, float b)
{
    float r;
    asm("min.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

class Camera : public GameObject, public SearchableObject {
public:
    Camera(int enabled);
    int SurfaceChanged(float aspect);
    int SetUpViewport();
    int SetUpViewportAndMatrices(int x, int y, int width, int height);
    void SetClipPlanes(float nearZ, float farZ);
    int ResetClipPlanes();
    void SetFOV(float fov);
    void AddSearchPlacement(SearchNode *node, unsigned int flags);
    int ResizeViewport(int x, int y, int width, int height);
    void GetViewportBoundsPhysical(_RECT *rect);
    void GetViewportBoundsLogical(_RECT *rect);
    void SetViewParameters(_D3DVECTOR *position, _D3DVECTOR *lookAt, _D3DVECTOR *up, float *x1D4, float *fov);
    void GetViewParameters(_D3DVECTOR *position, _D3DVECTOR *lookAt, _D3DVECTOR *up, float *x1D4, float *fov);

    // --- virtuals in retail slot order ---
    virtual ~Camera();
    virtual void Disable();
    virtual void Enable();
    virtual GameObject *Create();
    virtual int Tick(float dt);
    virtual int PrepareGeometry();
    virtual int RenderPre3D();
    virtual int Render3D();
    virtual int Restore();
    virtual int KeyDownHandler(ProcessNode *node, KeyInfo *key);
    virtual void DumpSnapShot() = 0;    // slot 27: empty in Camera's vtable, PSXCamera's
    virtual void UpdateMatrices();
    virtual void UpdateClipDistances(float nearZ, float farZ);
    virtual int LookAt(_D3DVECTOR target);
    virtual int SetTransformWorld(_D3DMATRIX *matrix);
    virtual int SetTransformView(_D3DMATRIX *matrix);
    virtual int SetTransformProjection(_D3DMATRIX *matrix);

    // +0x30: SearchableObject (m_x00, m_typeID, m_searchVisible/m_addToList/m_hidden bits)
    SearchPlacement *m_placement;       // +0x34
    GameObject *m_searchNode;           // +0x38, the placement's SearchNode (a GameObject)
    char pad3C[0x40 - 0x3C];
    _D3DMATRIX m_world;                 // +0x40
    _D3DMATRIX m_projection;            // +0x80
    _D3DMATRIX m_view;                  // +0xC0
    _D3DMATRIX m_viewProjection;        // +0x100
    _D3DMATRIX m_x140;                  // +0x140
    _D3DVECTOR m_position;              // +0x180
    _D3DVECTOR m_lookAt;                // +0x190
    _D3DVECTOR m_up;                    // +0x1A0
    _D3DVECTOR m_prevPosition;          // +0x1B0, last Tick's
    _D3DVECTOR m_prevLookAt;            // +0x1C0, last Tick's
    float m_fov;                        // +0x1D0
    float m_x1D4;                       // +0x1D4
    float m_focalLength;                // +0x1D8, in pixels
    float m_aspect;                     // +0x1DC
    _D3DVIEWPORT8 m_viewport;           // +0x1E0
    float m_pixelAspect;                // +0x1F8
    float m_nearClip;                   // +0x1FC
    float m_farClip;                    // +0x200
    int m_surfaceWidth;                 // +0x204
    int m_surfaceHeight;                // +0x208
    int m_viewportDirty;                // +0x20C
    int m_matricesDirty;                // +0x210
    int m_x214;                         // +0x214
    int m_x218;                         // +0x218
    float m_x21C;                       // +0x21C, 109.0 from the ctor
    float m_x220;                       // +0x220, 10.0 from the ctor
    FILE_HANDLE_STRUCT *m_logFile;      // +0x224, closed by the dtor
    int m_x228;                         // +0x228
    int m_x22C;                         // +0x22C
    int m_x230;                         // +0x230
    int m_x234;                         // +0x234
    int m_x238;                         // +0x238
    int m_x23C;                         // +0x23C
    float m_speed;                      // +0x240, distance moved last Tick
    float m_turn;                       // +0x244, angle turned last Tick
    char pad248[0x250 - 0x248];
};

#endif
