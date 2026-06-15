#pragma once
#include "schema.h"	
#include "../core/mem.hpp"
#include "interface.h"
#include "entitysystem.hpp"
#include "bone.hpp"
#include "mesh_sdk.h"
#include "utl.hpp"
#include "input/usercmd.h"

class C_CSPlayerPawn;
class C_CSPlayerController {
public:
	uint64_t GetSteamID() {
		return MEM::read<uint64_t>((uintptr_t)this + 0x780);
	}
    SCHEMA(m_hPlayerPawn, CHandle<C_CSPlayerPawn>, "CCSPlayerController", "m_hPlayerPawn");
    SCHEMA(m_sSanitizedPlayerName, CUtlString, "CCSPlayerController", "m_sSanitizedPlayerName");
    SCHEMA(m_iPawnHealth, uint32_t, "CCSPlayerController", "m_iPawnHealth");
    SCHEMA(m_nTickBase, int32_t, "CBasePlayerController", "m_nTickBase");
    SCHEMA(m_hPawn, CHandle<C_CSPlayerPawn>, "CBasePlayerController", "m_hPawn")
    SCHEMA(m_iszPlayerName, const char*, "CBasePlayerController", "m_iszPlayerName");

    CUserCmdManager* GetCmdManager() {
        static uintptr_t addr = PatternScan("client.dll", "41 56 41 57 48 83 EC ? 48 8D 54 24");
        return reinterpret_cast<CUserCmdManager * (__fastcall*)(C_CSPlayerController*)>(addr)(this);
    }
};
class CPlayer_WeaponServices {
public:
	SCHEMA(m_active_weapon, c_base_handle, "CPlayer_WeaponServices", "m_hActiveWeapon");
	SCHEMA(my_weapons, CNetworkUtlVector<c_base_handle>, "CPlayer_WeaponServices", "m_hMyWeapons");
};

class CPlayer_ItemServices {
public:
	SCHEMA(m_bHasDefuser, bool, "CCSPlayer_ItemServices", "m_bHasDefuser");
	SCHEMA(m_bHasHelmet, bool, "CCSPlayer_ItemServices", "m_bHasHelmet");
    SCHEMA(m_bHasHeavyArmor, bool, "CCSPlayer_ItemServices", "m_bHasHeavyArmor");
};

class WeaponData_t {
public:
	SCHEMA(m_flRange, float, "CCSWeaponBaseVData", "m_flRange");
	SCHEMA(m_flHeadshotMultiplier, float, "CCSWeaponBaseVData", "m_flHeadshotMultiplier");
	SCHEMA(m_flArmorRatio, float, "CCSWeaponBaseVData", "m_flArmorRatio");
	SCHEMA(m_nDamage, int, "CCSWeaponBaseVData", "m_nDamage");
	SCHEMA(m_flPenetration, float, "CCSWeaponBaseVData", "m_flPenetration");
	SCHEMA(m_flRangeModifier, float, "CCSWeaponBaseVData", "m_flRangeModifier");
	SCHEMA(m_nNumBullets, int, "CCSWeaponBaseVData", "m_nNumBullets");
};

struct WeaponInaccuracyData_t {
    float flTotalInaccuracy;
    float flMoveInaccuracy;
    float flAirInaccuracy;
};

class C_CSWeaponBase {
public:
	SCHEMA(m_flRecoilIndex, float, "C_CSWeaponBase", "m_flRecoilIndex");

	[[nodiscard]] WeaponData_t* GetWeaponData()
	{
		return MEM::read<WeaponData_t*>((uintptr_t)this + 0x388);
	}
    WeaponInaccuracyData_t GetInaccuracy() {
        WeaponInaccuracyData_t ret{};

        using Fn = float(__fastcall*)(void*, float*, float*);

        static Fn fn = nullptr;

        if (!fn)
            fn = reinterpret_cast<Fn>(PatternScan("client.dll", GETINACCURACY_PATTERN));

        if (!fn)
            return ret;

        ret.flTotalInaccuracy = fn(this, &ret.flMoveInaccuracy, &ret.flAirInaccuracy);

        return ret;
    }
    float get_max_speed() {
        return MEM::CallVFunc<float, 366>(this);
    }
    [[nodiscard]] float GetSpread()
    {
        using Fn = float(__fastcall*)(void*);

        static Fn fn = nullptr;

        if (!fn)
        {
            fn = reinterpret_cast<Fn>(
                PatternScan("client.dll", GETSPREAD_PATTERN)
                );
        }

        if (!fn)
            return 0.0f;

        return fn(this);
    }

};


class c_entity_identity
{
public:
    OFFSET(int, index, 0x10);

    bool is_valid();
    int get_serial_number();
    int get_entry_index();
};


class CEntityInstance
{
public:
    SCHEMA(m_pEntity, c_entity_identity, "CEntityInstance", "m_pEntity");

};

class CPlayer_MovementServices {
public:
    SCHEMA(m_flSurfaceFriction, float, "CPlayer_MovementServices_Humanoid", "m_flSurfaceFriction");
    SCHEMA(m_flStamina, float, "CCSPlayer_MovementServices", "m_flStamina");
};

class C_CSPlayerPawn {
public:
    SCHEMA(m_hud_model_arms, c_base_handle, "C_CSPlayerPawn", "m_hHudModelArms");
    SCHEMA(m_pItemServices, CPlayer_ItemServices*, "C_BasePlayerPawn", "m_pItemServices");
    SCHEMA(m_iTeamNum, int32_t, "C_BaseEntity", "m_iTeamNum");
    SCHEMA(m_ArmorValue, int32_t, "C_CSPlayerPawn", "m_ArmorValue");

    SCHEMA(m_fFlags, int32_t, "C_BaseEntity", "m_fFlags");
    SCHEMA(m_vecVelocity, Vec3, "C_BaseEntity", "m_vecVelocity");
    SCHEMA(m_vecAbsVelocity, Vec3, "C_BaseEntity", "m_vecAbsVelocity");
    SCHEMA(m_iHealth, int32_t, "C_BaseEntity", "m_iHealth");
    SCHEMA(m_flSpawnTime, float, "C_CSPlayerPawnBase", "m_flLastSpawnTimeIndex");
	SCHEMA(m_vOldOrigin, Vec3, "C_BasePlayerPawn", "m_vOldOrigin");
	SCHEMA(m_vecViewOffset, Vec3, "C_BaseModelEntity", "m_vecViewOffset");
    SCHEMA(m_hOriginalController, CHandle<C_CSPlayerController>, "C_CSPlayerPawnBase", "m_hOriginalController");
    SCHEMA(m_hController, CHandle<C_CSPlayerController>, "C_CSPlayerPawnBase", "m_hController");

    SCHEMA(m_nNoInterpolationTick, int32_t, "C_BaseEntity", "m_nNoInterpolationTick");
    SCHEMA(m_bSimulationTimeChanged, bool, "C_BaseEntity", "m_bSimulationTimeChanged");
    SCHEMA(m_flSimulationTime, float, "C_BaseEntity", "m_flSimulationTime");
    SCHEMA(m_flOldSimulationTime, float, "C_BaseEntity", "m_flOldSimulationTime");
    SCHEMA(m_lifeState, uint8_t, "C_BaseEntity", "m_lifeState");
    SCHEMA(m_bGunGameImmunity, bool, "C_CSPlayerPawn", "m_bGunGameImmunity");
    SCHEMA(m_nActualMoveType, int, "C_BaseEntity", "m_nActualMoveType");

    SCHEMA(m_pMovementServices, CPlayer_MovementServices*, "C_BasePlayerPawn", "m_pMovementServices");
    SCHEMA(m_pWeaponServices, CPlayer_WeaponServices*, "C_BasePlayerPawn", "m_pWeaponServices");

    SCHEMA(m_pGameSceneNode, C_GameSceneNode*, "C_BaseEntity", "m_pGameSceneNode");

    bool HasArmour(int iHitgroup) {
        if (!m_pItemServices())
            return false;

        switch (iHitgroup) {
        case HITGROUP_HEAD:
            return m_pItemServices()->m_bHasHelmet();
        case HITGROUP_CHEST:
        case HITGROUP_STOMACH:
        case HITGROUP_LEFTARM:
        case HITGROUP_RIGHTARM:
            return true;
        default:
            return false;
        }
    }

    C_CSPlayerController* GetController() {
        return reinterpret_cast<C_CSPlayerController*>(
            Interface::GetEntitySystem()->getLocalController()
            );
    }
    void set_velocity(Vec3* velocity) {
        using SetVelocity_t = void(__fastcall*)(C_CSPlayerPawn*, Vec3*);
        static SetVelocity_t set_vel = (SetVelocity_t)PatternScan("client.dll", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 80 B9 ? ? ? ? ? 48 8B EA");

        set_vel(this, velocity);
        
    }
    float get_some_timing(int idx0, int idx1)
    {
        using GetSomeTiming_t = float(__fastcall*)(C_CSPlayerPawn*, int, int);

        static GetSomeTiming_t get_some_timing =
            (GetSomeTiming_t)PatternScan(
                "client.dll",
                "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 49 63 D8 48 8B F1"
            );

        return get_some_timing(this, idx0, idx1);
    }
    bool is_enemy() {
        C_CSPlayerPawn* local_pawn = (C_CSPlayerPawn*)Interface::GetEntitySystem()->getLocalPawn();

        if (!local_pawn)
            return true;

        if (this == local_pawn)
            return false;

        return this->m_iTeamNum() != local_pawn->m_iTeamNum();
    }
    bool is_alive() {
        return m_iHealth() > 0 && m_lifeState() == 0U;
    }
};