#include "../features.h"
#include "../../sdk/econsdk.hpp"
#include "../../sdk/entitysystem.hpp"
#include "../../sdk/itemschema.hpp"
#include "../../core/hooks/hooks.h"
#include "../../sdk/c_csplayerpawn.hpp"



namespace {
	void* (__fastcall* g_alloc)(std::size_t) = nullptr;
	void(__fastcall* g_free)(void*) = nullptr;
}

bool InitMemAlloc() {
	HMODULE tier0 = GetModuleHandleA("tier0.dll");
	if (!tier0)
		return false;

	g_alloc = reinterpret_cast<decltype(g_alloc)>(GetProcAddress(tier0, "MemAlloc_AllocFunc"));
	g_free = reinterpret_cast<decltype(g_free)> (GetProcAddress(tier0, "MemAlloc_FreeFunc"));

	return g_alloc && g_free;
}

void* GameAlloc(std::size_t size) {
	return g_alloc ? g_alloc(size) : nullptr;
}

void GameFree(void* ptr) {
	if (ptr && g_free)
		g_free(ptr);
}


namespace {
	struct EconItemAttribute_t {
		char     pad_0000[0x30];
		uint16_t def_index;
		char     pad_0032[2];
		float    value;
		float    init_value;
		int32_t  refundable_currency;
		bool     set_bonus;
		char     pad_0041[7];
	};

	struct ptr_game_vector_t {
		uint64_t  size;
		uintptr_t ptr;
	};

	enum : uint16_t {
		ATTR_PAINT = 6,
		ATTR_PATTERN = 7,
		ATTR_WEAR = 8,
	};
}
	void create(C_EconItemView* item, int paint_kit, float wear, int seed) {
		if (!item || paint_kit <= 0)
			return;

		auto* attr_list = item->m_attribute_list();
		if (!attr_list)
			return;

		auto* vec = reinterpret_cast<ptr_game_vector_t*>(&attr_list->m_attributes());
		if (vec->size != 0 || vec->ptr != 0)
			return;

		constexpr size_t attr_count = 3;
		auto* attrs = static_cast<EconItemAttribute_t*>(GameAlloc(attr_count * sizeof(EconItemAttribute_t)));
		if (!attrs)
			return;

		memset(attrs, 0, attr_count * sizeof(EconItemAttribute_t));

		attrs[0].def_index = ATTR_PAINT;
		attrs[0].value = static_cast<float>(paint_kit);
		attrs[0].init_value = attrs[0].value;

		attrs[1].def_index = ATTR_PATTERN;
		attrs[1].value = static_cast<float>(seed >= 0 ? seed : 0);
		attrs[1].init_value = attrs[1].value;

		attrs[2].def_index = ATTR_WEAR;
		attrs[2].value = wear >= 0.0f ? wear : 0.01f;
		attrs[2].init_value = attrs[2].value;

		vec->size = attr_count;
		vec->ptr = reinterpret_cast<uintptr_t>(attrs);
	}

	void remove(C_EconItemView* item) {
		if (!item)
			return;

		auto* attr_list = item->m_attribute_list();
		if (!attr_list)
			return;

		auto* vec = reinterpret_cast<ptr_game_vector_t*>(&attr_list->m_attributes());
		if (vec->size == 0)
			return;

		void* ptr = reinterpret_cast<void*>(vec->ptr);
		vec->size = 0;
		vec->ptr = 0;

		GameFree(ptr);
	}











int m_last_team = 0;
float m_last_spawn_time = 0.0f;
std::vector<uint16_t> m_last_weapon_indices{};
bool should_update = true;
int m_update_frames = 0;

uint16_t m_last_knife = 0;
int m_last_knife_paint_kit_id = 0;
float m_last_knife_wear = 0.0001f;
int m_last_knife_seed = 0;


C_BaseEntity* get_hud_weapon(C_BaseEntity* weapon, C_CSPlayerPawn* local_pawn) {
	auto arms_handle = local_pawn->m_hud_model_arms();
	if (!arms_handle.is_valid())
		return nullptr;

	auto* hud_arms = reinterpret_cast<C_BaseEntity*>(
		Interface::GetEntitySystem()->getBaseBntity(arms_handle.get_entry_index())
		);
	if (!MEM::validPtr(hud_arms))
		return nullptr;

	auto* arms_node = hud_arms->m_scene_node();
	if (!MEM::validPtr(arms_node))
		return nullptr;

	for (auto* vm = arms_node->m_child(); MEM::validPtr(vm); vm = vm->m_next_sibling()) {
		auto* vm_owner = vm->m_owner();
		if (!MEM::validPtr(vm_owner))
			continue;

		auto* vm_entity = reinterpret_cast<C_BaseEntity*>(vm_owner);
		auto owner_handle = vm_entity->m_owner_entity();
		if (!owner_handle.is_valid())
			continue;

		if (Interface::GetEntitySystem()->getBaseBntity<C_BaseEntity>(owner_handle.get_entry_index()) == weapon)
			return vm_entity;
	}
	return nullptr;
}



void process_knife(c_econ_entity* weapon, C_EconItemView* item, C_CSPlayerPawn* local_pawn, bool force_update, bool& did_update, uint64_t local_steam_id) {
	if (CFG::SKINS::knifeChanger.m_knife == 0)
		return;

	C_ItemSchema* g_item_schema = ItemSchema::get();

	if (!g_item_schema->is_initialized()
		|| CFG::SKINS::knifeChanger.m_knife >= (int)g_item_schema->knives.size())
		return;

	const uint16_t def_index = item->m_definition_index();
	const uint16_t selected_knife = g_item_schema->knives[CFG::SKINS::knifeChanger.m_knife].definition_index;
	if (selected_knife == 0)
		return;

	int paint_kit_id = g_item_schema->get_paint_kit_id_for_item(selected_knife, CFG::SKINS::knifeChanger.m_paint_kit);
	bool config_changed = (m_last_knife != selected_knife) ||
		(m_last_knife_paint_kit_id != paint_kit_id) ||
		(m_last_knife_wear != CFG::SKINS::knifeChanger.m_wear) ||
		(m_last_knife_seed != CFG::SKINS::knifeChanger.m_seed);


	if (def_index == selected_knife && !config_changed && !force_update)
		return;

	item->m_definition_index() = selected_knife;
	item->m_entity_quality() = QUALITY_UNUSUAL;
	item->m_item_id_high() = 0xFFFFFFFF;
	item->m_initialized() = true;
	item->m_bDisallowSOCm() = false;
	item->m_bRestoreCustomMaterialAfterPrecache() = true;

	if (const char* model_path = g_item_schema->knives[CFG::SKINS::knifeChanger.m_knife].model_path) {
		weapon->SetModel(model_path);

		if (auto* hud_weapon = get_hud_weapon(weapon, local_pawn)) {
			hud_weapon->SetModel(model_path);

		}
	}


	bool uses_old_model = false;
	if (paint_kit_id > 0)
		if (auto* pk = Interface::GetIEngine2Client()->GetEconItemSystem()->get_econ_item_schema()->get_paint_kits().find_by_key(paint_kit_id))
			uses_old_model = pk->uses_old_model();

	uint64_t mesh_mask = uses_old_model ? 1 : 2;
	if (auto* scene_node = weapon->m_scene_node())
		scene_node->set_mesh_group_mask(mesh_mask);
	if (auto* hud_weapon = get_hud_weapon(weapon, local_pawn))
		if (auto* hud_node = hud_weapon->m_scene_node())
			hud_node->set_mesh_group_mask(mesh_mask);

	weapon->m_paint_kit() = paint_kit_id;
	weapon->m_wear() = CFG::SKINS::knifeChanger.m_wear;
	weapon->m_seed() = CFG::SKINS::knifeChanger.m_seed;



	weapon->update_composite(true);

	if (paint_kit_id > 0)
		create(item, paint_kit_id, CFG::SKINS::knifeChanger.m_wear, CFG::SKINS::knifeChanger.m_seed);

	if (CFG::SKINS::knifeChanger.m_custom_name[0] != '\0')
	{
		strcpy_s(item->m_custom_name(), 161, CFG::SKINS::knifeChanger.m_custom_name);
		item->m_name_description_ptr() = 0;
		weapon->add_nametag_entity();
	}

	else
	{
		item->m_custom_name()[0] = '\0';
	}

	weapon->update_subclass(selected_knife);
	weapon->update_skin(true);
	MEM::CallVFunc<void, 195>(weapon);



	m_last_knife = selected_knife;
	m_last_knife_paint_kit_id = paint_kit_id;
	m_last_knife_wear = CFG::SKINS::knifeChanger.m_wear;
	m_last_knife_seed = CFG::SKINS::knifeChanger.m_seed;
	//c_hud::clear_hud_weapon_icon_for(weapon);
	did_update = true;
}

void apply_skin(c_econ_entity* weapon, C_EconItemView* item, int paint_kit_id, float wear, int seed, const char* custom_name, C_CSPlayerPawn* local_pawn, uint16_t def_index)
{
	remove(item);
	auto* controller = local_pawn->GetController();
	uint32_t local_account_id = controller ? (uint32_t)controller->GetSteamID() : 0;

	uint32_t real_item_id_low = item->m_item_id_low();
	uint64_t real_id_full = item->m_item_id();

	item->m_item_id_high() = 0xFFFFFFFF;
	//item->m_item_id_low() = real_item_id_low ;
	//item->m_item_id() = real_id_full;           //m_iItemID
	//item->m_account_id() = local_account_id; //m_iAccountID

	item->m_initialized() = true;            //m_bInitialized
	item->m_bDisallowSOCm() = false;
	item->m_bRestoreCustomMaterialAfterPrecache() = true;

	weapon->m_paint_kit() = paint_kit_id;
	weapon->m_wear() = wear;
	weapon->m_seed() = seed;

	create(item, paint_kit_id, wear, seed);
	weapon->update_composite(true);

	if (custom_name && custom_name[0] != '\0')
	{
		strcpy_s(item->m_custom_name(), 161, custom_name);
		item->m_name_description_ptr() = 0;
		weapon->add_nametag_entity();
	}

	else
	{
		item->m_custom_name()[0] = '\0';
	}

	bool uses_old_model = false;
	C_PaintKit* pk = nullptr;
	if ((pk = Interface::GetIEngine2Client()->GetEconItemSystem()->get_econ_item_schema()->get_paint_kits().find_by_key(paint_kit_id)))
		uses_old_model = pk->uses_old_model();

	uint64_t mesh_mask = uses_old_model ? 2 : 1;

	if (auto* scene_node = weapon->m_scene_node())
		scene_node->set_mesh_group_mask(mesh_mask);

	if (auto* hud_weapon = get_hud_weapon(weapon, local_pawn))
		if (auto* hud_node = hud_weapon->m_scene_node())
			hud_node->set_mesh_group_mask(mesh_mask);



	weapon->m_scene_node();


	weapon->update_skin(true);
	weapon->update_weapon_data();
}

void process_weapon(c_econ_entity* weapon, C_EconItemView* item, C_CSPlayerPawn* local_pawn, bool force_update, bool& did_update, uint64_t local_steam_id) {
	if (weapon->get_original_owner_xuid() != local_steam_id)
		return;
	auto* weapon_service = local_pawn->m_pWeaponServices();
	if (!MEM::validPtr(weapon_service))
		return;
	auto& my_weapons = weapon_service->my_weapons();


	uint16_t def_index = item->m_definition_index();
	int config_index = CFG::SKINS::skinChanger.GetConfigIndex(def_index);
	if (config_index == 0)
		return;

	auto& skin = CFG::SKINS::skinChanger.weapon_skins[config_index];
	if (skin.paint_kit == 0)
		return;
	C_ItemSchema* g_item_schema = ItemSchema::get();
	int paint_kit_id = g_item_schema->get_paint_kit_id_for_item(def_index, skin.paint_kit);
	if (paint_kit_id == 0 || (weapon->m_paint_kit() == paint_kit_id && !force_update))
		return;

	apply_skin(weapon, item, paint_kit_id, skin.wear, skin.seed, skin.custom_name, local_pawn, def_index);
	//c_hud::clear_hud_weapon_icon_for(weapon);
	did_update = true;
}










static void invoke_regen(void(__fastcall* fn)()) {
	__try {
		fn();
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {}
}

void regenerate_skins() {

	static auto fn = reinterpret_cast<void(__fastcall*)()>(
		PatternScan("client.dll",
			"48 83 EC ? E8 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 48 8B 10"));
	if (fn)
		invoke_regen(fn);
}

void FEATURES::SKINS::onFrameStage(int stage)
{
	const bool skin_enabled = true;
	const bool knife_enabled = true;

	if ((!skin_enabled && !knife_enabled) || stage != 7 || !Interface::GetEntitySystem()->getLocalPawn())
		return;


	auto* local_pawn = reinterpret_cast<C_CSPlayerPawn*>(Interface::GetEntitySystem()->getLocalPawn());
	if (!MEM::validPtr(local_pawn) || local_pawn->m_iHealth() <= 0)
		return;


	auto* controller = reinterpret_cast<C_CSPlayerController*>(Interface::GetEntitySystem()->getLocalController());
	if (!controller) return;

	const float current_spawn_time = local_pawn->m_flSpawnTime();
	const int   current_team = local_pawn->m_iTeamNum();
	uint64_t    local_steam_id = controller->GetSteamID();


	const bool team_changed = (current_team != m_last_team) && m_last_team != 0;
	const bool spawn_changed = (current_spawn_time != m_last_spawn_time) && m_last_spawn_time != 0.0f;


	static auto last_cfg_skin = CFG::SKINS::skinChanger;
	static auto last_cfg_knife = CFG::SKINS::knifeChanger;


	bool config_changed = memcmp(&last_cfg_skin, &CFG::SKINS::skinChanger, sizeof(last_cfg_skin)) != 0 ||
		memcmp(&last_cfg_knife, &CFG::SKINS::knifeChanger, sizeof(last_cfg_knife)) != 0;

	std::vector<uint16_t> current_weapon_indices;
	auto* weapon_service = local_pawn->m_pWeaponServices();
	if (MEM::validPtr(weapon_service)) {
		auto& my_weapons = weapon_service->my_weapons();
		for (unsigned int i = 0; i < my_weapons.m_size; i++) {
			auto* weapon = reinterpret_cast<c_econ_entity*>(Interface::GetEntitySystem()->getBaseBntity(my_weapons.m_elements[i].get_entry_index()));
			if (!weapon) continue;

			auto* item = weapon->m_attribute_manager()->m_item();
			if (MEM::validPtr(item)) {
				current_weapon_indices.push_back(item->m_definition_index());
			}
		}
	}


	bool weapon_changed = (current_weapon_indices != m_last_weapon_indices);


	m_last_weapon_indices = current_weapon_indices;

	if (team_changed || spawn_changed || should_update || weapon_changed || config_changed) {
		m_update_frames = 6;
		should_update = false;

		last_cfg_skin = CFG::SKINS::skinChanger;
		last_cfg_knife = CFG::SKINS::knifeChanger;
	}
	auto sync_pawn_state = [&]() {
		m_last_spawn_time = current_spawn_time;
		m_last_team = current_team;
		};


	if (m_update_frames <= 0) {
		sync_pawn_state();
		return;
	}


	if (!MEM::validPtr(weapon_service)) {
		sync_pawn_state();
		return;
	}

	auto& my_weapons = weapon_service->my_weapons();
	auto* entity_system = Interface::GetEntitySystem();
	std::vector<std::pair<C_EconItemView*, uint32_t>> restore_list;
	bool did_update = false;

	for (unsigned int i = 0; i < my_weapons.m_size; i++) {
		auto* weapon = reinterpret_cast<c_econ_entity*>(
			entity_system->getBaseBntity(my_weapons.m_elements[i].get_entry_index())
			);
		if (!weapon) continue;

		auto item = weapon->m_attribute_manager()->m_item();
		if (!MEM::validPtr(item)) continue;

		uint32_t original_id = item->m_item_id_high();
		uint16_t def_index = item->m_definition_index();
		bool is_knife = (def_index == WEAPON_KNIFE || def_index == WEAPON_KNIFE_T || (def_index >= 500 && def_index <= 526));

		if (is_knife && knife_enabled)
			process_knife(weapon, item, local_pawn, true, did_update, local_steam_id);
		else if (!is_knife && skin_enabled)
			process_weapon(weapon, item, local_pawn, true, did_update, local_steam_id);

	}

	if (did_update)
		regenerate_skins();


	sync_pawn_state();
	m_update_frames--;
}