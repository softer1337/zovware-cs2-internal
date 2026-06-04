#include "hitchance.h"
#include <vector>

static std::vector<Vec3> CalculateSpreadInternal(int seed, float accuracy, float spread, float recoil_index, int item_def_idx, int num_bullets)
{
	std::vector<Vec3> spread_results(num_bullets);
	static auto fnCalcSpread = reinterpret_cast<void(__fastcall*)(int16_t, int, int, std::uint32_t, float, float, float, float*, float*)>(PatternScan("client.dll", "48 8B C4 48 89 58 ? 48 89 68 ? 48 89 70 ? 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 4C 63 EA"));
	fnCalcSpread(
		item_def_idx,
		num_bullets,
		0,
		seed + 1,
		accuracy,
		spread,
		recoil_index,
		reinterpret_cast<float*>(&spread_results[0].x),
		reinterpret_cast<float*>(&spread_results[0].y)
	);

	return spread_results;
}

bool IsAccurate(QAngle targetAngle, C_CSPlayerPawn* target, Vec3 start)
{
	C_CSPlayerPawn* localPawn = (C_CSPlayerPawn*)Interface::GetEntitySystem()->getLocalPawn();
	CPlayer_WeaponServices* weapon_service = localPawn->m_pWeaponServices();
	C_CSWeaponBase* weapon = (C_CSWeaponBase*)Interface::GetEntitySystem()->getBaseBntity(weapon_service->m_active_weapon().get_entry_index());
	if (!weapon)
		return false;
	WeaponData_t* weaponData = weapon->GetWeaponData();

	const float flWeaponRange = weaponData->m_flRange();
	float flWeaponInaccuracy = weapon->GetInaccuracy().flTotalInaccuracy;
	float flWeaponSpread = weapon->GetSpread();
	float flRecoilIndex = weapon->m_flRecoilIndex();
	int nNumBullets = weaponData->m_nNumBullets();
	int m_nWeaponDefinitionIndex = ((c_econ_entity*)weapon)->m_attribute_manager()->m_item()->m_definition_index();

	Vec3 vForward, vRight, vUp;
	targetAngle.ToDirections(&vForward, &vRight, &vUp);

	const int iHitChance = CFG::AIM::RAGEBOT::hitChance;
	constexpr int nWantedHitCount = 256;
	const int nNeededHits = static_cast<int>(nWantedHitCount * (iHitChance / 100.f));

	int nHits = 0;

	CGameTrace GameTrace = CGameTrace();
	Trace_Filter_t Filter;
	Ray_t Ray = Ray_t();

	Interface::GetTraceManager()->Init(Filter, localPawn, 0x1C300B, 3, 7);

	auto fnCheckHit = [&](Vec3 vSpread)
		{
			Vec3 vDirection = vForward + vRight * vSpread.x + vUp * vSpread.y;
			vDirection.NormalizeInPlace();
			Vec3 vEnd = start + (vDirection * flWeaponRange);
			return Interface::GetTraceManager()->ClipRayToEntity(&Ray, start, vEnd, target, &Filter, &GameTrace) &&
				GameTrace.pHitEntity == target /*&& GameTrace.m_pHitboxData->m_nHitboxId == g_Rage->m_Data.m_LastTarget.m_hitboxData.m_nHitboxIndex*/;
		};

	for (int nSeed = 0; nSeed < nWantedHitCount; nSeed++)
	{
		Vec3 vSpread = CalculateSpreadInternal(nSeed, flWeaponInaccuracy, flWeaponSpread, flRecoilIndex, m_nWeaponDefinitionIndex, nNumBullets)[0];

		if (fnCheckHit(vSpread))
			++nHits;

		const float flCurrentHitChance = (static_cast<float>(nHits) / static_cast<float>(nSeed + 1)) * 100.f;
		if (flCurrentHitChance >= static_cast<float>(iHitChance))
			return true;
	}

	return false;
}