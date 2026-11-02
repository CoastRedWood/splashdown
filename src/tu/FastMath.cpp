// FastMath.cpp  @ 0x0030D860 (text 0x30D860-0x30E580)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// The fast trigonometry (FastMathInitialize, FastSin ... PlusMinusPi) between
// BuoyObject.cpp and JetskiThrillCam.cpp in BuoyObject.cpp's tu_map row, as a
// TU without a static initializer. These functions have no data (every
// constant is a lui/ori immediate), so the name is a guess and nothing
// decides between a TU of their own and BuoyObject.cpp's tail; link order
// puts an "F..." file here. `make link` places the object's code
// (tools/link_c.py).


extern "C" double fmod(double x, double y);

// Retail's fabs is an inline-asm helper: abs.s into $f8, stored through a
// pointer to a stack temporary.
static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

// ---- src/global/FastMathInitialize.cpp
void FastMathInitialize()
{
}

// ---- src/global/FastSin.cpp
//
// Identified by constants:
//   0x3FC90FDB = pi/2, 0x40490FDB = pi
//   0xBFC90FDB = -pi/2, 0xC0490FDB = -pi
//   Polynomial coefficients are Taylor series terms:
//     0xBE2AAAAB = -1/6   (x^3 term)
//     0x3C088889 =  1/120 (x^5 term)
//     0xB9500D01 = -1/5040 (x^7 term)
//     0x3638EF1D =  1/362880 (x^9 term)
//
// Algorithm:
//   1. Range-reduce to [-pi/2, pi/2]:
//        if x > pi/2:  x = pi - x
//        if x < -pi/2: x = -pi - x
//   2. 9th-order Taylor: sin(x) = x + x^3*(c3 + x^2*(c5 + x^2*(c7 + x^2*c9)))
//
// Called by slerp (FastInterpolateQuaternion__FP10QuaternionP10QuaternionP10Quaternionf) three times per interpolation.

float FastSin(float x)
{
    if (x > 1.57079637f) {          // pi/2
        x = 3.14159274f - x;        // pi - x
    } else if (x < -1.57079637f) {  // -pi/2
        x = -3.14159274f - x;       // -pi - x
    }

    float x2 = x * x;
    // Horner form of Taylor series
    float p = -0.000198412701f + 2.75573188e-06f * x2;  // -1/5040 + x^2/362880
    p = 0.00833333377f + x2 * p;                  // 1/120
    p = -0.166666672f + x2 * p;                   // -1/6
    float x3 = x * x2;
    return x + x3 * p;
}

// ---- src/global/FastCos.cpp
// Cosine by Taylor series, after folding x into [-pi/2, pi/2].
float FastCos(float x)
{
    float sign = 1.0f;
    if (x > 1.57079637f) {
        x = 3.14159274f - x;
        sign = -1.0f;
    } else if (x < -1.57079637f) {
        x = -3.14159274f - x;
        sign = -1.0f;
    }
    float x2 = x * x;
    return sign * (1.0f + x2 * (-0.5f + x2 * (0.0416666679f + x2 * (-0.00138888892f +
                   x2 * (2.48015876e-05f + -2.755732e-07f * x2)))));
}

// ---- src/global/FastTan.cpp
// tan(x) = sin(x) / cos(x), both by Taylor series after folding x into
// [-pi/2, pi/2]; FLT_MAX where cos(x) is 0.
float FastTan(float x)
{
    float sign = 1.0f;
    if (x > 1.57079637f) {
        x = 3.14159274f - x;
        sign = -1.0f;
    } else if (x < -1.57079637f) {
        x = -3.14159274f - x;
        sign = -1.0f;
    }
    float x2 = x * x;
    float cosine = sign * (1.0f + x2 * (-0.5f + x2 * (0.0416666679f + x2 * (-0.00138888892f +
                           x2 * (2.48015876e-05f + -2.755732e-07f * x2)))));
    float sine = x + x * x2 * (-0.166666672f + x2 * (0.00833333377f + x2 * (-0.000198412701f + 2.75573188e-06f * x2)));
    if (cosine == 0.0f)
        return 3.40282347e+38f;
    return sine / cosine;
}

// ---- src/global/FastSinCosTan.cpp
// Sine, cosine and tangent together (see FastSinCos, FastTan); the tangent
// is FLT_MAX where the cosine is 0.
void FastSinCosTan(float x, float *sine, float *cosine, float *tangent)
{
    float sign = 1.0f;
    if (x > 1.57079637f) {
        x = 3.14159274f - x;
        sign = -1.0f;
    } else if (x < -1.57079637f) {
        x = -3.14159274f - x;
        sign = -1.0f;
    }
    float x2 = x * x;
    float c = sign * (1.0f + x2 * (-0.5f + x2 * (0.0416666679f + x2 * (-0.00138888892f +
                      x2 * (2.48015876e-05f + -2.755732e-07f * x2)))));
    float s = x + x * x2 * (-0.166666672f + x2 * (0.00833333377f + x2 * (-0.000198412701f + 2.75573188e-06f * x2)));
    *cosine = c;
    *sine = s;
    if (c == 0.0f)
        *tangent = 3.40282347e+38f;
    else
        *tangent = s / c;
}

// ---- src/global/FastSinCos.cpp
// Sine and cosine together, by Taylor series after folding x into
// [-pi/2, pi/2] (see FastSin, FastCos).
void FastSinCos(float x, float *sine, float *cosine)
{
    float sign = 1.0f;
    if (x > 1.57079637f) {
        x = 3.14159274f - x;
        sign = -1.0f;
    } else if (x < -1.57079637f) {
        x = -3.14159274f - x;
        sign = -1.0f;
    }
    float x2 = x * x;
    *cosine = sign * (1.0f + x2 * (-0.5f + x2 * (0.0416666679f + x2 * (-0.00138888892f +
                      x2 * (2.48015876e-05f + -2.755732e-07f * x2)))));
    *sine = x + x * x2 * (-0.166666672f + x2 * (0.00833333377f + x2 * (-0.000198412701f + 2.75573188e-06f * x2)));
}

// ---- src/global/FastAtan2.cpp
// atan2(y, x) by the rational approximation atan(z) ~ z / (1 + 0.28 z^2),
// with atan(z) = pi/2 - z / (z^2 + 0.28) for z > 1, per quadrant.
float FastAtan2(float y, float x)
{

    if (x != 0.0f) {
        if (y == 0.0f) {
            if (x > 0.0f)
                return 0.0f;
            return 3.14159274f;
        }
        if (y > 0.0f) {
            if (x > 0.0f) {
                float z = Fabs(y / x);
                if (z > 1.0f)
                    return 1.57079637f - z / (0.28f + z * z);
                return z / (1.0f + 0.28f * z * z);
            } else {
                float z = Fabs(y / x);
                if (z > 1.0f)
                    return 1.57079637f + z / (0.28f + z * z);
                return 3.14159274f - z / (1.0f + 0.28f * z * z);
            }
        } else {
            if (x > 0.0f) {
                float z = Fabs(y / x);
                if (z > 1.0f)
                    return -1.57079637f + z / (0.28f + z * z);
                return -z / (1.0f + 0.28f * z * z);
            } else {
                float z = Fabs(y / x);
                if (z > 1.0f)
                    return -1.57079637f - z / (0.28f + z * z);
                return -3.14159274f + z / (1.0f + 0.28f * z * z);
            }
        }
    }
    if (y > 0.0f)
        return 1.57079637f;
    if (y < 0.0f)
        return -1.57079637f;
    return 0.0f;
}

// ---- src/global/FastASin.cpp
// Arc sine by a polynomial fit in |x|.
float FastASin(float x)
{
    float r;
    float a = Fabs(x);
    if (x > 1.0f)
        r = 1.57079637f;
    else if (x < -1.0f)
        r = 4.71238899f;  // sic: 3*pi/2, not -pi/2
    else
        r = 0.0328437015f + a * (-1.45183837f + a * (29.6615391f + a * (-131.11235f +
            a * (262.813049f + (-242.719955f + 84.3146591f * a) * a))));
    if (x < 0.0f)
        return -r;
    return r;
}

// ---- src/global/FastACos.cpp
// Arc cosine: pi/2 minus FastASin's polynomial in |x|.
float FastACos(float x)
{
    float a = Fabs(x);
    if (!(x <= 1.0f))
        return 0.0f;
    if (x < -1.0f)
        return 3.14159274f;
    float r = 0.0328437015f + a * (-1.45183837f + a * (29.6615391f + a * (-131.11235f +
              a * (262.813049f + (-242.719955f + 84.3146591f * a) * a))));
    if (x < 0.0f)
        r = 1.57079637f + r;
    else
        r = 1.57079637f - r;
    return r;
}

// ---- src/global/PlusMinusPi.cpp

#define PI 3.14159274f
#define TWO_PI 6.28318548f

// Wraps an angle into [-pi, pi].
void PlusMinusPi(float &angle)
{
    angle = fmod(angle, TWO_PI);
    if (angle > PI)
        angle -= TWO_PI;
    else if (angle < -PI)
        angle += TWO_PI;
}
