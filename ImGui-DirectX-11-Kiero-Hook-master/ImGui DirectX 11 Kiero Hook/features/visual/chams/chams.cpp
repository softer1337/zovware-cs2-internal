#include "../../features.h"
#include "../../../sdk/materials.h"
#include "../../../sdk/schema.h"
#include "../../../core/hooks/drawarray/drawarray.h"
#include "../../../core/hooks/generateprimitives/generateprimitives.h"
#include <mutex>
#include "../../../sdk/entitysystem.hpp"
#include "../../../sdk/hash.hpp"

inline static LoadedMaterial m_CurrentMaterialInvisible;
inline static LoadedMaterial m_CurrentMaterialVisible;

inline static LoadedMaterial m_CurrentMaterialArms;

inline static std::mutex m_Mutex;


void updateCurrMat()
{
    std::lock_guard<std::mutex> lock(m_Mutex);

    switch (CFG::VISUAL::CHAMS::curMat)
    {
    case ChamsMat::SOLID:
        m_CurrentMaterialVisible.mat = solidVis;
        m_CurrentMaterialInvisible.mat = solidInvis;
        break;

    case ChamsMat::FLAT:
        m_CurrentMaterialVisible.mat = flatVis;
        m_CurrentMaterialInvisible.mat = flatInvis;
        break;

    case ChamsMat::LATEX:
        m_CurrentMaterialVisible.mat = latexVis;
        m_CurrentMaterialInvisible.mat = latexInvis;
        break;

    case ChamsMat::BLOOM:
        m_CurrentMaterialVisible.mat = bloomVis;
        m_CurrentMaterialInvisible.mat = bloomInvis;
        break;

    case ChamsMat::GLOW:
        m_CurrentMaterialVisible.mat = glowVis;
        m_CurrentMaterialInvisible.mat = glowInvis;
        break;

    case ChamsMat::GHOST:
        m_CurrentMaterialVisible.mat = ghostVis;
        m_CurrentMaterialInvisible.mat = ghostInvis;
        break;

    default:
        m_CurrentMaterialVisible.mat = flatVis;
        m_CurrentMaterialInvisible.mat = flatInvis;
        break;
    }

    switch (CFG::VISUAL::CHAMS::curArmsMat)
    {
    case ChamsMat::SOLID:
        m_CurrentMaterialArms.mat = solidVis;
        break;

    case ChamsMat::FLAT:
        m_CurrentMaterialArms.mat = flatVis;
        break;

    case ChamsMat::LATEX:
        m_CurrentMaterialArms.mat = latexVis;
        break;

    case ChamsMat::BLOOM:
        m_CurrentMaterialArms.mat = bloomVis;
        break;

    case ChamsMat::GLOW:
        m_CurrentMaterialArms.mat = glowVis;
        break;

    case ChamsMat::GHOST:
        m_CurrentMaterialArms.mat = ghostVis;
        break;

    default:
        m_CurrentMaterialArms.mat = flatVis;
        break;
    }
}

void FEATURES::VISUAL::CHAMS::onDrawArray(CSceneAnimatableObjectDesc* desc, void* a2, CMeshDrawPrimitive_t* mesh_draw, int a4, void* a5, void* a6, void* a7)
{
    if (!CFG::VISUAL::CHAMS::isChamsEnabled)
        return;

    // --- Валидация: является ли объект вражеским игроком ---
    if (!mesh_draw->pSceneAnimatableObject)
        return;


    // --- Обновление текущих материалов (из конфига) ---
    updateCurrMat();

    // Сохраняем оригинальные материал и цвет
    CMaterial2* old_mat1 = mesh_draw->pMaterial;
    CMaterial2* old_mat2 = mesh_draw->pMaterial2;
    Color_t old_col = mesh_draw->colValue;

    // --- Невидимый слой (игнорирует Z-буфер) ---
    if (CFG::VISUAL::CHAMS::isInvisEnabled && m_CurrentMaterialInvisible.mat.pBinding)
    {
        mesh_draw->pMaterial = m_CurrentMaterialInvisible.mat;
        mesh_draw->pMaterial2 = m_CurrentMaterialInvisible.mat;
        // Правильное преобразование ImColor -> Color_t
        ImColor invCol = CFG::VISUAL::CHAMS::chamsInvisColor;
        mesh_draw->colValue = Color_t(invCol.Value.x, invCol.Value.y, invCol.Value.z, invCol.Value.w);
        oDrawArray(desc, a2, mesh_draw, a4, a5, a6, a7);
    }

    // --- Видимый слой (с учётом Z-буфера) ---
    if (CFG::VISUAL::CHAMS::isVisEnabled && m_CurrentMaterialVisible.mat.pBinding)
    {
        mesh_draw->pMaterial = m_CurrentMaterialVisible.mat;
        mesh_draw->pMaterial2 = m_CurrentMaterialVisible.mat;
        ImColor visCol = CFG::VISUAL::CHAMS::chamsVisColor;
        mesh_draw->colValue = Color_t(visCol.Value.x, visCol.Value.y, visCol.Value.z, visCol.Value.w);
        oDrawArray(desc, a2, mesh_draw, a4, a5, a6, a7);
    }
    else
    {
        // Если видимый слой отключён — рисуем оригинальный материал
        mesh_draw->pMaterial = old_mat1;
        mesh_draw->pMaterial2 = old_mat2;
        mesh_draw->colValue = old_col;
        oDrawArray(desc, a2, mesh_draw, a4, a5, a6, a7);
    }
}

const char* GetName(CSceneAnimatableObject* object) {
    if (!object->hOwner.is_valid())
        return "";

    CSchemaClassInfo* info = ((C_BaseEntity*)Interface::GetEntitySystem()->getBaseBntity(object->hOwner.get_entry_index()))->getSchemaClassInfo();
    if (!info)
        return "";

    return info->GetName();
}

bool IsPlayer(CSceneAnimatableObject* object)
{
    static auto hash = fnv1a::hash_64("C_CSPlayerPawn");
    if (fnv1a::hash_64(GetName(object)) == hash)
        return true;

    return false;
}

void ApplyChams(CMeshDrawPrimitive_t* mesh, CStrongHandle<CMaterial2> mat, Color_t  color) {
    mesh->pMaterial = mat;
    mesh->pMaterial2 = mat;
    mesh->colValue = color;
}

enum ChamsObjectType {
    PLAYER = 1,
    HUDARMS = 2,
    HUDWEAPON = 3,
    WEAPONS = 4,
    UNKNOWN = 999
};


bool FEATURES::VISUAL::CHAMS::onGeneratePrimitives(CSceneAnimatableObject* object, void* a3, CMeshPrimitiveOutputBuffer* render_buf, void* thisptr)
{
    if (!CFG::VISUAL::CHAMS::isChamsEnabled)
        return false;

    const char* name = GetName(object);
    uint64_t hashed_name = fnv1a::hash_64(name);

    ChamsObjectType type = ChamsObjectType::UNKNOWN;

    if (hashed_name == fnv1a::hash_64("C_CSPlayerPawn"))
        type = ChamsObjectType::PLAYER;
    else if (hashed_name == fnv1a::hash_64("C_CS2HudModelArms"))
        type = ChamsObjectType::HUDARMS;
    else if (hashed_name == fnv1a::hash_64("C_CS2HudModelWeapon"))
        type = ChamsObjectType::HUDWEAPON;
    else if (hashed_name == fnv1a::hash_64("C_CSWeaponBase"))
        type = ChamsObjectType::WEAPONS;
    else
        return false;

    updateCurrMat();
    bool processed = false;

    // Invisible Chams
    if (CFG::VISUAL::CHAMS::isInvisEnabled && m_CurrentMaterialInvisible.mat.pBinding)
    {
        uint32_t* flags = reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(object) + 0x78);
        uint32_t old_flags = *flags;
        *flags &= ~(1u << 3);

        int prev = render_buf->m_start_primitive;
        oGeneratePrimitives(thisptr, object, a3, render_buf);
        int new_count = render_buf->m_start_primitive;

        *flags = old_flags;

        for (int i = prev; i < new_count; ++i)
        {
            auto mesh = &render_buf->m_out[i];
            if (!mesh || !mesh->pSceneAnimatableObject)
                continue;

            if (type == ChamsObjectType::PLAYER)
            {
                ApplyChams(mesh, m_CurrentMaterialInvisible.mat, ImColorToColor(CFG::VISUAL::CHAMS::chamsInvisColor));
                processed = true;
            }
        }
    }

    // Visible Chams
    if (CFG::VISUAL::CHAMS::isVisEnabled && m_CurrentMaterialVisible.mat.pBinding)
    {
        int prev = render_buf->m_start_primitive;
        oGeneratePrimitives(thisptr, object, a3, render_buf);
        int new_count = render_buf->m_start_primitive;

        for (int i = prev; i < new_count; ++i)
        {
            auto mesh = &render_buf->m_out[i];
            if (!mesh || !mesh->pSceneAnimatableObject)
                continue;

            if (type == ChamsObjectType::PLAYER)
            {
                ApplyChams(mesh, m_CurrentMaterialVisible.mat, ImColorToColor(CFG::VISUAL::CHAMS::chamsVisColor));
                processed = true;
            }
            else if (type == ChamsObjectType::HUDARMS && CFG::VISUAL::CHAMS::isArmsEnabled)
            {
                ApplyChams(mesh, m_CurrentMaterialArms.mat, ImColorToColor(CFG::VISUAL::CHAMS::chamsArmsColor));
                processed = true;
            }
        }
    }

    if (!processed)
        return false;

    return true;
}