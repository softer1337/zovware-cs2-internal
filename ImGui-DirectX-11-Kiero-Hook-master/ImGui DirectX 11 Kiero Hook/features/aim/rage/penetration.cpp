#include "penetration.h"
#include "../../../core/mem.hpp"
#include "../../../sdk/trace/trace.h"
#include "../../../sdk/bone.hpp"
#include "../../../sdk/schema.h"
#include "../../../sdk/c_csplayerpawn.hpp"
#include "../../../sdk/ctx.hpp"


using namespace MEM;
enum ETeamID : int
{
	TEAM_UNK,
	TEAM_SPECTATOR,
	TEAM_TT,
	TEAM_CT
};
#define INVALID_EHANDLE_INDEX 0xFFFFFFFF
#define ENT_ENTRY_MASK 0x7FFF
#define NUM_SERIAL_NUM_SHIFT_BITS 15
// @source: https://developer.valvesoftware.com/wiki/Entity_limit#Source_2_limits
#define ENT_MAX_NETWORKED_ENTRY 16384

int GetEntryIndex(uintptr_t entity)
{
	if (!entity)
		return -1;

	uintptr_t identity = read<uintptr_t>(entity + 0x10); // m_pEntity
	if (!identity)
		return -1;

	uint32_t index = read<uint32_t>(identity + 0x10); // m_index
	return index & 0x7FFF; // ENT_ENTRY_MASK
}


__forceinline C_CSWeaponBase* GetCurreintWeapon(uintptr_t pLocal) {
	uintptr_t wps = read<uintptr_t>(pLocal + offsets::m_pWeaponServices);
	if (!wps)
		return 0;

	uintptr_t weaponHandle = read<uintptr_t>(wps + 0x60);
	if (!weaponHandle)
		return 0;

	uintptr_t entityList = *reinterpret_cast<uintptr_t*>(GetClient() + offsets::dwEntityList);
	if (!entityList) return 0;

	uintptr_t listEntry = read<uintptr_t>(entityList + 0x8 * ((weaponHandle & 0x7FFF) >> 9) + 16);
	if (!listEntry) return 0;

	return read<C_CSWeaponBase*>(listEntry + 112 * (weaponHandle & 0x1FF));
}





void ScaleDamage(Data_t& mData) {
	if (!mData.pTargetPawn)
		return;

	CPlayer_ItemServices* ItemServices = mData.pTargetPawn->m_pItemServices();
	if (!ItemServices)
		return;


	auto flDamageScaleCTHead = 1.f;
	auto flDamageScaleCTBody = 1.f;
	auto flDamageScaleTTHead = 1.f;
	auto flDamageScaleTTBody = 1.f;

	bool bIsCT = mData.pTargetPawn->m_iTeamNum() == TEAM_CT;
	bool bIsTT = mData.pTargetPawn->m_iTeamNum() == TEAM_TT;
	float flHeadDamageScale = bIsCT ? flDamageScaleCTHead : bIsTT ? flDamageScaleTTHead : 1.0f;
	float flBodyDamageScale = bIsCT ? flDamageScaleCTBody : bIsTT ? flDamageScaleTTBody : 1.0f;

	if (ItemServices->m_bHasHelmet())
		flHeadDamageScale *= 0.5f;
	switch (mData.iHitGroup) {
	case HITGROUP_HEAD:
		mData.flDamage *= context->activeWeaponData->m_flHeadshotMultiplier() * flHeadDamageScale;
		break;
		case HITGROUP_CHEST:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
		case HITGROUP_NECK:
			mData.flDamage *= flBodyDamageScale;
			break;
		case HITGROUP_STOMACH:
			mData.flDamage *= 1.25f * flBodyDamageScale;
			break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			mData.flDamage *= 0.75f * flBodyDamageScale;
			break;
		default:
			break;
		}
	if (!mData.pTargetPawn->HasArmour(mData.iHitGroup))
		return;

	float flHeavyArmorBonus = 1.0f, flArmorBonus = 0.5f, flArmorRatio = context->activeWeaponData->m_flArmorRatio() * 0.5f;

	if (ItemServices->m_bHasHelmet()) {
		flHeavyArmorBonus = 0.25f;
		flArmorBonus = 0.33f;
		flArmorRatio *= 0.20f;
	}

	float flDamageToHealth = mData.flDamage * flArmorRatio;
	float flDamageToArmor = (mData.flDamage - flDamageToHealth) * flHeavyArmorBonus * flArmorBonus;

	if (flDamageToArmor > static_cast<float>(mData.pTargetPawn->m_ArmorValue()))
	{
		flDamageToHealth =
			mData.flDamage -
			static_cast<float>(mData.pTargetPawn->m_ArmorValue()) / flArmorBonus;
	}


	mData.flDamage = flDamageToHealth;
}


bool FireBullet(Data_t& mData) {
	printf("[FireBullet] start\n");
	printf("StartPos: %.2f %.2f %.2f\n", mData.vecStartPos.x, mData.vecStartPos.y, mData.vecStartPos.z);
	printf("EndPos:   %.2f %.2f %.2f\n", mData.vecEndPos.x, mData.vecEndPos.y, mData.vecEndPos.z);

	Vec3 direction = mData.vecEndPos - mData.vecStartPos;
	direction.NormalizeInPlace();

	TraceData_t trace_data;
	Interface::GetTraceManager()->InitTraceData(&trace_data);

	printf("[Trace] InitTraceData done\n");

	printf("[Trace] start=%.2f %.2f %.2f\n",
		trace_data.m_start.x,
		trace_data.m_start.y,
		trace_data.m_start.z);

	printf("[Trace] end=%.2f %.2f %.2f\n",
		trace_data.m_end.x,
		trace_data.m_end.y,
		trace_data.m_end.z);


	// Aim the damage trace straight at the requested point. Previously a random spread
	// offset was baked into the direction, which pushed the ray off the hitbox and made
	// fire_bullet under-report or miss damage entirely. Spread is the game's runtime
	// randomness — it must NOT be part of the ragebot's damage estimate.



	printf("[Weapon] ptr=%p\n", context->activeWeapon);

	printf("[WeaponData] ptr=%p\n", context->activeWeaponData);


	if (context->activeWeaponData)
	{
		printf("[WeaponData] dmg=%f range=%f pen=%f\n",
			context->activeWeaponData->m_nDamage(),
			context->activeWeaponData->m_flRange(),
			context->activeWeaponData->m_flPenetration());
	}

	Vec3 delta = direction * context->activeWeaponData->m_flRange();
	Vec3 end_pos = mData.vecStartPos + delta;

	trace_data.m_start = mData.vecStartPos;
	trace_data.m_end = end_pos;

	Trace_Filter_t filter;
	Interface::GetTraceManager()->InitializeTraceFilter(&filter, (C_CSPlayerPawn*)MEM::GetLocalPawn(), 0x1C300B, 3, 15);
	filter.m_ptr4 |= 2u;
	filter.m_ptr[0] |= 0x4000000000uLL;

	printf("[Trace] CreateTrace calling...\n");

	Interface::GetTraceManager()->CreateTrace(&trace_data, mData.vecStartPos, delta, &filter, 4);

	printf("[Trace] CreateTrace done\n");

	Interface::GetTraceManager()->DamageToPoint(&trace_data, context->activeWeaponData->m_nDamage(), context->activeWeaponData->m_flPenetration(), context->activeWeaponData->m_flRangeModifier(), context->localPawn->m_iTeamNum());

	printf("[DamageToPoint] surfaces=%d\n", trace_data.m_surfaces_count);
	for (int i = 0; i < trace_data.m_surfaces_count; i++)
	{
		printf("[Loop] i=%d\n", i);

		c_trace_info& info = trace_data.m_trace_info[i];

		//printf("[Info] dist=%f dmg=%f handle=%d\n",
		//	info.m_distance,
		//	info.m_damage,
		//	info.m_handle.to_int());

		//int idx = (info.m_handle.to_int() >> 16) & 0xFFFF;

		//printf("[IDX] %d\n", idx);
	}

	//	printf("[GetTraceInfo] calling idx=%d ptr=%p\n",
	//		idx,
	//		trace_data.m_trace_segments_ptr);
	//	CGameTrace trace;
	//	Interface::GetTraceManager()->GetTraceInfo(&trace_data, &trace, info.m_distance, &trace_data.m_trace_segments_ptr[(info.m_handle.to_int() >> 16) & 0xFFFF]);
	//	printf("[GetTraceInfo] done\n");

	//	if (trace.pHitEntity)
	//	{
	//		printf("[HIT] entity=%p target=%p match=%d\n",
	//			trace.pHitEntity,
	//			mData.pTargetPawn,
	//			trace.pHitEntity == mData.pTargetPawn);
	//	}

	//	if (trace.pHitEntity && trace.pHitEntity == mData.pTargetPawn) {
	//		mData.flDamage = info.m_damage;
	//		mData.iHitGroup = trace.pHitBox->nHitBoxIndex;
	//		ScaleDamage(mData);

	//		printf("[FireBullet] end\n");
	//		return true;
	//	}
	//}
	//printf("[FireBullet] end\n");
	return false;
}