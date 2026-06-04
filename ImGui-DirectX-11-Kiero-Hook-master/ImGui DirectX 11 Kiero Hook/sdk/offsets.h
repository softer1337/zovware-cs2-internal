#pragma once
#include <cstddef>


namespace offsets {
    constexpr std::ptrdiff_t dwCSGOInput = 0x2355230;
    constexpr std::ptrdiff_t dwEntityList = 0x24E6590;
    constexpr std::ptrdiff_t dwGameEntitySystem = 0x24E6590;
    constexpr std::ptrdiff_t dwGameEntitySystem_highestEntityIndex = 0x2090;
    constexpr std::ptrdiff_t dwGameRules = 0x2340158;
    constexpr std::ptrdiff_t dwGlobalVars = 0x20606D0;
    constexpr std::ptrdiff_t dwGlowManager = 0x233CF50;
    constexpr std::ptrdiff_t dwLocalPlayerController = 0x231F700;
    constexpr std::ptrdiff_t dwLocalPlayerPawn = 0x2340698;
    constexpr std::ptrdiff_t dwPlantedC4 = 0x234EF88;
    constexpr std::ptrdiff_t dwPrediction = 0x23405A0;
    constexpr std::ptrdiff_t dwSensitivity = 0x233DA68;
    constexpr std::ptrdiff_t dwSensitivity_sensitivity = 0x58;
    constexpr std::ptrdiff_t dwViewAngles = 0x23558B8;
    constexpr std::ptrdiff_t dwViewMatrix = 0x2345B30;
    constexpr std::ptrdiff_t dwViewRender = 0x2345ED8;
    constexpr std::ptrdiff_t dwWeaponC4 = 0x22BDD00;

	static std::ptrdiff_t m_iHealth = 0x34C;
	static std::ptrdiff_t m_fFlags = 0x3F8;
	static std::ptrdiff_t m_vOldOrigin = 0x1390;
	static std::ptrdiff_t m_iTeamNum = 0x3EB;
	static std::ptrdiff_t m_hPlayerPawn = 0x90C;
	static std::ptrdiff_t m_pGameSceneNode = 0x330;
	static std::ptrdiff_t m_vecViewOffset = 0xE70;
    static std::ptrdiff_t m_pWeaponServices = 0x11E0;
}