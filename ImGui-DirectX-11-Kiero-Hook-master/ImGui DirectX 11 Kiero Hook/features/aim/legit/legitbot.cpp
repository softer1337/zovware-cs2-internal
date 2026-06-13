#include "../../features.h"
#include "../../../sdk/schema.h"
#include "../../../core/mem.hpp"
#include "../../../sdk/math.hpp"
#include "../../../sdk/bone.hpp"
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include "../../../sdk/trace/trace.h"
#include "../../../sdk/schema.h"
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


void FEATURES::AIM::LEGITBOT::onMove(CUserCmd* pCmd)
{
    if (!CFG::AIM::LEGITBOT::isAimEnabled)
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

    float bestFov = CFG::AIM::LEGITBOT::fov; 
    Vec3 bestAngle{};
    bool found = false;

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

        if (!isVisibleBone(6, (uintptr_t)context->localPawn, (uintptr_t)playerPawn))
            continue;

        Vec3 aim = calcAngle(localPos, pos);

        float fov = getFov(viewAngles, aim);
        if (fov > bestFov)
            continue;

        bestFov = fov;
        bestAngle = aim;
        found = true;
    }

    if (!found)
        return;

 
    float smooth = CFG::AIM::LEGITBOT::smooth / 10.f; 
    if (smooth < 1.f) smooth = 1.f;

    Vec3 delta = bestAngle - viewAngles;
    NormalizeAngles(delta);

    viewAngles.x += delta.x / smooth;
    viewAngles.y += delta.y / smooth;
    NormalizeAngles(viewAngles);

    pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue.x = viewAngles.x;
    pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue.y = viewAngles.y;
}