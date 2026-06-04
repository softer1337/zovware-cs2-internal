#pragma once
#include <cstdint>
#include <cstddef>
#include "hash.hpp"
#include "../core/mem.hpp"
#include "schema.h"
#include "mesh_sdk.h"

class c_base_handle
{
public:
	c_base_handle() noexcept :
		m_index(INVALID_EHANDLE_INDEX) {
	}

	c_base_handle(const int nEntry, const int nSerial) noexcept {
		m_index = nEntry | (nSerial << NUM_SERIAL_NUM_SHIFT_BITS);
	}

	bool operator!=(const c_base_handle& other) const noexcept {
		return m_index != other.m_index;
	}

	bool operator==(const c_base_handle& other) const noexcept {
		return m_index == other.m_index;
	}

	bool operator<(const c_base_handle& other) const noexcept {
		return m_index < other.m_index;
	}

	bool is_valid() const noexcept {
		return m_index != INVALID_EHANDLE_INDEX;
	}

	int get_entry_index() const noexcept {
		return static_cast<int>(m_index & ENT_ENTRY_MASK);
	}

	int get_serial_number() const noexcept {
		return static_cast<int>(m_index >> NUM_SERIAL_NUM_SHIFT_BITS);
	}

	inline int to_int() const {
		return static_cast<int>(m_index);
	}

private:
	std::uint32_t m_index;
};


class C_EconItemDefinition {
public:
	bool is_knife(bool exclude_default) {
		static auto csgo_type_knife = fnv1a::hash_64("#CSGO_Type_Knife");
		if (fnv1a::hash_64(m_item_type_name) != csgo_type_knife)
			return false;

		return exclude_default ? m_definition_index >= 500 : true;
	}

	bool is_glove(bool exclude_default) {
		static auto type_hands = fnv1a::hash_64("#Type_Hands");
		if (fnv1a::hash_64(m_item_type_name) != type_hands)
			return false;

		const bool default_glove = m_definition_index == 5028 || m_definition_index == 5029;
		return exclude_default ? !default_glove : true;
	}

	bool is_agent() {
		static auto type_custom_player = fnv1a::hash_64("#Type_CustomPlayer");
		return fnv1a::hash_64(m_item_type_name) == type_custom_player;
	}

	const char* get_model_name() {
		return *reinterpret_cast<const char**>((uintptr_t)(this) + 0x148);
	}

	const char* get_simple_weapon_name() {
		return *reinterpret_cast<const char**>((uintptr_t)(this) + 0x230);
	}

	const char* get_weapon_name() {
		return *reinterpret_cast<const char**>((uintptr_t)(this) + 0x248);
	}

	const char* get_item_name() {
		return *reinterpret_cast<const char**>((uintptr_t)(this) + 0x260);
	}

	const char* get_view_model() {
		return *reinterpret_cast<const char**>((uintptr_t)(this) + 0x348);
	}

	const char* get_vo_prefix() {
		return *reinterpret_cast<const char**>((uintptr_t)(this) + 0xEB0);
	}

public:
	char pad_001[0x8];
	void* m_kv_item;
	std::uint16_t m_definition_index;
	char pad_002[0x1e];
	bool m_enabled;
	char pad_003[0xf];
	std::uint8_t m_min_item_level;
	std::uint8_t m_max_item_level;
	std::uint8_t m_item_rarity;
	std::uint8_t m_item_quality;
	char pad_004[0x2c];
	char* m_item_base_name;
	char pad_005[0x8];
	char* m_item_type_name;
	char pad_006[0x8];
	char* m_item_desc;
};

class C_PaintKit {
public:
	int m_id;
	const char* m_name;
	const char* m_description_string;
	const char* m_description_tag;
	char pad0[0x8];
	char pad1[0x8];
	char pad2[0x8];
	char pad3[0x8];
	char pad4[0x4];
	int m_rarity;

	bool uses_old_model() {
		return *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + 0xAE);
	}
};



class C_AttributeList {
public:
	SCHEMA(m_attributes, uintptr_t, "CAttributeList", "m_Attributes");
};

enum EItemQuality : int {
	QUALITY_DEFAULT = 0,
	QUALITY_GENUINE = 1,
	QUALITY_VINTAGE = 2,
	QUALITY_UNUSUAL = 3,
	QUALITY_UNIQUE = 4,
	QUALITY_COMMUNITY = 5,
	QUALITY_DEVELOPER = 6,
	QUALITY_SELFMADE = 7,
	QUALITY_CUSTOMIZED = 8,
	QUALITY_STRANGE = 9,
	QUALITY_COMPLETED = 10,
	QUALITY_HAUNTED = 11,
	QUALITY_TOURNAMENT = 12,
};

class C_EconItemView {
public:
	SCHEMA(m_definition_index, uint16_t, "C_EconItemView", "m_iItemDefinitionIndex");
	SCHEMA(m_entity_quality, int32_t, "C_EconItemView", "m_iEntityQuality");
	SCHEMA(m_item_id, uint64_t, "C_EconItemView", "m_iItemID");
	SCHEMA(m_item_id_high, uint32_t, "C_EconItemView", "m_iItemIDHigh");
	SCHEMA(m_item_id_low, uint32_t, "C_EconItemView", "m_iItemIDLow");
	SCHEMA(m_account_id, uint32_t, "C_EconItemView", "m_iAccountID");
	SCHEMA(m_initialized, bool, "C_EconItemView", "m_bInitialized");
	SCHEMA_ARRAY(m_attribute_list, C_AttributeList, "C_EconItemView", "m_AttributeList");
	SCHEMA(m_bDisallowSOCm, bool, "C_EconItemView", "m_bDisallowSOC");
	SCHEMA(m_bRestoreCustomMaterialAfterPrecache, bool, "C_EconItemView", "m_bRestoreCustomMaterialAfterPrecache");
	SCHEMA_ARRAY(m_custom_name, char, "C_EconItemView", "m_szCustomName");
	OFFSET(std::uintptr_t, m_name_description_ptr, 0x200);

	C_EconItemDefinition* get_static_data() {
		return MEM::CallVFunc<C_EconItemDefinition*, 13>(this);
	}

	C_PaintKit* construct_paint_kit() {
		static auto fn = reinterpret_cast<C_PaintKit * (__fastcall*)(void*)>(
			PatternScan("client.dll", "48 89 5C 24 ? 56 48 83 EC ? 48 8B 01 FF 50")
			);
		if (fn) {
			return fn(this);
		}
		return nullptr;
	}

	int get_custom_paint_kit() {
		return MEM::CallVFunc<int, 2>(this);
	}
};

class C_AttributeContainer {
public:
	C_EconItemView* m_item() {
		return reinterpret_cast<C_EconItemView*>((uintptr_t)this + 0x50);
	}
};

class c_econ_entity : public C_BaseEntity {
public:
	SCHEMA_ARRAY(m_attribute_manager, C_AttributeContainer, "C_EconEntity", "m_AttributeManager");
	SCHEMA(m_paint_kit, int, "C_EconEntity", "m_nFallbackPaintKit");
	SCHEMA(m_seed, int, "C_EconEntity", "m_nFallbackSeed");
	SCHEMA(m_wear, float, "C_EconEntity", "m_flFallbackWear");
	SCHEMA(m_StatTrak, int, "C_EconEntity", "m_nFallbackStatTrak");

	SCHEMA(m_original_owner_xuid_low, uint32_t, "C_EconEntity", "m_OriginalOwnerXuidLow");
	SCHEMA(m_original_owner_xuid_high, uint32_t, "C_EconEntity", "m_OriginalOwnerXuidHigh");

	uint64_t get_original_owner_xuid() {
		return (static_cast<uint64_t>(m_original_owner_xuid_high()) << 32) | m_original_owner_xuid_low();
	}

	void update_composite(bool force = true) {
		MEM::CallVFunc<void*, 10>(this, force);
	}

	void update_subclass(uint16_t def_index = 0) {
		static auto fn = reinterpret_cast<void(__fastcall*)(void*)>(
			PatternScan("client.dll", "4C 8B DC 53 48 81 EC ?? ?? ?? ?? 48 8B 41")
			);

		if (def_index != 0) {
			char buf[16];
			sprintf_s(buf, "%d", def_index);
			uint32_t hash = string_token_hash(buf);
			m_nSubclassID() = hash;
		}
		fn(this);
	}

	void update_skin(bool force = true) {
		MEM::CallVFunc<void, 110>(this, force);
	}

	void update_weapon_data() {
		MEM::CallVFunc<void, 195>(this);
	}

	C_EconItemView* get_econ_item_view() {
		return this->m_attribute_manager()->m_item();
	}

	void add_stattrak_entity() {
		static auto fn = reinterpret_cast<void(__fastcall*)(void*, void*)>(
			PatternScan("client.dll", "48 8B C4 48 89 58 08 48 89 70 10 57 48 83 EC 50 33 F6 8B FA"
			)
			);

		if (fn) {
			fn(this, this->get_econ_item_view());
		}
	}

	void add_nametag_entity() {
		static auto fn = reinterpret_cast<void(__fastcall*)(void*, void*)>(PatternScan("client.dll","40 55 53 56 48 8D AC 24 B0 FC FF FF 48 81 EC 50"));

		if (fn) {
			fn(this, this->get_econ_item_view());
		}

	}

	void rebuild_animgraph() {
		static auto fn = reinterpret_cast<void(__fastcall*)(void*)>(
			PatternScan("client.dll","40 55 56 48 83 EC 28 4C 89 74 24 58 48 8B F1 80"
			)
			);

		if (fn) {
			fn(this);
		}
	}


};


enum e_item_def_index_t : short
{
	WEAPON_DEAGLE = 1,
	WEAPON_DUAL_BERETTAS = 2,
	WEAPON_FIVE_SEVEN = 3,
	WEAPON_GLOCK = 4,
	WEAPON_AK47 = 7,
	WEAPON_AUG = 8,
	WEAPON_AWP = 9,
	WEAPON_FAMAS = 10,
	WEAPON_G3SG1 = 11,
	WEAPON_GALIL = 13,
	WEAPON_M249 = 14,
	WEAPON_M4A4 = 16,
	WEAPON_MAC10 = 17,
	WEAPON_P90 = 19,
	WEAPON_UMP = 24,
	WEAPON_MP5SD = 23,
	WEAPON_XM1024 = 25,
	WEAPON_BIZON = 26,
	WEAPON_MAG7 = 27,
	WEAPON_NEGEV = 28,
	WEAPON_SAWEDOFF = 29,
	WEAPON_TEC9 = 30,
	WEAPON_TASER = 31,
	WEAPON_HKP2000 = 32,
	WEAPON_MP7 = 33,
	WEAPON_MP9 = 34,
	WEAPON_NOVA = 35,
	WEAPON_P250 = 36,
	WEAPON_SCAR20 = 38,
	WEAPON_SG553 = 39,
	WEAPON_SSG08 = 40,
	WEAPON_KNIFE = 42,
	WEAPON_FLASHBANG = 43,
	WEAPON_HEGRENADE = 44,
	WEAPON_SMOKE = 45,
	WEAPON_MOLOTOV = 46,
	WEAPON_DECOY = 47,
	WEAPON_INCDENDIARY = 48,
	WEAPON_HEALTHSHOT = 57,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_S = 60,
	WEAPON_USP_S = 61,
	WEAPON_CZ75 = 63,
	WEAPON_REVOLVER = 64,
	WEAPON_BAYONET = 500,
	WEAPON_KNIFECSS = 503,
	WEAPON_KNIFEFLIP = 505,
	WEAPON_KNIFEGUT = 506,
	WEAPON_KNIFEKARAMBIT = 507,
	WEAPON_KNIFEM9BAYONET = 508,
	WEAPON_KNIFETACTICAL = 509,
	WEAPON_KNIFEFALCHION = 512,
	WEAPON_KNIFESURVIVALBOWIE = 514,
	WEAPON_KNIFEBUTTERFLY = 515,
	WEAPON_KNIFEPUSH = 516,
	WEAPON_KNIFECORD = 517,
	WEAPON_KNIFECANIS = 518,
	WEAPON_KNIFEURSUS = 519,
	WEAPON_KNIFEGYPSYJACKKNIFE = 520,
	WEAPON_KNIFEOUTDOOR = 521,
	WEAPON_KNIFESTILETTO = 522,
	WEAPON_KNIFEWIDOWMAKER = 523,
	WEAPON_KNIFESKELETON = 525,
	WEAPON_KNIFEKUKRI = 526,
};

enum e_weapon_type : uint32_t
{
	WEAPONTYPE_KNIFE = 0x0,
	WEAPONTYPE_PISTOL = 0x1,
	WEAPONTYPE_SUBMACHINEGUN = 0x2,
	WEAPONTYPE_RIFLE = 0x3,
	WEAPONTYPE_SHOTGUN = 0x4,
	WEAPONTYPE_SNIPER_RIFLE = 0x5,
	WEAPONTYPE_MACHINEGUN = 0x6,
	WEAPONTYPE_C4 = 0x7,
	WEAPONTYPE_TASER = 0x8,
	WEAPONTYPE_GRENADE = 0x9,
	WEAPONTYPE_EQUIPMENT = 0xa,
	WEAPONTYPE_STACKABLEITEM = 0xb,
	WEAPONTYPE_FISTS = 0xc,
	WEAPONTYPE_BREACHCHARGE = 0xd,
	WEAPONTYPE_BUMPMINE = 0xe,
	WEAPONTYPE_TABLET = 0xf,
	WEAPONTYPE_MELEE = 0x10,
	WEAPONTYPE_SHIELD = 0x11,
	WEAPONTYPE_ZONE_REPULSOR = 0x12,
	WEAPONTYPE_UNKNOWN = 0x13,
};
