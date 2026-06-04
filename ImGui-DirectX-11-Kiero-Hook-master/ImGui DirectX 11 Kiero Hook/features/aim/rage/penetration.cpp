#include "penetration.h"
#include "../../../core/mem.hpp"
#include "../../../sdk/trace/trace.h"
#include "../../../sdk/bone.hpp"
#include "../../../sdk/schema.h"
#include "../../../sdk/c_csplayerpawn.hpp"


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

	uintptr_t pLocal = read<uintptr_t>(GetClient() + offsets::dwLocalPlayerPawn);
	if (!pLocal)
		return;

	CPlayer_ItemServices* ItemServices = mData.pTargetPawn->m_pItemServices();
	if (!ItemServices)
		return;

	C_CSWeaponBase* weapon = GetCurreintWeapon(pLocal);
	if (!weapon)
		return;
	WeaponData_t* weaponData = weapon->GetWeaponData();
	if (!weaponData)
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
		mData.flDamage *= weaponData->m_flHeadshotMultiplier() * flHeadDamageScale;
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

	float flHeavyArmorBonus = 1.0f, flArmorBonus = 0.5f, flArmorRatio = weaponData->m_flArmorRatio() * 0.5f;

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


bool FireBullet(Data_t& mData)
{

	if (!mData.pTargetPawn) {
		return false;
	}

	uintptr_t pLocal = read<uintptr_t>(GetClient() + offsets::dwLocalPlayerPawn);

	if (!pLocal) {
		return false;
	}

	CPlayer_ItemServices* ItemServices = mData.pTargetPawn->m_pItemServices();

	if (!ItemServices) {
		return false;
	}

	C_CSWeaponBase* weapon = GetCurreintWeapon(pLocal);

	if (!weapon) {
		return false;
	}

	WeaponData_t* weaponData = weapon->GetWeaponData();

	if (!weaponData) {
		return false;
	}

	Vec3 vecDirection = (mData.vecEndPos - mData.vecStartPos).Normalize();
	


	const Vec3 vecEndPosition =
		mData.vecStartPos + vecDirection * weaponData->m_flRange();

	TraceData_t pTraceData{};
	Interface::GetTraceManager()->InitTraceData(&pTraceData);

	Trace_Filter_t pTraceFilter;

	Interface::GetTraceManager()->Init(
		pTraceFilter,
		(C_CSPlayerPawn*)pLocal,
		0x1C300B,
		4,
		7);

	Interface::GetTraceManager()->CreateTrace(
		&pTraceData,
		mData.vecStartPos,
		vecEndPosition,
		&pTraceFilter,
		4);


	HandleBulletPenetrationData_t mHandleBulletData;
	mHandleBulletData.damage = weaponData->m_nDamage();
	mHandleBulletData.penetration = weaponData->m_flPenetration();
	mHandleBulletData.penetrationCount = 4;
	mHandleBulletData.rangeModifier = weaponData->m_flRangeModifier();
	mHandleBulletData.penetrationStopped = false;
	mHandleBulletData.tailLength = pTraceData.tail_end.Length();

	mData.flDamage = static_cast<float>(weaponData->m_nDamage());

	float flTraceLength = 0.f;
	auto flMaxRange = weaponData->m_flRange();

	auto& arr = pTraceData.mod_array;

	for (int i = 0; i < arr.size; ++i)
	{
		BulletModulateEntry_t* entry = &arr.data[i];
		uint16_t surf_idx = entry->surfEnd & 0x7FFF;
		if (surf_idx >= 0x80) break;
		TraceArrayElement_t* surf = &pTraceData.m_Arr[surf_idx];

		alignas(16) CGameTrace tr {};

		Interface::GetTraceManager()->InitializeTraceInfo(&tr);

		Interface::GetTraceManager()->GetTraceInfo(&pTraceData, &tr, entry->startFrac, surf);



		flMaxRange -= flTraceLength;



		flTraceLength += tr.flFraction * flMaxRange;

		mData.flDamage *= std::powf(
			weaponData->m_flRangeModifier(),
			flTraceLength / 500.f);

		if (tr.pHitEntity)
		{

			int hitIndex =
				GetEntryIndex((uintptr_t)tr.pHitEntity);

			int targetIndex =
				GetEntryIndex((uintptr_t)mData.pTargetPawn);


			if (hitIndex == targetIndex)
			{

				ScaleDamage(mData);

				return true;
			}
		}

		bool result =
			Interface::GetTraceManager()->HandleBulletPenetration(
				&pTraceData,
				&mHandleBulletData,
				entry,
				3,
				0);


		if (result)
		{
			break;
		}
		if (mHandleBulletData.penetrationCount <= 0 || mHandleBulletData.penetrationStopped) {
			break;
		}

		mData.flDamage = mHandleBulletData.damage;
	}


	return false;
}