// PSXCamera.cpp  @ 0x0026EB00 (text 0x26EB00-0x270160)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp)
//
// Retail's PSXCamera.cpp as one translation unit: the first TU of its
// tu_map row. Its .rodata is PSXCamera's strings and RTTI (to 0x416B80);
// PSXControlInterface's RTTI follows with a label restart.
// __sinit_PSXCamera.cpp builds only the axis vectors. `make link` places
// its data as well as its code.


#include "d3dtypes.h"
#include "PSXCamera/PSXCamera.h"
#include "PSXVideoCard/PSXVideoCard.h"
#include "Game/Game.h"
#include "ControlInterface/ControlInterface.h"
#include "GsTextureTransfer/GsTextureTransfer.h"
#include "PSXTextureMapManager/PSXTextureMapManager.h"
#include "RenderTarget/RenderTarget.h"
#include "global/Vu0Inline.h"
#include "sinit/AxisVectors.h"

extern "C" void *memalign(unsigned int align, unsigned int size);
extern "C" void *malloc(unsigned int size);
extern "C" void free(void *p);
extern "C" void *memcpy(void *dst, const void *src, unsigned int n);
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" void FlushCache(int mode);
void PSXprintf(const char *fmt, ...);

extern PSXVideoCard *LpVideoCard;
extern PSXTextureMapManager *LpTextureMapManager;
extern Game *LpGame;

_D3DMATRIX IdentityMatrix();
_D3DMATRIX MatrixRotateY(float angle);
_D3DMATRIX ProjectionMatrix(float nearZ, float farZ, float fov, float aspect);
float FastAtan2(float y, float x);

// ---- src/PSXCamera/ctor.cpp

// The float locals and their declaration order (some after the member
// stores) are what give retail's register choice for the vptr store.
// Heap shim (not part of the retail file, image object in .mwccshim): a 0x30000 image
// (a new head block of mwcc's parser heap) with the "has call" bytes of the
// 1.0 and 448.0 constant nodes set, as the per-function file had them
// (epoch.sh offsets 0x790, 0x8D0; calibrated with r18's image.py).
struct MwccCtorImage {
    char pad0[0x785];
    char flag0;
    char pad1[0x13F];
    char flag1;
    char tail[0x2F73A];
};
#pragma section mwccshim begin
inline void MwccCtorImageShim() { const MwccCtorImage image = { {0}, 1, {0}, 1 }; }
#pragma section mwccshim end
PSXCamera::PSXCamera(int enabled) : Camera(enabled)
{
    float fov = 80.0f;
    float one = 1.0f;
    float height = 448.0f;
    float width = 640.0f;
    m_x338 = one;
    m_x33C = one;
    float farZ = 1000.0f;
    float center = 2048.0f;
    SetPerspectiveCamera(fov, one, farZ, one, (float)LpVideoCard->m_maxDepth, center, center, width, height);
}


// ---- src/PSXCamera/Create.cpp

GameObject *PSXCamera::Create()
{
    if (!Camera::Create())
        return 0;
    return this;
}


// ---- src/PSXCamera/SetPerspectiveCamera.cpp



// Sets the projection parameters and the viewport rectangle/matrix. Retail
// stores m_top/m_bottom twice.
void PSXCamera::SetPerspectiveCamera(float fov, float nearZ, float farZ, float minZ, float maxZ,
                                     float centerX, float centerY, float width, float height)
{
    m_left = centerX - 0.5f * width;
    m_right = centerX + 0.5f * width;
    float top = centerY - 0.5f * height;
    float bot = centerY + 0.5f * height;
    m_top = top;
    m_bottom = bot;
    m_aspect = m_pixelAspect = (m_bottom - m_top) / (m_right - m_left);
    m_fov = fov;
    m_nearClip = nearZ;
    m_farClip = farZ;
    m_minZ = minZ;
    m_maxZ = maxZ;
    m_top = top;
    m_bottom = bot;
    float halfWidth = 0.5f * (m_right - m_left);
    float halfHeight = 0.5f * (m_top - m_bottom);
    float left = m_left;
    float bottom = m_bottom;
    m_x140 = IdentityMatrix();
    m_x140._11 = halfWidth;
    m_x140._41 = halfWidth + left;
    m_x140._22 = halfHeight;
    m_x140._42 = halfHeight + bottom;
    m_x140._33 = minZ - maxZ;
    m_x140._43 = maxZ;
    m_x250 = IdentityMatrix();
    m_guardBandViewProjection = IdentityMatrix();
}


// ---- src/PSXCamera/Tick.cpp


// As CollisionObject/vu0col.h's (that header has its own _D3DVECTOR):
// out = v rotated by m's upper 3x3, w from v.
static inline void ApplyMatrixVector(_D3DVECTOR *out, _D3DMATRIX *m, _D3DVECTOR v)
{
    asm("lqc2 vf4, 0x0(%1); lqc2 vf5, 0x10(%1); lqc2 vf6, 0x20(%1); lqc2 vf8, 0x0(%2); vmulax.xyz ACC, vf4, vf8x; vmadday.xyz ACC, vf5, vf8y; vmaddz.xyz vf9, vf6, vf8z; vadd.xyzw vf9, vf9, vf0; sqc2 vf9, 0x0(%0)"
        : : "r"(out), "r"(m), "r"(&v) : "memory");
}

inline float RSqrt(float x)
{
    float r;
    asm("rsqrt.s $f8, %0, %1; swc1 $f8, 0(%2)" : : "f"(1.0f), "f"(x), "r"(&r) : "$f8", "memory");
    return r;
}

// Normalizes v in place (the zero vector stays zero).
inline void Normalize(_D3DVECTOR &v)
{
    float len2 = v.x * v.x + v.y * v.y + v.z * v.z;
    if (len2 == 0.0f) {
        v = ZeroVector;
    } else {
        float r = RSqrt(len2);
        v.x *= r;
        v.y *= r;
        v.z *= r;
    }
}

inline void operator-=(_D3DVECTOR &a, const _D3DVECTOR &b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
}


// Debug fly-through: with the debug keys on, the keys move and turn the camera.
int PSXCamera::Tick(float dt)
{
    if (LpGame->m_debugKeys) {
        float pi = 3.1415927f;
        float turn;
        float speed = 100.0f * dt;
        turn = pi * speed / 180.0f;
        if (LpGame->m_controls->IsKeyDown(9, (DeviceType)2, 0x3F, 0)) {
            if (LpGame->m_controls->IsKeyDown(14, (DeviceType)2, 0x3F, 0))
                m_position -= speed * m_lookAt;
            if (LpGame->m_controls->IsKeyDown(12, (DeviceType)2, 0x3F, 0))
                m_position += speed * m_lookAt;
            if (LpGame->m_controls->IsKeyDown(15, (DeviceType)2, 0x3F, 0)) {
                _D3DVECTOR look;
                _D3DMATRIX rotation;
                float angle = 0.0f;
                angle += turn;
                rotation = MatrixRotateY(angle);
                ApplyMatrixVector(&look, &rotation, m_lookAt);
                m_lookAt = look;
                Normalize(m_lookAt);
            }
            if (LpGame->m_controls->IsKeyDown(13, (DeviceType)2, 0x3F, 0)) {
                _D3DVECTOR look;
                _D3DMATRIX rotation;
                float angle = 0.0f;
                angle -= turn;
                rotation = MatrixRotateY(angle);
                ApplyMatrixVector(&look, &rotation, m_lookAt);
                m_lookAt = look;
                Normalize(m_lookAt);
            }
            if (LpGame->m_controls->IsKeyDown(3, (DeviceType)2, 0x3F, 0)) {
                _D3DVECTOR side;
                _D3DMATRIX rotation;
                rotation = MatrixRotateY(1.5707961f);
                ApplyMatrixVector(&side, &rotation, m_lookAt);
                m_position += speed * side;
            }
            if (LpGame->m_controls->IsKeyDown(1, (DeviceType)2, 0x3F, 0)) {
                _D3DVECTOR side;
                _D3DMATRIX rotation;
                rotation = MatrixRotateY(1.5707961f);
                ApplyMatrixVector(&side, &rotation, m_lookAt);
                m_position -= speed * side;
            }
            if (LpGame->m_controls->IsKeyDown(0, (DeviceType)2, 0x3F, 0))
                m_position.y -= speed;
            if (LpGame->m_controls->IsKeyDown(2, (DeviceType)2, 0x3F, 0))
                m_position.y += speed;
        }
        SetViewParameters(&m_position, &m_lookAt, 0, 0, 0);
    }
    return Camera::Tick(dt);
}


// ---- src/PSXCamera/DumpSnapShot.cpp
// Heap shim (not part of the retail file, image object in .mwccshim). The optimizer
// pragma below makes mwcc generate the pending (deferred) functions here, so
// Tick's code is generated in this heap state rather than at the end of the
// file as in its per-function file (6 words: Normalize's m_lookAt loads
// swap). A zeroed image restores it.
struct MwccFlushImage {
    char tail[0x40000];
};
#pragma section mwccshim begin
inline void MwccFlushImageShim() { const MwccFlushImage image = { {0} }; }
#pragma section mwccshim end

// Retail compiled this function with the global optimizer off (unrotated loops).
#pragma global_optimizer off
struct FILE_HANDLE_STRUCT;
FILE_HANDLE_STRUCT *FOPEN(char *name, const char *mode);
int FCLOSE(FILE_HANDLE_STRUCT *f);

struct RGBQuad;
void ExportImage32(RGBQuad *pixels, int width, int height, int a, char *name, unsigned char bits);

// Reads both (interlaced) fields of the display buffer back from GS memory
// and saves them as the next free imageN.tga.
void PSXCamera::DumpSnapShot()
{

    int offset2;
    unsigned char *dst;
    int base;
    int pixels;
    int fieldSize;
    unsigned char *even;
    unsigned char *odd;
    int width;
    RGBQuad *image;
    int height;
    int i;
    int offset;
    int fieldHeight;
    int j;
    int rowSize;
    int rowStride;

    char name[0x100];

    LpRenderTarget->TrySendRenderQueue(1);
    LpRenderTarget->WaitForRenderingToComplete();
    static int currentShot = 0;
    PSXprintf("DumpSnapShot\n");
    width = LpVideoCard->m_width;
    height = LpVideoCard->m_height;
    base = LpVideoCard->GetCurrentDisplayBufferAddress() << 5;
    pixels = width * height;
    fieldSize = pixels / 2 * 4;
    even = (unsigned char *)memalign(16, fieldSize);
    odd = (unsigned char *)memalign(16, fieldSize);
    dst = (unsigned char *)(image = (RGBQuad *)malloc(pixels * 4));
    LpTextureMapManager->m_gsTransfer->GetTextureFromVideoMemory(even, base, m_surfaceWidth, width, fieldHeight = height / 2, 0, 0, 0, 4);
    FlushCache(0);
    LpRenderTarget->WaitForRenderingToComplete();
    LpTextureMapManager->m_gsTransfer->GetTextureFromVideoMemory(odd, base + 0x8C0, m_surfaceWidth, width, fieldHeight, 0, 0, 0, 4);
    FlushCache(0);
    LpRenderTarget->WaitForRenderingToComplete();

    i = 0;
    offset = 0;
    rowSize = width * 4;
    rowStride = width / 4 * 16;
    for (; i < fieldHeight; i++) {
        memcpy(dst, even + offset / 4 * 16, rowSize);
        dst += rowStride;
        offset += width;
    }
    for (j = 0, offset2 = 0; j < fieldHeight; j++) {
        memcpy(dst, odd + offset2 / 4 * 16, rowSize);
        dst += rowStride;
        offset2 += width;
    }

    sprintf(name, "image%d.tga", currentShot);
    FILE_HANDLE_STRUCT *f;
    if ((f = FOPEN(name, "r"))) {
        do {
            FCLOSE(f);
            currentShot++;
            sprintf(name, "image%d.tga", currentShot);
        } while ((f = FOPEN(name, "r")));
    }
    ExportImage32(image, width, height, 0, name, 0x20);
    free(image);
    free(even);
    free(odd);
}
#pragma global_optimizer reset


// ---- src/PSXCamera/UpdateMatrices.cpp


// Camera's matrices, then the screen scale in m_x140 and the guard-band
// projection (wider FOV) and its products.
// Heap shim (not part of the retail file): has-call bytes of the 0.5 and 1.0
// nodes (epoch.sh 0x648, 0x2B88), as UpdateMatrices.cpp had them; image.py
// calibration start 0x1E8 (3 words off without).
struct MwccMatricesImage {
    char pad0[0x465];
    char flag0;
    char pad1[0x253F];
    char flag1;
    char tail[0x3565A];
};
#pragma section mwccshim begin
inline void MwccMatricesImageShim() { const MwccMatricesImage image = { {0}, 1, {0}, 1 }; }
#pragma section mwccshim end
void PSXCamera::UpdateMatrices()
{
    Camera::UpdateMatrices();
    m_x140._11 = 0.5f * (m_right - m_left);
    m_x140._22 = 0.5f * (m_top - m_bottom);
    m_x140._11 = m_x140._11 * m_x338;
    m_x140._22 = m_x140._22 * m_x33C;
    CalculateGuardBandFOV();
    Vu0MulMatrix(&m_x250, &m_x140, &m_viewProjection);
    if (!(m_viewport.Width < m_viewport.Height))
        m_guardBandProjection = ProjectionMatrix(m_nearClip, m_farClip, 0.0174532905f * m_guardBandFOV, 1.0f / m_pixelAspect);
    else
        m_guardBandProjection = ProjectionMatrix(m_nearClip, m_farClip, 0.0174532905f * m_guardBandFOV, m_pixelAspect);
    Vu0MulMatrix(&m_guardBandViewProjection, &m_guardBandProjection, &m_view);
}


// ---- src/PSXCamera/UpdateClipDistances.cpp


// Also rebuild the guard-band projection (wider FOV) and its products.
// Heap shim (not part of the retail file): has-call bytes of the 1.0 and
// second 0.0174533 nodes (epoch.sh 0x1850, 0x2358), as the per-function file
// (on Camera/CameraVU0.h) had them; 3 words off without. CameraVU0.h's last
// user is gone with this fold.
struct MwccClipImage {
    char pad0[0x166D];
    char flag0;
    char pad1[0xB07];
    char flag1;
    char tail[0x39E8A];
};
#pragma section mwccshim begin
inline void MwccClipImageShim() { const MwccClipImage image = { {0}, 1, {0}, 1 }; }
#pragma section mwccshim end
void PSXCamera::UpdateClipDistances(float nearZ, float farZ)
{
    Camera::UpdateClipDistances(nearZ, farZ);
    Vu0MulMatrix(&m_x250, &m_x140, &m_viewProjection);
    if (!(m_viewport.Width < m_viewport.Height))
        m_guardBandProjection = ProjectionMatrix(m_nearClip, m_farClip, 0.0174532905f * m_guardBandFOV, 1.0f / m_pixelAspect);
    else
        m_guardBandProjection = ProjectionMatrix(m_nearClip, m_farClip, 0.0174532905f * m_guardBandFOV, m_pixelAspect);
    Vu0MulMatrix(&m_guardBandViewProjection, &m_guardBandProjection, &m_view);
}


// ---- src/PSXCamera/CalculateGuardBandFOV.cpp
// Retail converts float->int inline (cvt.w.s), never via fptosi: needs fast_fptosi.
#pragma fast_fptosi on



void PSXCamera::CalculateGuardBandFOV()
{
    m_guardBandFOV = (float)((int)(FastAtan2(2048.0f, m_focalLength) * 57.2957802f) * 2);
}


// ---- src/sinit/PSXCamera.cpp
// __sinit_PSXCamera.cpp builds only the axis vectors (sinit/AxisVectors.h, included above).
