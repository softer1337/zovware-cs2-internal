#include "../../features.h"
#include "../../../core/mem.hpp"
#include "../../../sdk/math.hpp"
#include "../../../sdk/bone.hpp"
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include "../../../sdk/trace/trace.h"
#include "penetration.h"
#include "../../../sdk/schema.h"
#include "hitchance.h"
#include "../../../sdk/utl.hpp"
#include "../../../sdk/ctx.hpp"

static float distance(const Vec3& p1, const Vec3& p2) {
    return std::sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2) + pow(p1.z - p2.z, 2));
}

static Vec3 calcAngle(Vec3 localPos, Vec3 enemyPos) {
    Vec3 delta = enemyPos - localPos;
    float hyp = sqrtf(delta.x * delta.x + delta.y * delta.y);

    Vec3 angles;
    angles.x = atan2f(-delta.z, hyp) * (180.0f / M_PI);
    angles.y = atan2f(delta.y, delta.x) * (180.0f / M_PI);
    angles.z = 0.0f;
    return angles;
}

static Vec3 AngleToVector(const Vec3& angles) {
    float pitch = angles.x * (M_PI / 180.0f);
    float yaw = angles.y * (M_PI / 180.0f);

    float cp = cosf(pitch);
    float sp = sinf(pitch);
    float cy = cosf(yaw);
    float sy = sinf(yaw);

    return Vec3(cp * cy, cp * sy, -sp);
}

static float getFov(const Vec3& viewAngles, const Vec3& aimAngles)
{
    Vec3 viewDir = AngleToVector(viewAngles);
    Vec3 aimDir = AngleToVector(aimAngles);

    float vLen = std::sqrt(viewDir.x * viewDir.x +
        viewDir.y * viewDir.y +
        viewDir.z * viewDir.z);

    float aLen = std::sqrt(aimDir.x * aimDir.x +
        aimDir.y * aimDir.y +
        aimDir.z * aimDir.z);

    if (vLen <= 0.f || aLen <= 0.f)
        return 0.f;

    viewDir.x /= vLen;
    viewDir.y /= vLen;
    viewDir.z /= vLen;

    aimDir.x /= aLen;
    aimDir.y /= aLen;
    aimDir.z /= aLen;

    float dotProduct =
        viewDir.x * aimDir.x +
        viewDir.y * aimDir.y +
        viewDir.z * aimDir.z;

    dotProduct = std::clamp(dotProduct, -1.0f, 1.0f);

    return std::acos(dotProduct) * (180.0f / 3.14159265f);
}




bool should_stop(C_CSPlayerPawn* pLocal) {
    float velocity = pLocal->m_vecAbsVelocity().Length_2d();
    // C_CSWeaponBase* weapon = (C_CSWeaponBase*)(Interface::GetEntitySystem()->getBaseBntity(pLocal->m_pWeaponServices()->m_active_weapon().get_entry_index()));
    //float max_speed = weapon->get_max_speed();
    //printf("vel=%.f max=%.f", velocity, max_speed);

   // if (velocity < 220.f * 0.34f)
   //     return false;

    return true;
}



void FEATURES::AIM::RAGEBOT::onMove(CUserCmd* pCmd)
{
    if (!CFG::AIM::RAGEBOT::isAimEnabled)
        return;

    if (!MEM::IsInGame())
        return;

    if (!pCmd || !pCmd->csgoUserCmd.pBaseCmd || !pCmd->csgoUserCmd.pBaseCmd->pViewAngles)
        return;

    uintptr_t client = MEM::GetClient();
    if (!client || !context->localPawn)
        return;

    int localTeam = context->localPawn->m_iTeamNum();

    uintptr_t entityList = MEM::read<uintptr_t>(client + offsets::dwEntityList);
    if (!entityList)
        return;

    Vec3 localPos = context->localPawn->m_vOldOrigin() +
        context->localPawn->m_vecViewOffset();

    QAngle qView = pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue;
    Vec3 viewAngles(qView.x, qView.y, 0.f);

    float bestFov = CFG::AIM::RAGEBOT::fov;
    Vec3 bestAngle{};
    bool found = false;
    C_CSPlayerPawn* target{};

    for (int i = 1; i < 64; i++)
    {
        uintptr_t controller = MEM::GetEntityByIndex(entityList, i);
        if (!controller) continue;

        uint32_t pawnHandle = MEM::read<uint32_t>(controller + offsets::m_hPlayerPawn);
        if (!pawnHandle) continue;

        C_CSPlayerPawn* playerPawn = (C_CSPlayerPawn*)MEM::GetEntityByHandle(entityList, pawnHandle);
        if (!playerPawn || playerPawn == context->localPawn) continue;

        if (playerPawn->m_iHealth() <= 0)
            continue;

        if (playerPawn->m_iTeamNum() == localTeam)
            continue;

        Vec3 pos = GetBonePosition(static_cast<int>(bone::Head), (uintptr_t)playerPawn);
        if (pos.IsZero())
            continue;

        Data_t data;
        data.iHitGroup = HITGROUP_HEAD;
        data.pTargetPawn = (C_CSPlayerPawn*)playerPawn;
        data.vecStartPos = localPos;
        data.vecEndPos = pos;

        bool visible = isVisibleBone(
            6,
            (uintptr_t)context->localPawn,
            (uintptr_t)playerPawn
        );

        if (!visible &&
            (!CFG::AIM::RAGEBOT::isPenetrationEnabled || !FireBullet(data)))
            continue;

        if (!visible &&
            data.flDamage < CFG::AIM::RAGEBOT::minDamage)
            continue;

        Vec3 aim = calcAngle(localPos, pos);


        float fov = getFov(viewAngles, aim);
        if (fov > bestFov)
            continue;

        bestFov = fov;
        bestAngle = aim;
        found = true;
        target = playerPawn;
    }

    if (!found)
        return;

    if (!CFG::AIM::RAGEBOT::isSilentEnabled)
    {
        pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue.x = bestAngle.x;
        pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue.y = bestAngle.y;
    }
    else {
        pCmd->SetSubTickAngle(QAngle(bestAngle.x, bestAngle.y, 0.f));
    }

    CFG::AIM::RAGEBOT::internal_wantsStop = should_stop(context->localPawn);

    if (!CFG::AIM::RAGEBOT::isForceShootEnabled)
        return;

    if (!IsAccurate(QAngle(bestAngle.x, bestAngle.y, bestAngle.z), target, localPos)) 
        return;

    

    pCmd->nButtons.nValue |= 1 << 0;
    
}