#include "../../../features.h"
#include "../../../../sdk/mesh_sdk.h"
#include "../../../../core/hooks/skybox/updateskybox.h"
#include "../../../../sdk/interface.h"
#include <cstdio>

namespace {
    // Глобальные переменные
    CStrongHandle<CMaterial2> g_custom_skybox_material = {};
    CMaterial2* g_default_skybox_material = nullptr;
    bool g_has_custom_skybox = false;
    int g_last_selected_skybox = -1;

    // KV3 заголовок для материалов
    const char* kv3_header = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
    Shader = "sky.vfx"
    g_flBrightnessExposureBias = 0.000000
    g_flRenderOnlyExposureBias = 0.000000
    g_tSkyTexture = resource:"%s_c"
})";

    // Доступные скайбоксы
    const char* arr_skybox_paths[] = {
        "materials/skybox/cs_italy_s2_skybox_sunset_2_lighting_exr_17a5b8e9.vtex",
        "materials/skybox/jungle_cube_pfm_da6ad36f.vtex",
    };

    // Блокирующая загрузка текстуры скайбокса
    void preload_skybox_texture(CResourceSystem* res_sys, const char* texture_path) {
        if (!res_sys || !texture_path)
            return;

        // Используем 'xetv' тег для vtex ресурсов
        CBufferString buffer_vtex(texture_path, 'xetv');
        res_sys->BlockingLoadResourceByName(&buffer_vtex);
    }

    // Создание материала скайбокса
    CStrongHandle<CMaterial2> create_skybox_material(CResourceSystem* res_sys, const char* texture_path) {
        if (!res_sys || !texture_path)
            return {};

        // Сначала предзагружаем текстуру
        preload_skybox_texture(res_sys, texture_path);

        // Форматируем KV3 материал
        char material_buffer[2048];
        int written = sprintf_s(material_buffer, sizeof(material_buffer), kv3_header, texture_path);
        
        if (written <= 0)
            return {};

        // Создаём материал через compile функцию из mesh_sdk.h
        return CResourceSystem::createMaterial("custom_skybox_material", material_buffer);
    }
}

void FEATURES::VISUAL::WORLD::SKYBOX::onUpdateSkyBox(__int64 pObjectDesc, __int64 a2, __int64 pSceneData, int iDataCount, int a5, __int64 a6, __int64 a7)
{
    if (iDataCount > 0)
    {
        uintptr_t skybox_data_addr = 0x68 * iDataCount + pSceneData - 0x50;
        auto SkyBoxData = *(CSkyBoxSceneObject**)(skybox_data_addr);
        if (SkyBoxData)
        {
            // Применяем цвет скайбокса если включена регулировка цвета
            if (CFG::VISUAL::WORLD::isSkyboxEnabled && CFG::VISUAL::WORLD::isWorldEnabled)
                SkyBoxData->skyColor = CFG::VISUAL::WORLD::skyboxColor;
            else
                SkyBoxData->skyColor = { 1.f, 1.f, 1.f };

            // Применяем кастомный материал если он загружен
            if (g_has_custom_skybox && g_custom_skybox_material) {
                CMaterial2* mat = g_custom_skybox_material;
                if (SkyBoxData->mat && mat) {
                    *SkyBoxData->mat = mat;
                }
            }
        }
    }

    oUpdateSkyBox(pObjectDesc, a2, pSceneData, iDataCount, a5, a6, a7);
}

void FEATURES::VISUAL::WORLD::SKYBOX::init_on_level_load() {
    auto res_sys = Interface::GetCResourceSystem();
    if (!res_sys)
        return;

    g_has_custom_skybox = false;
    g_custom_skybox_material = {};
    g_default_skybox_material = nullptr;
    g_last_selected_skybox = -1;

    printf("[Skybox] Инициализация на загрузке уровня\n");
}

void FEATURES::VISUAL::WORLD::SKYBOX::change_skybox_material() {
    auto res_sys = Interface::GetCResourceSystem();
    auto engine = Interface::GetIVEngineToClient();
    
    if (!res_sys || !engine)
        return;

    // Отключаем кастомный скайбокс если игра не в игре
    if (!engine->IsConnected() || !engine->IsInGame()) {
        if (g_has_custom_skybox) {
            g_has_custom_skybox = false;
            g_custom_skybox_material = {};
        }
        return;
    }

    if (!CFG::VISUAL::WORLD::isCustomSkyboxEnabled) {
        if (g_has_custom_skybox) {
            g_has_custom_skybox = false;
            g_custom_skybox_material = {};
        }
        return;
    }

    int current_idx = CFG::VISUAL::WORLD::selectedSkybox;

    // Проверяем, изменился ли выбранный скайбокс
    if (g_last_selected_skybox == current_idx)
        return;

    g_last_selected_skybox = current_idx;

    // Индекс 0 = дефолтный скайбокс
    if (current_idx == 0) {
        g_has_custom_skybox = false;
        g_custom_skybox_material = {};
        printf("[Skybox] Восстановлен дефолтный скайбокс\n");
        return;
    }

    // Проверяем границы массива
    if (current_idx < 1 || current_idx > 2) {
        printf("[Skybox] Неверный индекс скайбокса: %d\n", current_idx);
        return;
    }

    // Получаем путь к текстуре
    const char* texture_path = arr_skybox_paths[current_idx - 1];

    // Создаём материал
    auto new_material = create_skybox_material(res_sys, texture_path);
    
    if (new_material) {
        g_custom_skybox_material = new_material;
        g_has_custom_skybox = true;
        printf("[Skybox] Материал изменён: %s\n", texture_path);
    } else {
        printf("[Skybox] Ошибка при создании материала для: %s\n", texture_path);
        g_has_custom_skybox = false;
        g_custom_skybox_material = {};
    }
}

void FEATURES::VISUAL::WORLD::SKYBOX::restore_default_skybox() {
    if (!g_default_skybox_material)
        return;

    printf("[Skybox] Восстановление стандартного скайбокса\n");
    g_has_custom_skybox = false;
    g_custom_skybox_material = {};
}