#pragma once
#include "c_csplayerpawn.hpp"
#include <memory>

constexpr float interval_per_tick = 0.015625f;

constexpr int time_to_ticks(float time) {
	return static_cast<int>(0.5f + time / interval_per_tick);
}

constexpr float ticks_to_time(int ticks) {
	return interval_per_tick * static_cast<float>(ticks);
}

class CUserCmd;

class Context {
public:
	C_CSPlayerPawn* localPawn;
	C_CSPlayerController* localController;
	C_CSWeaponBase* activeWeapon;
	WeaponData_t* activeWeaponData;
	CUserCmd* pCmd;
	CUserCmdManager* pCmdManager;

	bool update(CUserCmd* pCmd1) {
		localPawn = (C_CSPlayerPawn*)Interface::GetEntitySystem()->getLocalPawn();
		if (!localPawn)
			return false;

		localController = (C_CSPlayerController*)Interface::GetEntitySystem()->getLocalController();
		if (!localController)
			return false;

		auto srv = localPawn->m_pWeaponServices();
		if (!srv || !srv->m_active_weapon().is_valid())
			return false;

		activeWeapon = (C_CSWeaponBase*)Interface::GetEntitySystem()->getBaseBntity(srv->m_active_weapon().get_entry_index());
		if (!activeWeapon)
			return false;

		activeWeaponData = activeWeapon->GetWeaponData();
		if (!activeWeaponData)
			return false;

		pCmd = pCmd1;
		if (!pCmd)
			return false;
		
		pCmdManager = localController->GetCmdManager();
		if (!pCmdManager)
			return false;

		return true;
	}
};

inline auto context = std::make_unique<Context>();