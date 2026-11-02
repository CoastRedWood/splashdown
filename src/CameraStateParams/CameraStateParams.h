// CameraStateParams: the 0x60-byte state a camera behaviour hands to the next
// one (CameraBehavior::SwitchOff returns it, SwitchOn takes it by value).
// +0x40..+0x5C are the behaviour's filter pointers (CopyFilterData reads
// them as CameraFilter *m_filters[8]; most copies declare float *).
// It is 16-byte aligned (CinematographerCam::SwitchOn copies the by-value
// params with lq/sq) and its vectors are _D3DVECTORs (SwitchOn's copies;
// every user passes &params.position as a _D3DVECTOR *). Every includer
// matches either way on the alignment; the local copies in the
// SimpleFollow/Thrill/WreckCamera files are not moved over yet.
#ifndef CAMERASTATEPARAMS_CAMERASTATEPARAMS_H
#define CAMERASTATEPARAMS_CAMERASTATEPARAMS_H

#include "d3dtypes.h"

struct CameraStateParams {
    _D3DVECTOR position;            // +0x00
    _D3DVECTOR lookPoint;           // +0x10
    _D3DVECTOR up;                  // +0x20
    float f30, f34, f38, f3C;       // +0x30
    float *p40, *p44, *p48, *p4C;   // +0x40
    float *p50, *p54, *p58, *p5C;   // +0x50
} __attribute__((aligned(16)));

#endif
