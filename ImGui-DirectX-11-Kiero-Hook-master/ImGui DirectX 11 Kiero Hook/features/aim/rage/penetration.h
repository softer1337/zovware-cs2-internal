#pragma once
#include "../../../sdk/math.hpp"

class C_CSPlayerPawn;

static struct Data_t {
    C_CSPlayerPawn* pTargetPawn{};
    Vec3 vecStartPos{};
    Vec3 vecEndPos{};
    float flDamage{};
    int iHitGroup{};
} mData;

void ScaleDamage(Data_t& mData);
bool FireBullet(Data_t& mData);