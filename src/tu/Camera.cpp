// Camera.cpp  @ 0x00213C10 (text 0x213C10-0x2155B0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's Camera.cpp as one translation unit: its functions in retail
// order, then its static initializer. `make link` places the object's data
// as well as its code (tools/link_c.py). Written on Camera/Camera.h, whose
// SearchableObject base at +0x30 the ctor (its inlined ctor before the vptr
// store) and AddSearchPlacement/Tick (the this+0x30 conversion) need; the
// old Camera/CameraVU0.h view is gone.



#include "d3dtypes.h"
#include "GameObject/GameObject.h"
#include "Camera/Camera.h"
#include "RenderTarget/RenderTarget.h"
#include "SearchPlacement/SearchPlacement.h"
#include "PSXSurfaceShaderList/PSXSurfaceShaderList.h"
#include "Game/Game.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "_RECT/_RECT.h"
#include "global/Vu0Inline.h"
#include "global/Vu0More.h"
#include "soultree_assert.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"

// retail never calls fptosi: float->int is an inline cvt.w.s
#pragma fast_fptosi on

extern "C" void *memset(void *dst, int c, unsigned int n);
struct FILE_HANDLE_STRUCT;
void FCLOSE(FILE_HANDLE_STRUCT *file);
enum DeviceType { DEVICE_KEYBOARD };
int KEYDOWN_HANDLER_TEST(unsigned int key, DeviceType device, ProcessNode *node, unsigned int mods);

_D3DMATRIX IdentityMatrix();
_D3DMATRIX CreateViewMatrix(_D3DVECTOR position, _D3DVECTOR lookAt, _D3DVECTOR up, float roll);
_D3DMATRIX ProjectionMatrix(float nearZ, float farZ, float fov, float aspect);
float FastTan(float);
float FastACos(float);

extern RenderTarget *LpRenderTarget;
extern PSXVideoCard *LpVideoCard;
extern PSXSurfaceShaderList *LpSurfaceShaderList;
extern Game *LpGame;
extern int TimeGPU;
int RenderSearchedObjects = 1;
int Vsync30Enable = 1;

// ---- src/Camera/ctor.cpp

Camera::Camera(int enabled) : GameObject(enabled)
{
    m_x21C = 109.0f;
    m_x220 = 10.0f;
    m_fov = 77.0f;
    m_logFile = 0;
    m_x228 = 0;
    m_x22C = 0;
    m_nearClip = 1.0f;
    m_farClip = 100000.0f;
    m_viewportDirty = 0;
    m_matricesDirty = 0;
    memset(&m_viewport, 0, sizeof(m_viewport));
    m_world = IdentityMatrix();
    m_lookAt = _D3DVECTOR(0.0f, 0.0f, 1.0f);
    m_up = _D3DVECTOR(0.0f, 1.0f, 0.0f);
    m_position = _D3DVECTOR(0.0f, 0.0f, 0.0f);
    m_position = _D3DVECTOR(0.0f, 50.0f, 35.0f);
    m_x1D4 = 0;
    m_x214 = 1;
    m_x218 = 1;
    m_x230 = 0;
    m_x23C = 0;
    m_x234 = 0;
    m_x238 = 0;
    m_speed = 0;
    m_turn = 0;
    m_prevLookAt = m_lookAt;
    m_prevPosition = m_position;
    m_x140 = IdentityMatrix();
    m_placement = 0;
    m_searchNode = 0;
}


// ---- src/Camera/Create.cpp

GameObject *Camera::Create()
{
    GameObject::Create();
    if (!SurfaceChanged(1.33333337f)) {
        Release();
        return 0;
    }
    UpdateMatrices();
    if (m_enabled)
        LpRenderTarget->SetCamera(this);
    return this;
}


// ---- src/Camera/AddSearchPlacement.cpp
#pragma alignlabel off

void Camera::AddSearchPlacement(SearchNode *node, unsigned int flags)
{
    m_placement = new SearchPlacement;
#line 99 "Camera.cpp"
    ASSERT(m_placement, "Camera::AddSearchPlacement: LpSearchPlacement==NULL");
    m_searchNode = (GameObject *)node;
    m_placement->PerformInitialPlacement(this, node, flags, &m_position, &ZeroVector, 0.0f);
#line 104 "Camera.cpp"
    ASSERT(m_placement->FirstPlacementItem.LpSearchNode != 0, "Camera::AddSearchPlacement: FirstPlacementItem.LpSearchNode == 0");
}
#pragma alignlabel reset


// ---- src/Camera/SurfaceChanged.cpp

int Camera::SurfaceChanged(float aspect)
{
    m_surfaceWidth = LpRenderTarget->m_width;
    m_surfaceHeight = LpRenderTarget->m_height;
    m_aspect = aspect;
    SetUpViewportAndMatrices(0, 0, m_surfaceWidth, m_surfaceHeight);
    return 1;
}


// ---- src/Camera/SetUpViewportAndMatrices.cpp
// Retail loads LpVideoCard->m_width before m_height: the card size is read
// into locals, which stay in that order only with copy propagation and
// dead-assignment removal off (found by a script).
#pragma opt_propagation off
#pragma opt_dead_assignments off

int Camera::SetUpViewportAndMatrices(int x, int y, int width, int height)
{
    m_viewport.X = x;
    m_viewport.Y = y;
    m_viewport.Width = width;
    m_viewport.Height = height;
    m_viewport.MinZ = 0.0f;
    m_viewport.MaxZ = 1.0f;
    int vw = LpVideoCard->m_width;
    int vh = LpVideoCard->m_height;
    m_pixelAspect = ((float)height / (float)width) / (((float)vh / (float)vw) * m_aspect);
    SetUpViewport();
    UpdateMatrices();
    return 1;
}
#pragma opt_propagation reset
#pragma opt_dead_assignments reset


// ---- src/Camera/SetUpViewport.cpp

int Camera::SetUpViewport()
{
    m_viewportDirty = m_matricesDirty = LpRenderTarget->m_frameCount + 1;
    return LpRenderTarget->SetViewport(&m_viewport) != 0;
}


// ---- src/Camera/RenderPre3D.cpp

int Camera::RenderPre3D()
{
    if (m_viewportDirty) {
        _RECT rect;
        rect.left = m_viewport.X;
        rect.top = m_viewport.Y;
        rect.right = m_viewport.X + m_viewport.Width;
        rect.bottom = m_viewport.Y + m_viewport.Height;
        LpRenderTarget->Clear(&rect, 0);
    }
    ResetClipPlanes();
    return GameObject::RenderPre3D();
}


// ---- src/Camera/Tick.cpp

static inline float Sqrt(float x)
{
    float r;
    asm("sqrt.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

static inline float Length(const _D3DVECTOR &v)
{
    return Sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Track how far the camera moved and turned since the last tick, then update
// the matrices and the search placement.
int Camera::Tick(float dt)
{
    m_speed = Length(_D3DVECTOR(m_position.x - m_prevPosition.x, m_position.y - m_prevPosition.y, m_position.z - m_prevPosition.z));
    m_prevPosition = m_position;
    m_turn = FastACos(Vu0DotProduct(&m_lookAt, &m_prevLookAt));
    if (m_turn < 0.0f)
        m_turn = -m_turn;
    m_prevLookAt = m_lookAt;
    if (m_paused)
        return 1;
    if (m_matricesDirty)
        m_matricesDirty--;
    if (m_viewportDirty)
        m_viewportDirty--;
    UpdateMatrices();
    if (m_placement) {
        m_placement->UpdatePlacement(this, &m_position, &ZeroVector, 0.0f);
        m_searchNode = (GameObject *)m_placement->FirstPlacementItem.LpSearchNode;
    }
    return GameObject::Tick(dt);
}


// ---- src/Camera/ResetClipPlanes.cpp

int Camera::ResetClipPlanes()
{
    if (m_farClip != 100000.0f || m_nearClip != 1.0f)
        SetClipPlanes(1.0f, 100000.0f);
    return 1;
}


// ---- src/Camera/PrepareGeometry.cpp

int Camera::PrepareGeometry()
{
    if (RenderSearchedObjects) {
        if (m_searchNode)
            m_searchNode->PrepareGeometry();
    }
    return GameObject::PrepareGeometry();
}


// ---- src/Camera/Render3D.cpp

int Camera::Render3D()
{
    if (!RenderSearchedObjects)
        return 1;
    if (TimeGPU) {
        LpRenderTarget->TrySendRenderQueue(1);
        LpRenderTarget->WaitForRenderingToComplete();
    }
    // Profiler IDs (unused).
    static int CPUTimeID = -1;
    static int timerID1 = -1;
    if (m_searchNode)
        m_searchNode->Render3D();
    static int timerID2 = -1;
    if (LpSurfaceShaderList)
        LpSurfaceShaderList->Render();
    if (TimeGPU) {
        static int GPUTimeID = -1;
        LpRenderTarget->TrySendRenderQueue(1);
        LpRenderTarget->WaitForRenderingToComplete();
    } else {
        LpRenderTarget->TrySendRenderQueue(0);
    }
    return GameObject::Render3D();
}


// ---- src/Camera/SetFOV.cpp

void Camera::SetFOV(float fov)
{
    if (fov != 0.0f)
        m_fov = fov;
    m_matricesDirty = LpRenderTarget->m_frameCount + 1;
}


// ---- src/Camera/SetClipPlanes.cpp

void Camera::SetClipPlanes(float nearZ, float farZ)
{
    m_nearClip = Max(1.0f, nearZ);
    m_farClip = Min(100000.0f, farZ);
    UpdateClipDistances(m_nearClip, m_farClip);
}


// ---- src/Camera/SetViewParameters.cpp

void Camera::SetViewParameters(_D3DVECTOR *position, _D3DVECTOR *lookAt, _D3DVECTOR *up, float *x1D4, float *fov)
{
    if (position)
        m_position = *position;
    if (lookAt)
        Vu0Normalize(&m_lookAt, lookAt);
    if (up)
        Vu0Normalize(&m_up, up);
    if (x1D4)
        m_x1D4 = *x1D4;
    if (fov)
        SetFOV(*fov);
}


// ---- src/Camera/GetViewParameters.cpp

void Camera::GetViewParameters(_D3DVECTOR *position, _D3DVECTOR *lookAt, _D3DVECTOR *up, float *x1D4, float *fov)
{
    if (position)
        *position = m_position;
    if (lookAt)
        *lookAt = m_lookAt;
    if (up)
        *up = m_up;
    if (x1D4)
        *x1D4 = m_x1D4;
    if (fov)
        *fov = m_fov;
}


// ---- src/Camera/LookAt.cpp

// Point the camera at target, rebuilding the up vector (world y unless the
// view is vertical). Returns 0 if target is the camera position.
int Camera::LookAt(_D3DVECTOR target)
{
    if (target == m_position)
        return 0;
    _D3DVECTOR side;
    m_up.x = 0.0f;
    m_up.y = 1.0f;
    m_up.z = 0.0f;
    Vu0Normalize(&m_lookAt, target - m_position);
    if (target.x != m_position.x || target.z != m_position.z) {
        Vu0CrossProduct(&side, &m_up, &m_lookAt);
        Vu0Normalize(&side, &side);
    } else {
        side = _D3DVECTOR(1.0f, 0.0f, 0.0f);
    }
    Vu0CrossProduct(&m_up, &m_lookAt, &side);
    m_matricesDirty = LpRenderTarget->m_frameCount + 1;
    return 1;
}


// ---- src/Camera/SetTransformWorld.cpp

int Camera::SetTransformWorld(_D3DMATRIX *matrix)
{
    m_world = *matrix;
    return 1;
}


// ---- src/Camera/SetTransformView.cpp

int Camera::SetTransformView(_D3DMATRIX *matrix)
{
    if (matrix)
        m_view = *matrix;
    return 1;
}


// ---- src/Camera/SetTransformProjection.cpp

int Camera::SetTransformProjection(_D3DMATRIX *matrix)
{
    if (matrix)
        m_projection = *matrix;
    return 1;
}


// ---- src/Camera/UpdateMatrices.cpp

// Rebuild the view, projection and view-projection matrices and the focal
// length in pixels. (Retail's degrees-to-radians constant is 0x3C8EFA34, one
// ulp under pi/180.)
void Camera::UpdateMatrices()
{
    m_view = CreateViewMatrix(m_position, m_lookAt, m_up, m_x1D4);
    m_projection = ProjectionMatrix(m_nearClip, m_farClip, 0.0174532905f * m_fov, 1.0f / m_pixelAspect);
    Vu0MulMatrix(&m_viewProjection, &m_projection, &m_view);
    m_focalLength = (0.5f * m_viewport.Width) / FastTan(0.0174532905f * (0.5f * m_fov));
}


// ---- src/Camera/UpdateClipDistances.cpp

// Rebuild the projection's depth terms for new near/far planes and
// recompute the combined view-projection matrix.
void Camera::UpdateClipDistances(float nearZ, float farZ)
{
    m_nearClip = nearZ;
    m_farClip = farZ;
    float q = 1.0f / (1.0f - nearZ / farZ);
    m_projection._33 = q;
    m_projection._43 = -q * nearZ;
    Vu0MulMatrix(&m_viewProjection, &m_projection, &m_view);
}


// ---- src/Camera/dtor.cpp

Camera::~Camera()
{
    if (m_placement)
        delete m_placement;
    if (m_logFile)
        FCLOSE(m_logFile);
    Camera *cameras[4];
    LpGame->GetCameras(cameras);
    for (int i = 0; i < 4; i++) {
        if (cameras[i] == this)
            cameras[i] = 0;
    }
    LpGame->SetCameras(cameras[0], cameras[1], cameras[2], cameras[3]);
    if (LpRenderTarget && LpRenderTarget->m_camera == this)
        LpRenderTarget->SetCamera(0);
}


// ---- src/Camera/Enable.cpp

void Camera::Enable()
{
    GameObject::Enable();
    LpRenderTarget->SetCamera(this);
    m_matricesDirty = LpRenderTarget->m_frameCount + 1;
}


// ---- src/Camera/Disable.cpp

void Camera::Disable()
{
    GameObject::Disable();
}


// ---- src/Camera/Restore.cpp

int Camera::Restore()
{
    GameObject::Restore();
    m_matricesDirty = LpRenderTarget->m_frameCount + 1;
    return 1;
}


// ---- src/Camera/KeyDownHandler.cpp

// Note: returns 0 when the base class handled the key (as retail does).
int Camera::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    if (!GameObject::KeyDownHandler(node, key)) {
        if (KEYDOWN_HANDLER_TEST(0x23, DEVICE_KEYBOARD, node, 0x80)) {
            RenderSearchedObjects = 1 - RenderSearchedObjects;
            return 1;
        }
        if (KEYDOWN_HANDLER_TEST(0x24, DEVICE_KEYBOARD, node, 0x80)) {
            TimeGPU = 1 - TimeGPU;
            return 1;
        }
        if (KEYDOWN_HANDLER_TEST(0x19, DEVICE_KEYBOARD, node, 0x80)) {
            Vsync30Enable = 1 - Vsync30Enable;
            if (Vsync30Enable)
                LpVideoCard->EnableVSyncTo30Hz(0);
            else
                LpVideoCard->DisableVSync(0);
            return 1;
        }
    }
    return 0;
}


// ---- src/Camera/ResizeViewport.cpp

int Camera::ResizeViewport(int x, int y, int width, int height)
{
    if (m_surfaceWidth < width)
        width = m_surfaceWidth;
    if (m_surfaceHeight < height)
        height = m_surfaceHeight;
    SetUpViewportAndMatrices(x, y, width, height);
    return 1;
}


// ---- src/Camera/GetViewportBoundsPhysical.cpp

void Camera::GetViewportBoundsPhysical(_RECT *rect)
{
    rect->left = m_viewport.X;
    rect->right = m_viewport.X + m_viewport.Width;
    rect->top = m_viewport.Y;
    rect->bottom = m_viewport.Y + m_viewport.Height;
}


// ---- src/Camera/GetViewportBoundsLogical.cpp

void Camera::GetViewportBoundsLogical(_RECT *rect)
{
    rect->left = 0;
    rect->right = (int)(float)m_viewport.Width;
    rect->top = 0;
    rect->bottom = (int)(float)m_viewport.Height;
}


// ---- src/sinit/Camera.cpp
// Static initializer of Camera.cpp: only the axis vectors from the shared header.
