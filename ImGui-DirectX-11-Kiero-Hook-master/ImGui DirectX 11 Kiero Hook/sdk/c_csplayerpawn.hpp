#pragma once
#include "schema.h"	
#include "../core/mem.hpp"
#include "interface.h"
#include "entitysystem.hpp"
#include "bone.hpp"
#include "mesh_sdk.h"
#include "utl.hpp"

class C_CSPlayerPawn;
class C_CSPlayerController {
public:
	uint64_t GetSteamID() {
		return MEM::read<uint64_t>((uintptr_t)this + 0x780);
	}
    SCHEMA(m_hPlayerPawn, CHandle<C_CSPlayerPawn>, "CCSPlayerController", "m_hPlayerPawn");
    SCHEMA(m_sSanitizedPlayerName, CUtlString, "CCSPlayerController", "m_sSanitizedPlayerName");
    SCHEMA(m_iPawnHealth, uint32_t, "CCSPlayerController", "m_iPawnHealth");
};
class CPlayer_WeaponServices {
public:
	SCHEMA(m_active_weapon, c_base_handle, "CPlayer_WeaponServices", "m_hActiveWeapon");
	SCHEMA(my_weapons, CNetworkUtlVector<c_base_handle>, "CPlayer_WeaponServices", "m_hMyWeapons");
};

class CPlayer_ItemServices {
public:
	SCHEMA(m_bHasDefuser, bool, "CPlayer_ItemServices", "m_bHasDefuser");
	SCHEMA(m_bHasHelmet, bool, "CPlayer_ItemServices", "m_bHasHelmet");
};

class WeaponData_t {
public:
	SCHEMA(m_flRange, float, "CCSWeaponBaseVData", "m_flRange");
	SCHEMA(m_flHeadshotMultiplier, float, "CCSWeaponBaseVData", "m_flHeadshotMultiplier");
	SCHEMA(m_flArmorRatio, float, "CCSWeaponBaseVData", "m_flArmorRatio");
	SCHEMA(m_nDamage, int32_t, "CCSWeaponBaseVData", "m_nDamage");
	SCHEMA(m_flPenetration, float, "CCSWeaponBaseVData", "m_flPenetration");
	SCHEMA(m_flRangeModifier, float, "CCSWeaponBaseVData", "m_flRangeModifier");
	SCHEMA(m_nNumBullets, int32_t, "CCSWeaponBaseVData", "m_nNumBullets");
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
};