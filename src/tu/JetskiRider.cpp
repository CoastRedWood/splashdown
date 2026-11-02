// JetskiRider.cpp  @ 0x003285B0 (text 0x3285B0-0x32D3B0)
// MATCHED (mwccps2 2.4.1.01, -O4,p -Cpp_exceptions off -sdatathreshold 1 -str readonly, mode: cpp) — byte-identical to retail.
//
// Retail's JetskiRider.cpp as one translation unit: its functions in
// retail order, then its static initializer. `make link` places the
// object's data as well as its code (tools/link_c.py). The vtable brings
// the two @2216@48@ virtual-base thunks at the end. m_jetski is
// JetskiRider.h's Jetski view of the JetskiVehicle; the old files'
// JetskiVehicle views go through a cast to the real class. Their other
// views map onto the shared headers: the wrecker's model is
// WaterWrecker::m_rider->m_root (collision) or m_vehicleObject (Tick), its
// +0xB0 the wreck flag, the "hud" LpGame->m_competition (m_infoText,
// m_performanceMeter), the career competition's +0x40C/+0x410
// m_raceType/m_qualifyPlace, Character::m_frame (+0x34).



#include "D3DIMSoultreeCharacter/D3DIMSoultreeCharacter.h"
#include "JetskiRider/JetskiRider.h"
#include "soultree_assert_block.h"
#include "SoultreeObject/SoultreeObject.h"
#include "ParameterBlock/ParameterBlock.h"
#include "ParticleEmitter/ParticleEmitter.h"
#include "JetskiStuntsKey/JetskiStuntsKey.h"
#include "JetskiRiderSoundEmitter/JetskiRiderSoundEmitter.h"
#include "AuralScape/AuralScape.h"
#include "JetskiGame/JetskiGame.h"
#include "JetskiGameOptions/JetskiGameOptions.h"
#include "Scene/Scene.h"
#include "MotionLibrary/MotionLibrary.h"
#include "WreckType/WreckType.h"
#include "d3dtypes.h"
#include "PhysicsRigidBody/PhysicsRigidBody.h"
#include "JetskiVehicle/JetskiVehicle.h"
#include "JetskiCompetition/JetskiCompetition.h"
#include "JetskiCareerCompetition/JetskiCareerCompetition.h"
#include "RaceStatusStruct_Tag/RaceStatusStruct_Tag.h"
#include "RaceStatusVehicleInterface/RaceStatusVehicleInterface.h"
#include "GameObject/GameObject.h"
#include "RaceStatusClass/RaceStatusClass.h"
#include "WaterWrecker/WaterWrecker.h"
#include "LocalizedString/LocalizedString.h"
#include "GameText/GameText.h"
#include "JetskiPerformanceMeter/JetskiPerformanceMeter.h"
#include "JetskiInfoText/JetskiInfoText.h"
// The axis vectors every Soultree TU defines (and __sinit constructs).
#include "sinit/AxisVectors.h"

extern "C" char *strcpy(char *dst, const char *src);
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);
extern "C" char *strstr(const char *s, const char *find);
extern "C" char *strcat(char *dst, const char *src);
extern "C" int strncasecmp(const char *a, const char *b, unsigned int n);
extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" void *memset(void *dst, int c, unsigned int n);
extern "C" unsigned int strlen(const char *s);

extern JetskiGame *LpGame;
extern MotionLibrary *LpMotionLibrary;

// The rider's splash (0x250 bytes).
#ifdef SPLASHDOWN_PC
#include "SplashParticleEmitter/SplashParticleEmitter.h"
#else
class SplashParticleEmitter : public ParticleEmitter {
public:
    SplashParticleEmitter(int enabled);
    virtual ~SplashParticleEmitter();
    char pad1E8[0x200 - 0x1E8];
    _D3DVECTOR m_splash[2];             // +0x200 (16-aligned, as in SplashParticleEmitter.h)
    char pad220[0x250 - 0x220];
};
#endif

static inline float Fmin(float a, float b)
{
    float r;
    asm("min.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

// An inlined strrchr(name, '.').
static inline char *FindExtension(char *s)
{
    char *start = s;
    while (*s++)
        ;
    do {
        s--;
    } while (s != start && *s != '.');
    if (*s == '.')
        return s;
    return 0;
}

#define LOAD_MOTION(name)                                       \
    (sprintf(motionName, "%s%s", name, suffix),                 \
     LpMotionLibrary->MotionOfName(motionName, 1))

// Retail builds a 4-float vector (w = 1) and passes it as a _D3DVECTOR *.
struct WreckVector {
    float x, y, z, w;
};



static inline float Fabs(float x)
{
    float r;
    asm("abs.s $f8, %1; swc1 $f8, 0(%0)" : : "r"(&r), "f"(x) : "$f8", "memory");
    return r;
}

static inline float Fmax(float a, float b)
{
    float r;
    asm("max.s $f8, %1, %2; swc1 $f8, 0(%0)" : : "r"(&r), "f"(a), "f"(b) : "$f8", "memory");
    return r;
}

// Rows (up x forward, up, forward, position) of an orientation matrix,
// re-orthogonalised and normalised on VU0 (as TriggerObject/Vu0Orient.h);
// the vf1 operand (loaded, unused) is a default-constructed local.
static inline void Vu0OrientMatrix(_D3DMATRIX *m, _D3DVECTOR *position, _D3DVECTOR &forwardIn, _D3DVECTOR &upIn, int mode, int keep)
{
    _D3DVECTOR origin; // (0, 0, 0, 1)
    _D3DVECTOR forward = forwardIn;
    _D3DVECTOR up = upIn;
    asm("lqc2 vf1, 0x0(%4); lqc2 vf2, 0x0(%2); lqc2 vf3, 0x0(%3); lqc2 vf4, 0x0(%6); addi $7, $0, 1; addi $6, $0, 2; bne %0, $7, _orientNormalize; nop; vopmula.xyz ACC, vf2, vf3; vopmsub.xyz vf5, vf3, vf2; vsub.w vf5, vf5, vf5; bne %1, $7, _orientKeepUp; nop; vopmula.xyz ACC, vf3, vf5; vopmsub.xyz vf2, vf5, vf3; vsub.w vf2, vf2, vf2; b _orientNormalize; _orientKeepUp: vopmula.xyz ACC, vf5, vf2; vopmsub.xyz vf3, vf2, vf5; vsub.w vf3, vf3, vf3; _orientNormalize: vmul.xyz vf5, vf2, vf2; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf6, vf0, vf0; vaddw.xyzw vf6, vf6, vf2w; vwaitq; vmulq.xyz vf6, vf2, Q; vmul.xyz vf5, vf3, vf3; vaddy.x vf5, vf5, vf5y; vaddz.x vf5, vf5, vf5z; vrsqrt Q, vf0w, vf5x; vsub.xyzw vf7, vf0, vf0; vaddw.xyzw vf7, vf7, vf3w; vwaitq; vmulq.xyz vf7, vf3, Q; vopmula.xyz ACC, vf6, vf7; vopmsub.xyz vf5, vf7, vf6; vsub.w vf5, vf5, vf5; vsub.w vf5, vf5, vf5; vsub.w vf6, vf6, vf6; vsub.w vf7, vf7, vf7; vsub.w vf4, vf4, vf4; vadd.w vf4, vf4, vf0; sqc2 vf5, 0x0(%5); sqc2 vf6, 0x10(%5); sqc2 vf7, 0x20(%5); sqc2 vf4, 0x30(%5)"
        : : "r"(mode), "r"(keep), "r"(&up), "r"(&forward), "r"(&origin), "r"(m), "r"(position) : "$7", "memory");
}

// JetskiRider.h types the Jetski view's +0x2C0 as floats (a _D3DVECTOR there
// moves DisplayStuntText), so its vectors are read through this view.
struct JetskiPose {
#ifdef SPLASHDOWN_PC
    char pad0[__builtin_offsetof(JetskiVehicle, m_x2C0)];
#else
    char pad0[0x2C0];
#endif
    _D3DVECTOR m_position;              // +0x2C0
    _D3DVECTOR m_x2D0;                  // +0x2D0
    _D3DVECTOR m_forward;               // +0x2E0
    _D3DVECTOR m_up;                    // +0x2F0
};
#define POSE(jetski) ((JetskiPose *)(jetski))

JetskiRider::JetskiRider(int construct)
    : GameObject(1), D3DIMSoultreeCharacter(construct)
{
    int i;
    m_x220 = -1;
    m_terrain = 0;
    m_water = 0;
    m_wrecker = 0;
    strcpy(m_file, "");
    m_player = -1;
    m_splash = 0;
    m_sounds = 0;
    m_x4A0 = 0;
    m_x4A4 = 0;
    m_x4AC = 0;
    m_x4A8 = 0;
    m_x4B0 = 0.0f;
    m_x4B4 = 0.0f;
    m_x354 = 1;
    m_x35C = 1.0f;
    m_x358 = 1.0f;
    m_x360 = 0;
    m_x364 = 0;
    m_x368 = 0;
    m_x880 = 0.0f;
    m_x884 = 0.0f;
    m_x894 = 0;
    m_stuntF = -1.0f;
    m_x494 = -1;
    m_x484 = 0;
    m_x48C = 0;
    m_winAnimation = 0;
    m_x49C = 0;
    for (i = 0; i < 6; i++)
        m_raceMotions[i] = 0;   // and m_placeMotions
    for (i = 0; i < 32; i++)
        m_stunts[i] = 0;        // and m_wobbles
    m_x89C = 0;
    m_x888 = 0;
    m_stuntsKey = 0;
    m_stunt4BC = 0;
    m_stunt4C0 = 0;
    m_stunt4C4 = 0;
    m_stunt4C8 = 0;
    m_stunt4CC = 0;
    m_stunt4D0 = 0;
    m_stunt4D4 = 0;
    m_stunt7FC = 1;
    m_x870 = 0;
    m_stunt = -1;
    for (i = 0; i < NUM_STUNTS; i++) {
        m_table800[i] = 0;
        m_table4D8[i] = 0;
        m_table548[i] = 0;
        m_table5B8[i] = 0;
        m_table628[i] = 0;
        m_table698[i] = 0;
        m_table708[i] = 0;
        m_table778[i] = 0;
    }
    for (i = 0; i < 5; i++)
        m_x7E8[i] = 0;
    m_x340 = 0;
    m_x898 = 0;
    m_x350 = 0;
    m_blendRate = 0;
    m_blendTime = 1.0f;
    m_x34C = 0;
    memset(m_x404, 0, sizeof(m_x404));
    m_x8A4 = 0;
    ResetVariables();
}

JetskiRider::~JetskiRider()
{
    if (m_stuntsKey)
        delete m_stuntsKey;
}

#define IS_RIDER(name) (!strncasecmp(mcfName, name, 3))

// Loads a rider: its parameter file, model (the C_ variant in mode 7),
// physics, wrecker, splash, race/place/stunt/wobble motions, stunt keys and
// sounds.
GameObject *JetskiRider::Create(JetskiVehicle *vehicle, JetskiCompetition *competition, char *file, char *keysFile,
                                char *soundFile, LightManager *lights, Terrain *terrain, PSXWater *water, int player,
                                Scene *scene, ParticleManager *particles)
{
    m_jetski = (Jetski *)vehicle;   // the header types it by the fields its other users need
    m_terrain = terrain;
    m_water = water;
    m_player = player;
    m_competition = competition;
    strcpy(m_file, file);
    // Declared here, after the member stores: the parameter block is built
    // after them, and the locals' stack slots follow it.
    ParameterBlock params;
    char mcfName[0x110];
    char path[0x110];
    char modelName[0x110];
    char motionName[0x4C];
    char suffix[4];
    int i;
    if (!params.OpenFile(m_file, 0)) {
        PSXprintf("Failed to load rider physical properties (.PHY) as well in JetskiRider::Create\n");
        return 0;
    }
    params.ReadParameterBlock("Rider Properties");
    params.GetParameter("MotionControlFile", mcfName, 0x104);
    sprintf(path, "Riders\\%s", mcfName);
    strncpy(m_riderName, mcfName, 0xF);
    m_riderName[0xF] = 0;
    char *mcf = strstr(m_riderName, "Mcf");
    if (mcf) {
        *mcf = 0;
    } else {
        PSXprintf("Failed to extract rider's name from %s JetskiRider::Create\n", m_riderName);
        return 0;
    }
    if (LpGame->m_gameOptions->m_raceType == 7) {
        sprintf(modelName, "Riders\\C_%s", mcfName);
        if (!D3DIMSoultreeCharacter::Create(modelName, 0, 1, 1)) {
            PSXprintf("Unable to load model %s in JetskiRider::Create\n", modelName);
            return 0;
        }
    } else {
        sprintf(modelName, "Riders\\%s", mcfName);
        if (!D3DIMSoultreeCharacter::Create(path, 0, 1, 1)) {
            PSXprintf("Unable to load model %s in JetskiRider::Create\n", path);
            return 0;
        }
    }
    if (IS_RIDER("Umi") || IS_RIDER("Jer"))
        strcpy(suffix, "UJ");
    else if (IS_RIDER("Seb") || IS_RIDER("Jua") || IS_RIDER("Ste"))
        strcpy(suffix, "SJ");
    else
        strcpy(suffix, "");
    LoadPhysicsParameters(&params);
    CreateWrecker(particles, scene->m_dynamicNodes[0], path, suffix);
    if (particles) {
        m_splash = new SplashParticleEmitter(1);
        m_splash->Create(particles, 0, 0);
        AddChild(m_splash, -1);
    }
    m_root->AddSearchPlacement(scene->m_staticNodes[0], 2);

    if (IS_RIDER("Bri"))
        strcpy(suffix, "BR");
    sprintf(motionName, "%s%s", "RaceLeanForward", suffix);
    m_raceMotions[0] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 267 "JetskiRider.cpp"
    ASSERTF(m_raceMotions[0], ("Can't load animation %s", motionName));
    sprintf(motionName, "%s%s", "RaceNeutral", suffix);
    m_raceMotions[1] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 270 "JetskiRider.cpp"
    ASSERTF(m_raceMotions[1], ("Can't load animation %s", motionName));
    sprintf(motionName, "%s%s", "RaceLeanBack", suffix);
    m_raceMotions[2] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 273 "JetskiRider.cpp"
    ASSERTF(m_raceMotions[2], ("Can't load animation %s", motionName));

    if (IS_RIDER("Bri"))
        strcpy(suffix, "BR");
    else if (IS_RIDER("Der"))
        strcpy(suffix, "DE");
    else if (IS_RIDER("And"))
        strcpy(suffix, "AN");
    else if (IS_RIDER("Jer"))
        strcpy(suffix, "JE");
    else if (IS_RIDER("Amm"))
        strcpy(suffix, "AM");
    else if (IS_RIDER("Umi"))
        strcpy(suffix, "UM");
    else if (IS_RIDER("Jua"))
        strcpy(suffix, "JU");
    else if (IS_RIDER("Seb") || IS_RIDER("Ste"))
        strcpy(suffix, "SB");
    sprintf(motionName, "%s%s", "1stPlace", suffix);
    m_placeMotions[0] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 295 "JetskiRider.cpp"
    ASSERTF(m_placeMotions[0], ("Can't load animation %s", motionName));

    if (IS_RIDER("Bri"))
        strcpy(suffix, "");
    else if (IS_RIDER("Der"))
        strcpy(suffix, "");
    else if (IS_RIDER("And"))
        strcpy(suffix, "");
    else if (IS_RIDER("Jer"))
        strcpy(suffix, "UJ");
    else if (IS_RIDER("Amm"))
        strcpy(suffix, "");
    else if (IS_RIDER("Umi"))
        strcpy(suffix, "UJ");
    else if (IS_RIDER("Jua"))
        strcpy(suffix, "SJ");
    else if (IS_RIDER("Seb") || IS_RIDER("Ste"))
        strcpy(suffix, "SJ");
    sprintf(motionName, "%s%s", "2ndPlace", suffix);
    m_placeMotions[1] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 317 "JetskiRider.cpp"
    ASSERTF(m_placeMotions[1], ("Can't load animation %s", motionName));

    if (IS_RIDER("Bri"))
        strcpy(suffix, "BR");
    else if (IS_RIDER("Jer"))
        strcpy(suffix, "JE");
    else if (IS_RIDER("Amm"))
        strcpy(suffix, "AM");
    else if (IS_RIDER("Umi"))
        strcpy(suffix, "UM");
    sprintf(motionName, "%s%s", "3rdPlace", suffix);
    m_placeMotions[2] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 330 "JetskiRider.cpp"
    ASSERTF(m_placeMotions[2], ("Can't load animation %s", motionName));

    if (IS_RIDER("Bri"))
        strcpy(suffix, "");
    else if (IS_RIDER("Amm"))
        strcpy(suffix, "");
    else if (IS_RIDER("Umi"))
        strcpy(suffix, "UJ");
    else if (IS_RIDER("Jer"))
        strcpy(suffix, "UJ");
    sprintf(motionName, "%s%s", "NACNAC", suffix);
    m_stunts[0] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 343 "JetskiRider.cpp"
    ASSERTF(m_stunts[0], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "SUPERMAN", suffix);
    m_stunts[1] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 347 "JetskiRider.cpp"
    ASSERTF(m_stunts[1], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "CORDOVA", suffix);
    m_stunts[2] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 351 "JetskiRider.cpp"
    ASSERTF(m_stunts[2], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "LAZYBOY", suffix);
    m_stunts[3] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 355 "JetskiRider.cpp"
    ASSERTF(m_stunts[3], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "BACKFLIP", suffix);
    m_stunts[4] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 359 "JetskiRider.cpp"
    ASSERTF(m_stunts[4], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "AIRWALK", suffix);
    m_stunts[5] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 363 "JetskiRider.cpp"
    ASSERTF(m_stunts[5], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "WHIRLYBIRD", suffix);
    m_stunts[6] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 367 "JetskiRider.cpp"
    ASSERTF(m_stunts[6], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "SHINGRAB", suffix);
    m_stunts[7] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 371 "JetskiRider.cpp"
    ASSERTF(m_stunts[7], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "SARANWRAP", suffix);
    m_stunts[8] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 375 "JetskiRider.cpp"
    ASSERTF(m_stunts[8], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "HANDLEBARHANDSTAND", suffix);
    m_stunts[9] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 379 "JetskiRider.cpp"
    ASSERTF(m_stunts[9], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "ROLLOVER", suffix);
    m_stunts[10] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 383 "JetskiRider.cpp"
    ASSERTF(m_stunts[10], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "SEATSURFING", suffix);
    m_stunts[11] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 387 "JetskiRider.cpp"
    ASSERTF(m_stunts[11], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "SPADE", suffix);
    m_stunts[12] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 391 "JetskiRider.cpp"
    ASSERTF(m_stunts[12], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "HEARTATTACK", suffix);
    m_stunts[13] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 395 "JetskiRider.cpp"
    ASSERTF(m_stunts[13], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "DOUBLECANCAN", suffix);
    m_stunts[14] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 399 "JetskiRider.cpp"
    ASSERTF(m_stunts[14], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "CLIFFHANGER", suffix);
    m_stunts[15] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 403 "JetskiRider.cpp"
    ASSERTF(m_stunts[15], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "RACK", suffix);
    m_stunts[16] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 407 "JetskiRider.cpp"
    ASSERTF(m_stunts[16], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "KISSOFDEATH", suffix);
    m_stunts[17] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 411 "JetskiRider.cpp"
    ASSERTF(m_stunts[17], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "METRONOME", suffix);
    m_stunts[18] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 415 "JetskiRider.cpp"
    ASSERTF(m_stunts[18], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "ROUNDHOUSE", suffix);
    m_stunts[19] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 419 "JetskiRider.cpp"
    ASSERTF(m_stunts[19], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "TUMBLEWEED", suffix);
    m_stunts[20] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 423 "JetskiRider.cpp"
    ASSERTF(m_stunts[20], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "TORNADO", suffix);
    m_stunts[21] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 427 "JetskiRider.cpp"
    ASSERTF(m_stunts[21], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "DOUBLEDOUBLECANCAN", suffix);
    m_stunts[22] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 431 "JetskiRider.cpp"
    ASSERTF(m_stunts[22], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "REARHANDSTAND", suffix);
    m_stunts[23] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 435 "JetskiRider.cpp"
    ASSERTF(m_stunts[23], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "BICYCLEKICK", suffix);
    m_stunts[24] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 439 "JetskiRider.cpp"
    ASSERTF(m_stunts[24], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "POMMELHORSE", suffix);
    m_stunts[25] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 443 "JetskiRider.cpp"
    ASSERTF(m_stunts[25], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "AZWUPING", suffix);
    m_stunts[26] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 447 "JetskiRider.cpp"
    ASSERTF(m_stunts[26], ("Can't load stunt %s", motionName));

    if (IS_RIDER("Der"))
        strcpy(motionName, "DerrickSignatureStunt");
    else if (IS_RIDER("Bri"))
        strcpy(motionName, "BrittSignatureStunt");
    else if (IS_RIDER("Umi"))
        strcpy(motionName, "UmiSignatureStuntUJ");
    else if (IS_RIDER("Jer"))
        strcpy(motionName, "JeremySignatureStuntUJ");
    else if (IS_RIDER("And"))
        strcpy(motionName, "AndreasSignatureStunt");
    else if (IS_RIDER("Amm"))
        strcpy(motionName, "AmmanRaSignatureStunt");
    else if (IS_RIDER("Seb"))
        strcpy(motionName, "SebastianSignatureStuntSJ");
    else if (IS_RIDER("Jua"))
        strcpy(motionName, "JuanaSignatureStuntSJ");
    else if (IS_RIDER("Ste"))
        strcpy(motionName, "SteveSignatureStuntSJ");
    m_stunts[27] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 470 "JetskiRider.cpp"
    ASSERTF(m_stunts[27], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "WobbleFront", suffix);
    m_wobbles[0] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 475 "JetskiRider.cpp"
    ASSERTF(m_wobbles[0], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "WobbleRear", suffix);
    m_wobbles[1] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 478 "JetskiRider.cpp"
    ASSERTF(m_wobbles[1], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "WobbleLeft", suffix);
    m_wobbles[2] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 481 "JetskiRider.cpp"
    ASSERTF(m_wobbles[2], ("Can't load stunt %s", motionName));
    sprintf(motionName, "%s%s", "WobbleRight", suffix);
    m_wobbles[3] = LpMotionLibrary->MotionOfName(motionName, 1);
#line 484 "JetskiRider.cpp"
    ASSERTF(m_wobbles[3], ("Can't load stunt %s", motionName));

    m_stuntsKey = new JetskiStuntsKey;
    if (m_stuntsKey) {
        m_stuntsKey->Initialize(keysFile, vehicle, m_player, m_stunts);
        for (i = 0; i < NUM_STUNTS; i++) {
            m_stuntsKey->GetSpecificInfo(i, m_stunt4BC, m_stunt4C0, m_stunt4C4, m_stunt4C8, m_stunt4CC, m_stunt4D0,
                                         m_stunt4D4, m_stunt7FC);
            m_table4D8[i] = m_stunt4BC;
            m_table628[i] = m_stunt4C0;
            m_table698[i] = m_stunt4C4;
            m_table708[i] = m_stunt4C8;
            m_table778[i] = m_stunt4CC;
            m_table548[i] = m_stunt4D0;
            m_table5B8[i] = m_stunt4D4;
            m_table800[i] = m_stunt7FC;
        }
        for (i = 0; i < 5; i++)
            m_x7E8[i] = m_stuntsKey->m_multiStuntBonus[i];
    }
    if (LpGame->m_gameOptions->m_racerActive[vehicle->m_joystick]) {
        delete m_stuntsKey;
        m_stuntsKey = 0;
    }
    TickActionByPercentage(m_raceMotions[1], 0.5f, 0);
    ((JetskiVehicle *)m_jetski)->m_x2C0 = ((JetskiVehicle *)m_jetski)->m_body->m_x60;
    m_root->GetOrientation(0, &((JetskiVehicle *)m_jetski)->m_x2E0, &((JetskiVehicle *)m_jetski)->m_x2F0);
    int computer = LpGame->m_gameOptions->m_racerActive[vehicle->m_joystick] != 0;
    if (LpGame->m_gameOptions->m_raceType != 8 || !computer) {
        m_sounds = new JetskiRiderSoundEmitter(LpGame->m_auralScape->m_soundInterface, *(unsigned int *)((char *)competition->m_x1EC + 0x40), 1);
        if (m_sounds) {
            if (m_sounds->Create(soundFile, (JetskiVehicle *)m_jetski, 100.0f, computer))
                AddChild(m_sounds, -1);
            else if (m_sounds && !m_sounds->Release())
                m_sounds = 0;
        }
    }
    if (LpGame->m_gameOptions->m_raceType != 3 || !LpGame->m_gameOptions->m_racerActive[vehicle->m_joystick])
        competition->SetPointersForVCR((JetskiVehicle *)m_jetski);
    return this;
}


void JetskiRider::CreateWrecker(ParticleManager *particles, SearchNode *node, char *collisionFile, char *suffix)
{
    char motionName[64];

    m_wrecker = new WaterWrecker(1);
    char *extension = FindExtension(collisionFile);
    if (extension)
        strcpy(extension, ".col");
    else
        strcat(collisionFile, (char *)".col");   // redundant cast: retail loads the literal first
    m_wrecker->Create(this, m_terrain, m_water, ((JetskiVehicle *)m_jetski)->m_body, collisionFile, node);
    AddChild(m_wrecker, 2);
    m_wrecker->AddSeparationMotion(WRECK_FRONT, LOAD_MOTION("SepFront"), -1);
    m_wrecker->AddSeparationMotion(WRECK_LEFT, LOAD_MOTION("SepLeft"), -1);
    m_wrecker->AddSeparationMotion(WRECK_REAR, LOAD_MOTION("SepRear"), -1);
    m_wrecker->AddSeparationMotion(WRECK_RIGHT, LOAD_MOTION("SepRight"), -1);
    m_wrecker->AddSeparationMotion(WRECK_UP, LOAD_MOTION("SepUp"), -1);
    m_wrecker->AddInAirMotion(LOAD_MOTION("Air1"), -1);
    m_wrecker->AddInAirMotion(LOAD_MOTION("Air2"), -1);
    m_wrecker->AddBounceMotion(LOAD_MOTION("Bounce1"), -1);
    m_wrecker->AddBounceMotion(LOAD_MOTION("Bounce2"), -1);
    m_wrecker->AddBounceMotion(LOAD_MOTION("Bounce3"), -1);
    m_wrecker->AddTreadMotion(LOAD_MOTION("Float1"), -1);
    m_wrecker->AddTreadMotion(LOAD_MOTION("Float2"), -1);
    m_wrecker->m_particles = particles;
}

#define BONE(name) m_wrecker->m_rider->m_root->GetNamedObject(name)

// The rider's ragdoll: collision points on the limbs, particle links from the
// hands and feet, and buoyancy points on the torso.
void JetskiRider::InitializeCollisionData(SearchNode *node)
{
    m_wrecker->AddCollisionPoint(_D3DVECTOR(0.0f, 0.0f, -0.35f), BONE("Pelvis"));
    m_wrecker->AddCollisionPoint(_D3DVECTOR(0.0f, 0.0f, -0.45f), BONE("Torso"));
    m_wrecker->AddCollisionPoint(_D3DVECTOR(0.0f, 0.9f, 0.0f), BONE("Head"));
    m_wrecker->AddCollisionPoint(_D3DVECTOR(0.0f, 0.0f, -0.2f), BONE("LwrArmL"));
    m_wrecker->AddCollisionPoint(_D3DVECTOR(-0.3f, -0.4f, 0.0f), BONE("HandL"));
    m_wrecker->AddParticleCollisionPoint(_D3DVECTOR(-0.3f, -0.4f, 0.0f), BONE("HandL"), ZeroVector, BONE("Torso"), 1);
    m_wrecker->AddCollisionPoint(_D3DVECTOR(0.0f, 0.0f, -0.2f), BONE("LwrArmR"));
    m_wrecker->AddCollisionPoint(_D3DVECTOR(0.3f, -0.4f, 0.0f), BONE("HandR"));
    m_wrecker->AddParticleCollisionPoint(_D3DVECTOR(0.3f, -0.4f, 0.0f), BONE("HandR"), ZeroVector, BONE("Torso"), 0);
    m_wrecker->AddCollisionPoint(_D3DVECTOR(0.0f, 0.0f, 0.25f), BONE("LwrLegL"));
    m_wrecker->AddCollisionPoint(_D3DVECTOR(0.0f, -0.3f, -0.1f), BONE("FootL"));
    m_wrecker->AddCollisionPoint(_D3DVECTOR(0.0f, -0.3f, 0.75f), BONE("FootL"));
    m_wrecker->AddParticleCollisionPoint(_D3DVECTOR(0.0f, -0.3f, -0.1f), BONE("FootL"), ZeroVector, BONE("Pelvis"), 1);
    m_wrecker->AddCollisionPoint(_D3DVECTOR(0.0f, 0.0f, 0.25f), BONE("LwrLegR"));
    m_wrecker->AddCollisionPoint(_D3DVECTOR(0.0f, -0.3f, -0.1f), BONE("FootR"));
    m_wrecker->AddCollisionPoint(_D3DVECTOR(0.0f, -0.3f, 0.75f), BONE("FootR"));
    m_wrecker->AddParticleCollisionPoint(_D3DVECTOR(0.0f, -0.3f, -0.1f), BONE("FootR"), ZeroVector, BONE("Pelvis"), 0);
    m_wrecker->AddBuoyancyPoint(_D3DVECTOR(0.362f, 0.5f, 0.3f), BONE("Torso"));
    m_wrecker->AddBuoyancyPoint(_D3DVECTOR(-0.362f, 0.5f, -0.3f), BONE("Torso"));
    m_wrecker->AddBuoyancyPoint(_D3DVECTOR(0.362f, 0.5f, -0.3f), BONE("Torso"));
    m_wrecker->AddBuoyancyPoint(_D3DVECTOR(-0.362f, 0.5f, 0.3f), BONE("Torso"));
    m_wrecker->CreateCollisionObject(node);
    m_wrecker->AddVehicleCollisionObject(((JetskiVehicle *)m_jetski)->m_collision);
}


void JetskiRider::LoadPhysicsParameters(ParameterBlock *params)
{
    params->ReadParameterBlock("Rider Properties");
    params->GetParameter("BlendRate", &m_blendRate);
    if (m_blendRate != 0.0f)
        m_blendTime = 1.0f / m_blendRate;
}

int JetskiRider::Render3D()
{
    return GameObject::Render3D();
}

void JetskiRider::ProcessInputs(float dt)
{
    if (m_stuntsKey) {
        if (m_x484) {
            m_x898 = m_stuntsKey->CheckStuntHolding();
        } else {
            m_x898 = 0.0f;
            m_stuntsKey->CheckKeyInputs(dt, m_jetski->m_input->m_player);
            int stunt = m_stuntsKey->CheckStuntActivation(dt, m_jetski->m_input->m_player);
            if (stunt >= 0)
                ActivateStunt(stunt);
        }
    } else if (m_x484 && (m_jetski->m_status->m_flags & 8)) {
        if (m_jetski->m_keys && (m_jetski->m_keys->m_buttons & 0x800))
            m_x898 = 1.0f;
        else
            m_x898 = 0.0f;
    }
}

int JetskiRider::Tick(float dt)
{
    if (LpGame->m_paused)
        return 1;
    if (!m_jetski->m_replay) {
        if (m_competition->m_raceStatus->m_flags & 2) {
            if (!m_x49C) {
                int raceType = LpGame->m_gameOptions->m_raceType;
                if (raceType == 5) {
                    if (m_jetski->m_status->m_flags & 1) {
                        if (m_jetski->m_x530)
                            ((JetskiVehicle *)m_jetski)->StopWreck(1);
                        ActivateWinningAnimation(3);
                    }
                } else if (raceType == 6 && ((JetskiCareerCompetition *)m_competition)->m_raceType == 1) {
                    ActivateWinningAnimation(((JetskiCareerCompetition *)m_competition)->m_qualifyPlace);
                } else if (raceType == 4) {
                    int place = m_competition->m_vehicles[m_player]->m_ownStatus->m_place;
                    if (m_competition->m_x118 == 2) {
                        if (place == 1)
                            ActivateWinningAnimation(1);
                        else
                            ActivateWinningAnimation(3);
                    } else if (!m_competition->m_timeElimTime) {
                        ActivateWinningAnimation(1);
                    }
                } else {
                    int place = m_competition->m_vehicles[m_player]->m_ownStatus->m_place;
                    if (!place)
                        place = 1;
                    ActivateWinningAnimation(place);
                }
            }
        } else {
            ProcessInputs(dt);
        }
        if (!m_jetski->m_x530) {
            AnimateRider(dt);
            Jetski *jetski = m_jetski;
            float *src = jetski->m_input->m_x60;
            jetski->m_x2C0[0] = src[0];
            jetski->m_x2C0[1] = src[1];
            jetski->m_x2C0[2] = src[2];
            jetski->m_x2C0[3] = src[3];
            if (m_x8A4) {
                RaceStatusStruct_Tag *status = m_jetski->m_status;
                RaceStatusClass *raceStatus = m_jetski->m_raceStatus;
                if (status->m_flags & 0x10)
                    raceStatus->EndStunts(status, 0, 0.0f);
                else if (raceStatus->m_raceType == 5 && (status->m_flags & 0x80))
                    status->m_timeRemaining = 0;
                JetskiGameOptions *options = LpGame->m_gameOptions;
                if (options->m_raceType != 8) {
                    switch (options->m_difficulty) {
                    case 1:
                        status->m_x6F4 *= 0.75f;
                        break;
                    case 2:
                        status->m_x6F4 *= 0.5f;
                        break;
                    case 3:
                        status->m_x6F4 = 0;
                        break;
                    }
                }
                m_x8A4 = 0;
            }
        } else {
            m_wrecker->AnimateRider(dt);
            m_wrecker->m_vehicleObject->GetPosition(0, (_D3DVECTOR *)m_jetski->m_x2C0);
            m_stuntF = -1.0f;
            m_x894 = 0;
        }
        m_root->GetOrientation(0, &POSE(m_jetski)->m_forward, &POSE(m_jetski)->m_up);
        PlaybackRider(dt);
    }
    return GameObject::Tick(dt);
}

int JetskiRider::KeyDownHandler(ProcessNode *node, KeyInfo *key)
{
    return GameObject::KeyDownHandler(node, key);
}

void JetskiRider::ActivateStunt(int stunt)
{
    if (stunt >= 0 && stunt < NUM_STUNTS) {
        if (m_jetski->m_x5CC <= 0.0f) {
            m_stuntF = stunt;
            m_stunt = stunt;
            m_x870 = 0;
            if (m_stuntsKey) {
                m_stuntsKey->GetSpecificInfo(m_stunt4BC, m_stunt4C0, m_stunt4C4, m_stunt4C8,
                                             m_stunt4CC, m_stunt4D0, m_stunt4D4, m_stunt7FC);
            } else {
                m_stunt4BC = m_table4D8[stunt];
                m_stunt4D0 = m_table548[stunt];
                m_stunt4D4 = m_table5B8[stunt];
                m_stunt7FC = m_table800[stunt];
                m_stunt4C0 = m_table628[stunt];
                m_stunt4C4 = m_table698[stunt];
                m_stunt4C8 = m_table708[stunt];
                m_stunt4CC = m_table778[stunt];
            }
        }
    }
}

void JetskiRider::ActivateWobble(int amount)
{
    WreckVector v;
    v.x = v.y = v.z = 0.0f;
    v.w = 1.0f;
    if (m_jetski->m_x5CC <= 0.0f) {
        m_stuntF = amount;
        m_wobbling = 1;
        m_sounds->PlayWreckSounds((_D3DVECTOR *)&v, 0, 0, 0);
    }
}

int JetskiRider::ActivateWinningAnimation(int place)
{
    if (place == 1 || place == 2) {
        if (m_x484 || m_wrecker->m_xB0)
            return 0;
        m_winAnimation = place + 2;
    } else {
        m_winAnimation = 5;
    }
    return 1;
}

void JetskiRider::AnimateRider(float dt)
{
    RaceStatusStruct_Tag *status = m_jetski->m_status;
    RaceStatusClass *raceStatus = m_jetski->m_raceStatus;
    if (m_jetski->m_input->m_player)
        m_x35C = 2.0f;
    else
        m_x35C = 1.0f;
    m_x360 = 0.5f * (1.0f + m_jetski->m_x60);
    m_x364 = 0.5f * (1.0f + m_jetski->m_x64);
    // Retail sets the same value either way.
    if (m_x358 == 1.0f)
        m_x368 = 1.0f;
    else
        m_x368 = 1.0f;
    if (!m_wrecker->m_xB0) {
        if (m_x484) {
            m_x874 += dt;
            m_x874 = Fmin(m_x874, m_stunt4BC);
            if (!m_x870 && !(m_frame < m_stunt4C0)) {
                if (m_sounds)
                    m_sounds->PlayStuntSounds();
                m_x870 = 1;
            }
            if (m_x898 != 0.0f && !(m_frame < m_stunt4C0) && m_frame <= m_stunt4C4) {
                if (m_x894 != 1.0f)
                    m_x894 = 1.0f;
            } else {
                m_x894 = 0;
            }
            m_x340 = 0;
        } else if (!m_jetski->m_replay && status) {
            if (status->m_x6F4 > 120.0f)
                status->m_x6F4 = 120.0f;
            if (LpGame->m_gameOptions->m_raceType != 8 && !m_jetski->m_x530) {
                status->m_x6F4 -= 1.5f * dt;
                if (status->m_x6F4 < 0.0f)
                    status->m_x6F4 = 0;
            }
            if ((status->m_flags & 0x10) && !m_jetski->m_input->m_player) {
                m_x340 += dt;
                if (!(m_x340 < 0.0f))
                    raceStatus->EndStunts(status, 1, 1.0f);
            }
        }
    } else {
        if (!m_jetski->m_replay)
            m_stuntF = -1.0f;
        m_x894 = 0;
    }
}

#pragma fast_fptosi on

void JetskiRider::PlaybackRider(float dt)
{
    RaceStatusClass *raceStatus = m_jetski->m_raceStatus;
    RaceStatusStruct_Tag *status = m_jetski->m_status;
    int stunt = (int)m_stuntF;
    if (m_jetski->m_replay) {
        m_root->m_child->SetMatrix(IdentityMatrix());
        _D3DMATRIX m;
        Vu0OrientMatrix(&m, &POSE(m_jetski)->m_position, POSE(m_jetski)->m_forward, POSE(m_jetski)->m_up, 1, 1);
        m_root->SetMatrix(0, &m);
    } else if (m_wrecker->m_wreckType == 0.0f || m_wrecker->m_wreckType == 1.0f) {
        _D3DMATRIX m;
        m_jetski->m_input->m_model->GetMatrix(0, &m);
        m_root->SetMatrix(0, &m);
        if (m_wrecker->m_wreckType == 1.0f)
            return;
    }
    if (m_wrecker->m_wreckType == 0.0f) {
        if (stunt >= 0 && stunt < 32 && !m_x484) {
            m_x484 = 1;
            m_x488 = 0;
            m_x48C = 1;
            m_x4B0 = m_x360;
            m_x4B4 = m_x364;
            m_x874 = 0;
            m_x880 = 0;
            m_x494 = stunt;
            if (stunt < 28 && !m_jetski->m_replay) {
                if (!(status->m_flags & 0x10))
                    raceStatus->BeginStunts(status);
                float scale;
                if (LpGame->m_gameOptions->m_raceType != 5 && m_x404[stunt])
                    scale = 0.5f;
                else
                    scale = 1.0f;
                if (status->m_numStunts > 0)
                    scale *= 2.0f;
                raceStatus->StuntStarted(status, m_table548[stunt], m_table4D8[stunt], 2.0f * m_table548[stunt], scale, stunt, m_table548[stunt] * scale, 12);
                DisplayStuntText(status);
            }
            if (stunt < 32) {
                m_x4A8 = 1;
                float dx = Fabs(m_x360 - 0.5f);
                float dy = Fabs(m_x364 - 0.5f);
                float d = Fmax(4.0f * dx, 2.0f * dy);
                m_x884 = m_blendRate * d;
            } else {
                m_x4A8 = 0;
                m_x884 = 0;
                PerformActionByMotion(m_stunts[m_x494]);
            }
        }
        if (m_x484) {
            // A switch for `if (m_x4A8)`: the block-optimizer event that gives
            // retail's delay-slot fills.
            switch (m_x4A8) {
            case 0:
                break;
            default:
                m_x880 += dt;
                m_x4B0 = (0.5f - m_x360) + m_x360;
                m_x4B4 = (0.5f - m_x364) + m_x364;
                if (!(m_x364 < 0.5f))
                    TickBlendedActionsByPercentage(m_raceMotions[2], m_raceMotions[1], m_x4B0, 2.0f * (m_x4B4 - 0.5f), 0);
                else
                    TickBlendedActionsByPercentage(m_raceMotions[1], m_raceMotions[0], m_x4B0, 2.0f * m_x4B4, 0);
                if (!(m_x880 < m_blendRate)) {
                    m_x4A8 = 0;
                    PerformActionByMotion(m_stunts[m_x494]);
                }
            }
            if (!m_x4A8) {
                // A named zero: retail compares against the register it then passes.
                float zero = 0.0f;
                if (m_x894 == zero) {
                    TickBlendedFrame(dt, 0, 0);
                    if (!m_jetski->m_replay)
                        raceStatus->HoldStunt(status, 0);
                } else {
                    TickBlendedFrame(zero, 0, 0);
                    if (!m_jetski->m_replay)
                        raceStatus->HoldStunt(status, 1);
                }
                if (!(m_frame < m_stunt4C0) && !m_jetski->m_replay)
                    m_stuntF = -1.0f;
                if (m_wrecker->m_wreckType == 0.0f && m_x494 >= 0 && m_x484 && m_x10) {
                    StopSpecialAnimation();
                    if ((status->m_flags & 0x20) && !m_jetski->m_replay) {
                        raceStatus->StuntEnded(status, 1, 1.0f);
                        m_x404[status->m_stunts->m_type] = 1;
                    }
                    m_x358 = 3.0f;
                    m_x880 = 0;
                }
            }
        } else if (m_winAnimation && !m_x49C) {
            if (m_raceMotions[m_winAnimation]) {
                m_x49C = 1;
                PerformActionByMotion(m_raceMotions[m_winAnimation]);
            }
        } else if (m_x49C) {
            TickBlendedFrame(dt, 0, 0);
        } else {
            if (!m_x4A0)
                m_x354 = (int)m_x35C;
            int mode = (int)m_x358;
            switch (m_x354) {
            case 2:
                switch (mode) {
                case 2:
                    m_x4A0 = 0;
                    m_x358 = 2.0f;
                    m_x880 = 0;
                    if (!(m_x364 < 0.5f))
                        TickBlendedActionsByPercentage(m_raceMotions[2], m_raceMotions[1], m_x360, 2.0f * (m_x364 - 0.5f), 0);
                    else
                        TickBlendedActionsByPercentage(m_raceMotions[1], m_raceMotions[0], m_x360, 2.0f * m_x364, 0);
                    break;
                case 1:
                    m_x880 += dt;
                    if (!(m_x880 < m_blendRate)) {
                        m_x880 = 0;
                        m_x358 = 2.0f;
                    }
                    if (!(m_x364 < 0.5f))
                        TickBlendedActionsByPercentage(m_raceMotions[2], m_raceMotions[1], m_x360, 2.0f * (m_x364 - 0.5f), 0);
                    else
                        TickBlendedActionsByPercentage(m_raceMotions[1], m_raceMotions[0], m_x360, 2.0f * m_x364, 0);
                    break;
                case 3:
                    m_x4A0 = 1;
                    m_x880 += dt;
                    if (!(m_x880 < m_blendRate)) {
                        m_x880 = 0;
                        m_x358 = 2.0f;
                    }
                    m_x4B0 = 0.5f + (m_x360 - 0.5f);
                    m_x4B4 = 0.5f + (m_x364 - 0.5f);
                    if (!(m_x4B4 < 0.5f))
                        TickBlendedActionsByPercentage(m_raceMotions[2], m_raceMotions[1], m_x4B0, 2.0f * (m_x4B4 - 0.5f), 0);
                    else
                        TickBlendedActionsByPercentage(m_raceMotions[1], m_raceMotions[0], m_x4B0, 2.0f * m_x4B4, 0);
                    break;
                }
                break;
            case 1:
            default:
                switch (mode) {
                case 2:
                    m_x4A0 = 1;
                    m_x880 += dt;
                    if (!(m_x880 < m_blendRate)) {
                        m_x880 = 0;
                        m_x358 = 1.0f;
                    }
                    if (!(m_x364 < 0.5f))
                        TickBlendedActionsByPercentage(m_raceMotions[2], m_raceMotions[1], m_x360, 2.0f * (m_x364 - 0.5f), 0);
                    else
                        TickBlendedActionsByPercentage(m_raceMotions[1], m_raceMotions[0], m_x360, 2.0f * m_x364, 0);
                    break;
                case 1:
                    m_x4A0 = 0;
                    m_x358 = 1.0f;
                    m_x880 = 0;
                    if (!(m_x364 < 0.5f))
                        TickBlendedActionsByPercentage(m_raceMotions[2], m_raceMotions[1], m_x360, 2.0f * (m_x364 - 0.5f), 0);
                    else
                        TickBlendedActionsByPercentage(m_raceMotions[1], m_raceMotions[0], m_x360, 2.0f * m_x364, 0);
                    break;
                case 3:
                    m_x4A0 = 1;
                    m_x880 += dt;
                    if (!(m_x880 < m_blendRate)) {
                        m_x880 = 0;
                        m_x358 = 1.0f;
                    }
                    m_x4B0 = 0.5f + (m_x360 - 0.5f);
                    m_x4B4 = 0.5f + (m_x364 - 0.5f);
                    if (!(m_x4B4 < 0.5f))
                        TickBlendedActionsByPercentage(m_raceMotions[2], m_raceMotions[1], m_x4B0, 2.0f * (m_x4B4 - 0.5f), 0);
                    else
                        TickBlendedActionsByPercentage(m_raceMotions[1], m_raceMotions[0], m_x4B0, 2.0f * m_x4B4, 0);
                    break;
                }
                break;
            }
        }
    }
    m_root->UpdateSearchPlacement();
}

#pragma fast_fptosi reset

// ---- src/JetskiRider/ResetVariables.cpp (was in the cross-TU stub
// src/JetskiWidgetHighlight/Initialize.cpp)

void JetskiRider::ResetVariables()
{
    m_x874 = 0;
    m_x8A0 = 0;
    m_x484 = 0;
    m_x488 = 0;
    m_x89C = 0;
    m_wobbling = 0;
    m_x49C = 0;
    m_winAnimation = 0;
    m_stunt = -1;
    m_x4A0 = 0;
}

void JetskiRider::StopSpecialAnimation()
{
    Jetski *jetski = m_jetski;
    if (jetski->m_x530 && (jetski->m_status->m_flags & 0x10))
        jetski->m_raceStatus->EndStunts(jetski->m_status, 0, 0.0f);
    m_x484 = 0;
    m_x488 = 1;
    m_x494 = -1;
    m_stuntF = -1.0f;
    m_x894 = 0;
    m_x4A0 = 0;
    m_wobbling = 0;
}

// Shows the names of the stunts in the current combo ("A + B + ...").
void JetskiRider::DisplayStuntText(RaceStatusStruct_Tag *status)
{
    char text[0x30];
    char name[0x30];

    if (status->m_flags & 4) {
        RaceStatusStunt *node = status->m_stunts;
        int length = 0;
        int maxLength = 0x22;
        text[0] = 0;
        while (node) {
            if (node->m_type >= 0)
                sprintf(name, "%s", LpGame->m_competition->m_performanceMeter->StuntsName(node->m_type, LpGame->m_gameOptions->m_racerCharacter[m_player]));
            else
                sprintf(name, "%s", LpGame->m_text->GetString(170));
            if (node->m_next)
                strcat(name, " + ");
            length += strlen(name);
            if (length < maxLength) {
                strcat(text, name);
                node = node->m_next;
            } else {
                strcat(text, "...");
                node = 0;
            }
        }
        float scale = 1.0f;
        LpGame->m_competition->m_infoText->AddInfoText(text, m_jetski->m_player, 3.5f, 0, scale);
    }
}

