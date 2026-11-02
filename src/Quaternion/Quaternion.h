// Quaternion: four floats laid out {w, x, y, z} (retail's quaternion helpers
// in src/global/: ComputeMatrix, ComputeQuaternion, FastInterpolateQuaternion,
// MakeClosest, ...). A plain struct: no constructor, so copies stay word
// copies.
#ifndef QUATERNION_QUATERNION_H
#define QUATERNION_QUATERNION_H

struct Quaternion {
    float w, x, y, z;
};

#endif
