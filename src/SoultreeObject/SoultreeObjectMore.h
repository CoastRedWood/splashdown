// Copy of SoultreeObject/SoultreeObject.h plus the methods its newer matches
// declare (ResetMatrices, Translate overloads).
#ifndef SOULTREEOBJECT_SOULTREEOBJECTMORE_H
#define SOULTREEOBJECT_SOULTREEOBJECTMORE_H

#include "GameObject/GameObject.h"

extern "C" int strcasecmp(const char *a, const char *b);

#include "d3dtypes.h"

struct Quaternion;

_D3DMATRIX IdentityMatrix();
class SearchableObject;

// SoultreeObject's first base: 8 bytes in front of its GameObject subobject.
#include "SearchableObject/SearchableObject.h"

#include "SearchPlacement/SearchPlacement.h"

#include "SoultreeObject/SoultreeObject.h"

#endif
