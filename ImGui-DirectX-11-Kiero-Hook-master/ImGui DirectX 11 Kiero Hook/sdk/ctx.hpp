#pragma once
#include "c_csplayerpawn.hpp"
#include <memory>

class Context {
public:
	C_CSPlayerPawn* localPawn;
	C_CSPlayerController* localController;
	C_CSWeaponBase* activeWeapon;
	WeaponData_t* activeWeaponData;

	bool update() {
		localPawn = (C_CSPlayerPawn*)Interface::GetEntitySystem()->getLocalPawn();
		if (!localPawn)
			return false;

		localController = (C_CSPlayerController*)Interface::GetEntitySystem()->getLocalController();
		if (!localController)
			return false;

		activeWeapon = (C_CSWeaponBase*)Interface::GetEntitySystem()->getBaseBntity(localPawn->m_pWeaponServices()->m_active_weapon().get_entry_index());
		if (!activeWeapon)
			return false;

		activeWeaponData = activeWeapon->GetWeaponData();
		if (!activeWeaponData)
			return false;

		return true;
	}
};

inline auto context = std::make_unique<Context>();