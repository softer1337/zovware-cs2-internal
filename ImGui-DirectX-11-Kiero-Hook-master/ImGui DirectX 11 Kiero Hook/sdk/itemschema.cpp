#include "itemschema.hpp"
#include <algorithm>
#include "localize.hpp"


bool C_ItemSchema::is_paint_kit_for_item(const char* simple_weapon_name, C_PaintKit* paint_kit) {
	if (!simple_weapon_name || !paint_kit || !paint_kit->m_name) {
		printf("[paintkit] invalid input simple_weapon=%p kit=%p name=%p\n",
			simple_weapon_name, paint_kit, paint_kit ? paint_kit->m_name : nullptr);
		return false;
	}

	std::string path = "panorama/images/econ/default_generated/" +
		std::string(simple_weapon_name) + "_" +
		paint_kit->m_name + "_light_png.vtex_c";

	bool exists = Interface::GetIFileSystem()->exists(path.c_str(), "GAME");

	return exists;
}

void C_ItemSchema::build_paint_kits_for_item(uint16_t def_index, CUtlMap<int, C_EconItemDefinition*>& items, CUtlMap<int, C_PaintKit*>& paint_kit_map) {
	C_EconItemDefinition* item_def = nullptr;
	const int items_n = items.count();
	for (int i = 0; i < items_n; i++) {
		auto& node = items.element(i);
		if (node.m_value && node.m_value->m_definition_index == def_index) {
			item_def = node.m_value;
			break;
		}
	}

	if (!item_def) {
		return;
	}

	const char* simple_name = item_def->get_item_name();
	if (!simple_name || simple_name[0] == '\0') {
		return;
	}



	item_paint_kits[def_index].push_back({ 0, "Default","Dedault", 1 });

	const int kits_n = paint_kit_map.count();
	for (int i = 0; i < kits_n; i++) {
		auto& node = paint_kit_map.element(i);
		if (!node.m_value) {
			continue;
		}

		C_PaintKit* kit = node.m_value;
		if (!kit->m_name || kit->m_id <= 0) {
			continue;
		}

		if (is_paint_kit_for_item(simple_name, kit)) {
			const char* display = localize::find_safe(kit->m_description_tag);
			if (!display || !*display)
				display = kit->m_name;

			uint32_t item_rarity = item_def->m_item_rarity;

			uint32_t paint_kit_rarity = kit->m_rarity;

			int final_rarity = (int)item_rarity + (int)paint_kit_rarity - 1;
			if (final_rarity < 0) final_rarity = 0;

			if (paint_kit_rarity == 7) {
				if (final_rarity > 7) final_rarity = 7;
			}
			else {
				if (final_rarity > 6) final_rarity = 6;
			}

			item_paint_kits[def_index].push_back({
		kit->m_id,
		std::string(display),
		std::string(kit->m_name),
		final_rarity
				});
		}
	}


	auto& kits = item_paint_kits[def_index];
	if (kits.size() > 1) {
		std::sort(kits.begin(), kits.end(), [](const PaintKitInfo_t& a, const PaintKitInfo_t& b) {
			if (a.rarity != b.rarity)
				return a.rarity > b.rarity;
			return a.name < b.name;
			});
	}
}
 
void C_ItemSchema::initialize()
{
	printf("[item_schema] init start\n");

	if (m_initialized)
	{
		printf("[item_schema] already initialized\n");
		return;
	}

	auto* item_system = Interface::GetIEngine2Client()->GetEconItemSystem();
	if (!item_system)
	{
		printf("[item_schema] item_system = nullptr\n");
		return;
	}

	printf("[item_schema] item_system OK\n");

	auto* item_schema = item_system->get_econ_item_schema();
	if (!item_schema)
	{
		printf("[item_schema] item_schema = nullptr\n");
		return;
	}

	printf("[item_schema] item_schema OK\n");

	auto& items = item_schema->get_sorted_item_definition_map();
	auto& paint_kit_map = item_schema->get_paint_kits();

	printf("[item_schema] maps OK | items count = %d\n", items.count());

	const int items_n = items.count();
	if (items_n <= 0)
	{
		printf("[item_schema] items_n <= 0\n");
		return;
	}

	int knife_c = 0, glove_c = 0, weapon_c = 0, skipped = 0;
	for (int i = 0; i < items_n; i++)
	{
		auto& node = items.element(i);
		if (!node.m_value)
		{
			skipped++;
			continue;
		}

		if (!node.m_value->m_item_type_name)
		{
			skipped++;
			continue;
		}

		C_EconItemDefinition* item_def = node.m_value;

		std::string item_name;
		if (item_def->m_item_base_name && item_def->m_item_base_name[0] != '\0')
		{
			const char* localized = localize::find_safe(item_def->m_item_base_name);
			item_name = (localized && *localized) ? localized : item_def->m_item_base_name;
		}
		else
		{
			item_name = "Item " + std::to_string(item_def->m_definition_index);
		}

		const char* model_path = item_def->get_model_name();

		if (item_def->is_knife(false))
		{
			knives.push_back({ item_def->m_definition_index, item_name, model_path });
			knife_c++;
		}
		else if (item_def->is_glove(false))
		{
			gloves.push_back({ item_def->m_definition_index, item_name, model_path });
			glove_c++;
		}
		else
		{
			const char* type_name = item_def->m_item_type_name;

			bool is_weapon =
				strstr(type_name, "Pistol") ||
				strstr(type_name, "Rifle") ||
				strstr(type_name, "SMG") ||
				strstr(type_name, "Sniper") ||
				strstr(type_name, "Shotgun") ||
				strstr(type_name, "Machine") ||
				strstr(type_name, "SubMachinegun");

			if (is_weapon && item_def->m_definition_index > 0 && item_def->m_definition_index <= 70)
			{
				weapons.push_back({ item_def->m_definition_index, item_name, model_path });
				weapon_c++;
			}
			else
			{
				skipped++;
			}
		}
	}
	std::sort(weapons.begin(), weapons.end(), [](const ItemInfo_t& a, const ItemInfo_t& b) {
		return a.definition_index < b.definition_index;
		});

	for (auto& knife : knives)
		if (knife.definition_index != 0)
			build_paint_kits_for_item(knife.definition_index, items, paint_kit_map);

	for (auto& glove : gloves)
		if (glove.definition_index != 0)
			build_paint_kits_for_item(glove.definition_index, items, paint_kit_map);

	for (auto& weapon : weapons)
		if (weapon.definition_index != 0)
			build_paint_kits_for_item(weapon.definition_index, items, paint_kit_map);

	for (auto& knife : knives)
		knife_names_cstr.push_back(knife.name.c_str());
	for (auto& glove : gloves)
		glove_names_cstr.push_back(glove.name.c_str());
	for (auto& weapon : weapons)
		weapon_names_cstr.push_back(weapon.name.c_str());

	for (auto& [def_index, kits] : item_paint_kits)
		for (auto& kit : kits)
			item_paint_kit_names[def_index].push_back(kit.name.c_str());


	m_initialized = true;

	printf("[item_schema] INIT SUCCESS | knives=%d gloves=%d weapons=%d\n",
		(int)knives.size(),
		(int)gloves.size(),
		(int)weapons.size());
}