#include "../imgui/imgui.h"
#include "../features/cfg.hpp"
#include "../features/features.h"
#include "fa_solid_900.h"
#include "font_data.h"
#include "../sdk/itemschema.hpp"

#define ICON_MIN_FA 0xf000
#define ICON_MAX_FA 0xf3ff
#define ICON_FA_COG "\xef\x80\x93"
#define ICON_FA_INFO "\xef\x81\x9a"

ImU32 GetRarityColor(int rarity) {
    switch (rarity) {
    case 1: return IM_COL32(176, 195, 217, 255); // Consumer
    case 2: return IM_COL32(94, 152, 217, 255);  // Industrial
    case 3: return IM_COL32(75, 105, 255, 255);  // Mil-Spec
    case 4: return IM_COL32(136, 71, 255, 255);  // Restricted
    case 5: return IM_COL32(211, 44, 230, 255);  // Classified
    case 6: return IM_COL32(235, 75, 75, 255);   // Covert
    case 7: return IM_COL32(228, 174, 57, 255);  // Contraband
    default: return IM_COL32(255, 255, 255, 255); // Default
    }
}

namespace MENU {
    ImFont* myFont = nullptr;

    void LoadFont()
    {
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->Clear();

        ImFontConfig base_cfg;
        base_cfg.OversampleH = 3;
        base_cfg.OversampleV = 3;
        base_cfg.PixelSnapH = true;
        base_cfg.FontDataOwnedByAtlas = false;

        myFont = io.Fonts->AddFontFromMemoryTTF(
            (void*)font_data,
            (int)font_size,
            18.0f,
            &base_cfg,
            io.Fonts->GetGlyphRangesCyrillic()
        );

        if (!myFont)
            return;

        static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

        ImFontConfig icon_cfg;
        icon_cfg.MergeMode = true;
        icon_cfg.PixelSnapH = true;
        icon_cfg.FontDataOwnedByAtlas = false;

        io.Fonts->AddFontFromMemoryTTF(
            (void*)fa_solid_900,
            sizeof(fa_solid_900),
            16.0f,
            &icon_cfg,
            icon_ranges
        );
    }

    void initStyle() {
        static bool styleInit = false;
        if (styleInit) return;

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 10.0f;
        style.FrameRounding = 6.0f;
        style.GrabRounding = 6.0f;
        style.WindowPadding = ImVec2(14, 12);
        style.FramePadding = ImVec2(6, 4);
        style.Alpha = 0.9f;

        ImVec4* colors = style.Colors;
        colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.15f, 1.0f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.15f, 0.18f, 1.0f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.15f, 0.18f, 0.95f);
        colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.32f, 1.0f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.35f, 0.45f, 1.0f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.35f, 0.42f, 0.52f, 1.0f);
        colors[ImGuiCol_Text] = ImVec4(0.92f, 0.92f, 0.94f, 1.0f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.40f, 0.60f, 0.80f, 1.0f);

        static constexpr int bone_indices[19] = { 6, 5, 0, 1, 2, 3, 4, 22, 25, 23, 26, 24, 27, 10, 15, 8, 9, 13, 14 };
        for (int i = 0; i < 19; ++i)
            CFG::AIM::RAGEBOT::m_hitboxes.push_back(bone_indices[i]);
        
        CFG::AIM::RAGEBOT::m_multipointed_hitboxes.push_back(6);

        LoadFont();
        if (myFont) ImGui::GetIO().FontDefault = myFont;

        styleInit = true;

    }

	void onRenders() {
		FEATURES::VISUAL::ESP::onRender();
	}

    void renderUI()
    {
        if (ImGui::BeginTabBar("MainTabs"))
        {
            // MOVEMENT
            if (ImGui::BeginTabItem("Misc"))
            {
                ImGui::Checkbox("Bhop", &CFG::MOVEMENT::isBhopEnabled);
                ImGui::Checkbox("Auto Strafer", &CFG::MOVEMENT::isAutoStraferEnabled);

                ImGui::Checkbox("Viewmodel", &CFG::VISUAL::VIEWMODEL::isViewModelEnabled);

				ImGui::SliderFloat("Viewmodel FOV", &CFG::VISUAL::VIEWMODEL::viewModelFOV, 30.f, 120.f, "%.1f");

				ImGui::SliderFloat("Viewmodel X", &CFG::VISUAL::VIEWMODEL::viewModelX, -4.f, 4.f, "%.1f");
				ImGui::SliderFloat("Viewmodel Y", &CFG::VISUAL::VIEWMODEL::viewModelY, -4.f, 4.f, "%.1f");
				ImGui::SliderFloat("Viewmodel Z", &CFG::VISUAL::VIEWMODEL::viewModelZ, -4.f, 4.f, "%.1f");

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Skins"))
            {
                ImGui::Checkbox("Enable Skins", &CFG::SKINS::isSkinsEnabled);
                ImGui::Text("Knife Changer");
                ImGui::Separator();
                ImGui::Checkbox("Enabled##knife", &CFG::SKINS::knifeChanger.m_enabled);

                if (CFG::SKINS::knifeChanger.m_enabled) {
                    static int last_knife = 0;
                    auto* g_item_schema = ItemSchema::get();

                    if (g_item_schema->is_initialized() && !g_item_schema->knife_names_cstr.empty()) {
                        ImGui::Combo("Knife Model", &CFG::SKINS::knifeChanger.m_knife,
                            g_item_schema->knife_names_cstr.data(),
                            (int)g_item_schema->knife_names_cstr.size());
                    }
                    else {
                        ImGui::TextDisabled("Loading knives...");
                    }

                    uint16_t selected_knife = 0;
                    if (g_item_schema->is_initialized() &&
                        CFG::SKINS::knifeChanger.m_knife < (int)g_item_schema->knives.size()) {
                        selected_knife = g_item_schema->knives[CFG::SKINS::knifeChanger.m_knife].definition_index;
                    }

                    if (last_knife != CFG::SKINS::knifeChanger.m_knife) {
                        CFG::SKINS::knifeChanger.m_paint_kit = 0;
                        last_knife = CFG::SKINS::knifeChanger.m_knife;
                    }

                    if (g_item_schema->is_initialized() && selected_knife > 0) {
                        auto& kits = g_item_schema->item_paint_kits[selected_knife];
                        if (!kits.empty()) {

                            const char* preview_value =
                                (CFG::SKINS::knifeChanger.m_paint_kit < (int)kits.size())
                                ? kits[CFG::SKINS::knifeChanger.m_paint_kit].name.c_str()
                                : "";

                            if (ImGui::BeginCombo("Knife Skin", preview_value)) {
                                for (int i = 0; i < (int)kits.size(); i++) {
                                    const bool is_selected = (CFG::SKINS::knifeChanger.m_paint_kit == i);
                                    ImGui::PushID(i);
                                    if (ImGui::Selectable(kits[i].name.c_str(), is_selected)) {
                                        CFG::SKINS::knifeChanger.m_paint_kit = i;
                                    }

                                    ImU32 kit_color = GetRarityColor(kits[i].rarity);
                                    ImVec2 p_min = ImGui::GetItemRectMin();
                                    ImVec2 p_max = ImGui::GetItemRectMax();
                                    ImGui::GetWindowDrawList()->AddRectFilled(
                                        ImVec2(p_min.x + 1.0f, p_min.y + 1.0f),
                                        ImVec2(p_min.x + 4.0f, p_max.y - 1.0f),
                                        kit_color
                                    );

                                    if (is_selected) ImGui::SetItemDefaultFocus();
                                    ImGui::PopID();
                                }
                                ImGui::EndCombo();
                            }
                        }
                    }

                    static float temp_wear = CFG::SKINS::knifeChanger.m_wear;
                    float item_width = ImGui::CalcItemWidth();
                    float half_width = (item_width - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

                    ImGui::SetNextItemWidth(half_width);
                    if (ImGui::SliderFloat("##Wear", &temp_wear, 0.0f, 1.0f, "%.4f")) {
                        if (!ImGui::IsMouseDown(0))
                            CFG::SKINS::knifeChanger.m_wear = temp_wear;
                    }

                    if (!ImGui::IsItemActive() && temp_wear != CFG::SKINS::knifeChanger.m_wear)
                        CFG::SKINS::knifeChanger.m_wear = temp_wear;

                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(half_width);
                    ImGui::InputInt("Wear / Seed", &CFG::SKINS::knifeChanger.m_seed, 0, 0);

                    ImGui::InputText("Custom Name",
                        CFG::SKINS::knifeChanger.m_custom_name,
                        sizeof(CFG::SKINS::knifeChanger.m_custom_name));
                }



                ImGui::Spacing();
                ImGui::Text("Skin Changer");
                ImGui::Separator();
                ImGui::Checkbox("Enabled##skin", &CFG::SKINS::skinChanger.m_enabled);
                auto* g_item_schema = ItemSchema::get();
                if (CFG::SKINS::skinChanger.m_enabled && g_item_schema->is_initialized()) {
                    if (!g_item_schema->weapon_names_cstr.empty()) {
                        ImGui::Combo("Weapon", &CFG::SKINS::skinChanger.m_selected_weapon,
                            g_item_schema->weapon_names_cstr.data(),
                            (int)g_item_schema->weapon_names_cstr.size());
                    }

                    uint16_t selected_weapon_def = 0;
                    if (CFG::SKINS::skinChanger.m_selected_weapon < (int)g_item_schema->weapons.size()) {
                        selected_weapon_def = g_item_schema->weapons[CFG::SKINS::skinChanger.m_selected_weapon].definition_index;
                    }

                    if (selected_weapon_def > 0) {
                        int config_index = CFG::SKINS::skinChanger.GetConfigIndex(selected_weapon_def);
                        auto& weapon_skin = CFG::SKINS::skinChanger.weapon_skins[config_index];

                        auto& kits = g_item_schema->item_paint_kits[selected_weapon_def];
                        if (!kits.empty()) {
                            const char* preview_value = (weapon_skin.paint_kit < (int)kits.size()) ? kits[weapon_skin.paint_kit].name.c_str() : "";

                            if (ImGui::BeginCombo("Skin##weapon_skin", preview_value)) {
                                for (int i = 0; i < (int)kits.size(); i++) {
                                    const bool is_selected = (weapon_skin.paint_kit == i);
                                    ImGui::PushID(i);
                                    if (ImGui::Selectable(kits[i].name.c_str(), is_selected)) {
                                        weapon_skin.paint_kit = i;
                                    }

                                    ImU32 kit_color = GetRarityColor(kits[i].rarity);
                                    ImVec2 p_min = ImGui::GetItemRectMin();
                                    ImVec2 p_max = ImGui::GetItemRectMax();
                                    ImGui::GetWindowDrawList()->AddRectFilled(
                                        ImVec2(p_min.x + 1.0f, p_min.y + 1.0f),
                                        ImVec2(p_min.x + 4.0f, p_max.y - 1.0f),
                                        kit_color
                                    );

                                    if (is_selected) ImGui::SetItemDefaultFocus();
                                    ImGui::PopID();
                                }
                                ImGui::EndCombo();
                            }
                        }

                        float weapon_item_width = ImGui::CalcItemWidth();
                        float weapon_half_width = (weapon_item_width - ImGui::GetStyle().ItemSpacing.x) * 0.5f;
                        ImGui::SetNextItemWidth(weapon_half_width);
                        ImGui::SliderFloat("##WeaponWear", &weapon_skin.wear, 0.0f, 1.0f, "%.4f");
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(weapon_half_width);
                        ImGui::InputInt("Wear / Seed##weapon", &weapon_skin.seed, 0, 0);
                        ImGui::InputText("Name##weapon_name", weapon_skin.custom_name,
                            sizeof(weapon_skin.custom_name));

                    }
                }







                ImGui::EndTabItem();
            }
            // ESP
            if (ImGui::BeginTabItem("ESP"))
            {
                ImGui::Checkbox("Enable ESP", &CFG::VISUAL::ESP::isEspEnabled);

                const char* items[] =
                {
                    "Box",
                    "Corner",
                    "3D Box"
                };

                ImGui::Combo(
                    "Type",
                    (int*)&CFG::VISUAL::ESP::espType,
                    items,
                    IM_ARRAYSIZE(items)
                );

                ImGui::ColorEdit4("Color", (float*)&CFG::VISUAL::ESP::espColor);

                ImGui::Separator();
                ImGui::Text("Overlay Options:");
                ImGui::Checkbox("Draw Health Bar", &CFG::VISUAL::ESP::drawHealthBar);
                ImGui::Checkbox("Draw Ammo", &CFG::VISUAL::ESP::drawAmmo);
                ImGui::Checkbox("Draw Nickname", &CFG::VISUAL::ESP::drawNickname);

                ImGui::EndTabItem();
            }

            // CHAMS
            if (ImGui::BeginTabItem("Chams"))
            {
                ImGui::Checkbox("Enable Chams", &CFG::VISUAL::CHAMS::isChamsEnabled);

                ImGui::Combo("Material", (int*)&CFG::VISUAL::CHAMS::curMat,
                    "Flat\0Solid\0Bloom\0Glow\0Ghost\0Latex\0\0");

                ImGui::Checkbox("Visible", &CFG::VISUAL::CHAMS::isVisEnabled);
                ImGui::ColorEdit4("Visible Color", (float*)&CFG::VISUAL::CHAMS::chamsVisColor);

                ImGui::Checkbox("Invisible", &CFG::VISUAL::CHAMS::isInvisEnabled);
                ImGui::ColorEdit4("Invisible Color", (float*)&CFG::VISUAL::CHAMS::chamsInvisColor);

                if (ImGui::TreeNode("Chams Arms"))
                {
                    ImGui::Checkbox("Enable Arms Chams", &CFG::VISUAL::CHAMS::isArmsEnabled);

                    if (CFG::VISUAL::CHAMS::isArmsEnabled)
                    {
                        ImGui::ColorEdit4("Arms Color", (float*)&CFG::VISUAL::CHAMS::chamsArmsColor, ImGuiColorEditFlags_NoInputs);

                        ImGui::Combo("Material", (int*)&CFG::VISUAL::CHAMS::curArmsMat,
                            "Flat\0Solid\0Bloom\0Glow\0Ghost\0Latex\0\0");
                    }

                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Chams HUD Weapon"))
                {
                    ImGui::Checkbox("Enable HUD Weapon Chams", &CFG::VISUAL::CHAMS::isHudWeaponEnabled);

                    if (CFG::VISUAL::CHAMS::isHudWeaponEnabled)
                    {
                        ImGui::ColorEdit4("HUD Weapon Color", (float*)&CFG::VISUAL::CHAMS::chamsHudWeaponColor, ImGuiColorEditFlags_NoInputs);

                        ImGui::Combo("Material##hudweapon", (int*)&CFG::VISUAL::CHAMS::curHudWeaponMat,
                            "Flat\0Solid\0Bloom\0Glow\0Ghost\0Latex\0\0");
                    }

                    ImGui::TreePop();
                }

                /*if (ImGui::TreeNode("Chams Weapon"))
                {
                    ImGui::Checkbox("Enable Weapon Chams", &CFG::VISUAL::CHAMS::isWeaponEnabled);

                    if (CFG::VISUAL::CHAMS::isWeaponEnabled)
                    {
                        ImGui::ColorEdit4("Weapon Color", (float*)&CFG::VISUAL::CHAMS::chamsWeaponColor, ImGuiColorEditFlags_NoInputs);

                        ImGui::Combo("Material##weapon", (int*)&CFG::VISUAL::CHAMS::curWeaponMat,
                            "Flat\0Solid\0Bloom\0Glow\0Ghost\0Latex\0\0");
                    }

                    ImGui::TreePop();
                }*/

                ImGui::EndTabItem();
            }

            // WORLD
            if (ImGui::BeginTabItem("World"))
            {
                ImGui::Checkbox("World Enable", &CFG::VISUAL::WORLD::isWorldEnabled);

                ImGui::Checkbox("Skybox", &CFG::VISUAL::WORLD::isSkyboxEnabled);
                ImGui::ColorEdit3("Sky Color", (float*)&CFG::VISUAL::WORLD::skyboxColor);

                ImGui::Separator();
                //ImGui::Text("Custom Skybox Material");
                //ImGui::Checkbox("Enable Custom Skybox", &CFG::VISUAL::WORLD::isCustomSkyboxEnabled);

                if (CFG::VISUAL::WORLD::isCustomSkyboxEnabled)
                {
                    const char* skybox_items[] = {
                        "Default",
                        "Overcast 01",
                        "Day 01"
                    };

                    ImGui::Combo("Skybox Material", &CFG::VISUAL::WORLD::selectedSkybox,
                        skybox_items, IM_ARRAYSIZE(skybox_items));
                }

                ImGui::Checkbox("Smoke", &CFG::VISUAL::WORLD::isSmokeEnabled);
                ImGui::ColorEdit3("Smoke Color", (float*)&CFG::VISUAL::WORLD::smokeColor);

                ImGui::Checkbox("World modulation", &CFG::VISUAL::WORLD::isWorldModulationEnabled);
                ImGui::ColorEdit3("World Color", (float*)&CFG::VISUAL::WORLD::worldModulationColor);

                ImGui::Checkbox("Nightmode", &CFG::VISUAL::WORLD::isNightmodeEnabled);
                ImGui::SliderInt("Nightmode value", &CFG::VISUAL::WORLD::nightmodeValue, 0, 100);

                ImGui::Checkbox("Light modulation", &CFG::VISUAL::WORLD::isLightModulationEnabled);
                ImGui::ColorEdit4("Light color", (float*)&CFG::VISUAL::WORLD::lightModulationColor);

                ImGui::EndTabItem();
            }

            // AIM
            if (ImGui::BeginTabItem("Aim"))
            {
                if (ImGui::CollapsingHeader("Legitbot"))
                {
                    ImGui::Checkbox("Enable", &CFG::AIM::LEGITBOT::isAimEnabled);
                    ImGui::SliderFloat("FOV", &CFG::AIM::LEGITBOT::fov, 0.f, 30.f);
                    ImGui::SliderFloat("Smooth", &CFG::AIM::LEGITBOT::smooth, 1.f, 100.f);
                }

                if (ImGui::CollapsingHeader("Ragebot"))
                {
                    ImGui::Checkbox("Enable", &CFG::AIM::RAGEBOT::isAimEnabled);
                    ImGui::Checkbox("Silent", &CFG::AIM::RAGEBOT::isSilentEnabled);
                    //ImGui::Checkbox("Penetration", &CFG::AIM::RAGEBOT::isPenetrationEnabled);
                    ImGui::Checkbox("Force Shoot", &CFG::AIM::RAGEBOT::isForceShootEnabled);

                    ImGui::SliderInt("Min Damage", &CFG::AIM::RAGEBOT::minDamage, 1, 100);
					ImGui::SliderInt("Hitchance", &CFG::AIM::RAGEBOT::hitChance, 1, 100);
                    ImGui::SliderFloat("FOV", &CFG::AIM::RAGEBOT::fov, 0.f, 180.f);
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }



    bool menu_open = true;
	bool mainActive = false;

	void render() {
       
		onRenders();

        if (menu_open) {
            ImGui::Begin("ImGui Window");
            renderUI();
            ImGui::End();
        }
	}
}