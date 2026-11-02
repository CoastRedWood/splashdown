// CameraPreset: one follow-camera preset, 0x50 bytes (SimpleFollowCamera's
// m_presets array, malloc'd by LoadPresetsFromFile). Retail's mangled names
// call this type FollowCameraPreset (Initialize__18SimpleFollowCamera...
// P18FollowCameraPreset...); SimpleFollowCamera.h names it CameraPreset.
// Member names are the parameter-file keys ReadPresetInfoFromFile reads
// into each field.
#ifndef CAMERAPRESET_CAMERAPRESET_H
#define CAMERAPRESET_CAMERAPRESET_H

struct CameraPreset {
    int m_id;                           // +0x00 "ID"
    float m_angleY;                     // +0x04 "AngleY" (radians)
    float m_angleX;                     // +0x08 "AngleX" (radians)
    float m_followingDistance;          // +0x0C
    float m_fov;                        // +0x10
    float m_reverseAngleXOffset;        // +0x14
    float m_reverseFollowingOffset;     // +0x18
    float m_reverseLookAheadOffset;     // +0x1C
    float m_forwardReverseSwitchSpeed;  // +0x20
    int m_cycleThruThisPreset;          // +0x24 (NextPreset skips presets without it)
    int m_filterThisPreset;             // +0x28
    int m_dontRotate;                   // +0x2C
    float m_lookAheadDistance;          // +0x30
    int m_isLookBackCam;                // +0x34
    float m_cameraSwing;                // +0x38
    float m_lookSwing;                  // +0x3C
    float m_minFollowingDistance;       // +0x40
    float m_maxFollowingDistance;       // +0x44
    float m_minFOV;                     // +0x48
    float m_maxFOV;                     // +0x4C
};

#endif
