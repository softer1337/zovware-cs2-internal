#pragma once
#include "../../../sdk/math.hpp"


class CViewSetup {

public:
    unsigned char pad0[0x450];
    float flOrthoLeft;
    float flOrthoTop;
    float flOrthoRight;
    float flOrthoBottom;
    unsigned char pad1[0x38];
    float flFov;
    float flFovViewmodel;
    Vec3 vecOrigin;
    unsigned char pad2[0xC];
    Vec3 angView;
    unsigned char pad3[0x14];
    float flAspectRatio;
    unsigned char pad4[0x79];
    unsigned char nSomeFlags;

};


using OverrideView_t = void(__fastcall*)(__int64 a1, CViewSetup* a2);
inline OverrideView_t oOverrideView = nullptr;


void Hook_overrideView();