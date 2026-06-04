#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include "econsdk.hpp"

template<typename K, typename V>
class CUtlMap {
public:
	struct node_t {
		int m_left;
		int m_right;
		V m_value;
		int m_flag;
		K m_key;
	};
	static_assert(sizeof(node_t) == 24, "c_utl_map::node_t must be 24 bytes to match game layout");

	int m_count;
	int m_capacity_flags;
	node_t* m_elements;

	int count() const { return m_count; }

	node_t& element(int i) { return m_elements[i]; }
	const node_t& element(int i) const { return m_elements[i]; }

	V find_by_key(K key) {
		for (int i = 0; i < m_count; i++) {
			if (m_elements[i].m_key == key)
				return m_elements[i].m_value;
		}
		return nullptr;
	}
};

template<>
class CUtlMap<int, C_PaintKit*> {
public:
	struct node_t {
		int m_left;
		int m_right;
		int _pad_8;
		int _pad_12;
		int m_key;
		int _pad_20;
		C_PaintKit* m_value;
	};
	static_assert(sizeof(node_t) == 32, "paint-kit node_t must be 32 bytes");

	int m_count;
	int m_capacity_flags;
	node_t* m_elements;

	int count() const { return m_count; }

	node_t& element(int i) { return m_elements[i]; }
	const node_t& element(int i) const { return m_elements[i]; }

	C_PaintKit* find_by_key(int key) {
		for (int i = 0; i < m_count; i++) {
			if (m_elements[i].m_key == key)
				return m_elements[i].m_value;
		}
		return nullptr;
	}
};

struct ItemInfo_t {
	uint16_t definition_index;
	std::string name;
	const char* model_path;
	uint32_t subclass_id;
};

struct PaintKitInfo_t {
	int id;
	std::string name;
	std::string skinToken;
	int rarity;
};

class C_ItemSchema {
public:
	std::vector<ItemInfo_t> knives;
	std::vector<ItemInfo_t> gloves;
	std::vector<ItemInfo_t> weapons;

	std::vector<PaintKitInfo_t> all_paint_kits;

	std::unordered_map<uint16_t, std::vector<PaintKitInfo_t>> item_paint_kits;
	std::unordered_map<uint16_t, std::vector<const char*>> item_paint_kit_names;

	std::vector<const char*> knife_names_cstr;
	std::vector<const char*> glove_names_cstr;
	std::vector<const char*> weapon_names_cstr;

	void initialize();
	bool is_initialized() const { return m_initialized; }

	const std::vector<const char*>& get_paint_kit_names_for_item(uint16_t def_index) const {
		static std::vector<const char*> empty = { "Default" };
		auto it = item_paint_kit_names.find(def_index);
		return it != item_paint_kit_names.end() ? it->second : empty;
	}

	int get_paint_kit_id_for_item(uint16_t def_index, int index) const {
		auto it = item_paint_kits.find(def_index);
		if (it != item_paint_kits.end() && index >= 0 && index < (int)it->second.size())
			return it->second[index].id;
		return 0;
	}

private:
	bool m_initialized = false;
	bool is_paint_kit_for_item(const char* simple_weapon_name, C_PaintKit* paint_kit);
	void build_paint_kits_for_item(uint16_t def_index, CUtlMap<int, C_EconItemDefinition*>& items, CUtlMap<int, C_PaintKit*>& paint_kit_map);
};

namespace ItemSchema {
	inline C_ItemSchema* get() {
		static C_ItemSchema instance;
		return &instance;
	}
}





class C_EconItemSchema {
public:

	static constexpr auto OFFSET_SORTED_ITEM_DEF_MAP = 0xF8;
	static constexpr auto OFFSET_PAINT_KITS = 0x2F0;
	static constexpr auto OFFSET_MUSIC_KITS = 0x4D0;

	CUtlMap<int, C_EconItemDefinition*>& get_sorted_item_definition_map() {
		return *reinterpret_cast<CUtlMap<int, C_EconItemDefinition*>*>(
			reinterpret_cast<std::uintptr_t>(this) + OFFSET_SORTED_ITEM_DEF_MAP
			);
	}

	CUtlMap<int, C_PaintKit*>& get_paint_kits() {
		return *reinterpret_cast<CUtlMap<int, C_PaintKit*>*>(
			reinterpret_cast<std::uintptr_t>(this) + OFFSET_PAINT_KITS
			);
	}

	//CUtlMap<int, *>& get_music_kits() {
	//	return *reinterpret_cast<c_utl_map<int, c_music_kit*>*>(
	//		reinterpret_cast<std::uintptr_t>(this) + OFFSET_MUSIC_KITS
	//		);
	//}
};


class C_EconItemSystem {
public:
	C_EconItemSchema* get_econ_item_schema() {

		return *reinterpret_cast<C_EconItemSchema**>(
			reinterpret_cast<std::uintptr_t>(this) + 0x8
			);
	}
};
