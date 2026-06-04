#include "../../features.h"
#include "../../../sdk/materials.h"
#include "../../../sdk/schema.h"
#include "../../../core/hooks/drawarray/drawarray.h"
#include "../../../core/hooks/generateprimitives/generateprimitives.h"
#include <mutex>

inline static LoadedMaterial m_CurrentMaterialInvisible;
inline static LoadedMaterial m_CurrentMaterialVisible;

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
bool IsValidPlayer(CSceneAnimatableObject* object)
{
    uintptr_t entityList = *reinterpret_cast<uintptr_t*>(MEM::GetClient() + offsets::dwEntityList);
    if (!entityList) return false;

    uintptr_t listEntry = MEM::read<uintptr_t>(entityList + 0x8 * ((object->hOwner & 0x7FFF) >> 9) + 16);
    if (!listEntry) return false;

    uintptr_t ppEntity = MEM::read<uintptr_t>(listEntry + 112 * (object->hOwner & 0x1FF));
    if (!ppEntity) return false;

    int health = MEM::read<int>(ppEntity + offsets::m_iHealth);
    if (health <= 0 || health > 100) return false;

    int team = MEM::read<int>(ppEntity + offsets::m_iTeamNum);
    if (team == MEM::read<int>(MEM::GetClient() + offsets::dwLocalPlayerPawn)) return false;

    return true;
}

bool FEATURES::VISUAL::CHAMS::onGeneratePrimitives(CSceneAnimatableObject* object, void* a3, CMeshPrimitiveOutputBuffer* render_buf, void* thisptr)
{
    if (!IsValidPlayer(object) || !CFG::VISUAL::CHAMS::isChamsEnabled)
        return false;

    updateCurrMat();
    bool processed = false;

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
            mesh->pMaterial = m_CurrentMaterialInvisible.mat;
            mesh->pMaterial2 = m_CurrentMaterialInvisible.mat;
            mesh->colValue = ImColorToColor(CFG::VISUAL::CHAMS::chamsInvisColor);
        }
        processed = true;
    }

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
            mesh->pMaterial = m_CurrentMaterialVisible.mat;
            mesh->pMaterial2 = m_CurrentMaterialVisible.mat;
            mesh->colValue = ImColorToColor(CFG::VISUAL::CHAMS::chamsVisColor);
        }
        processed = true;
    }

    if (!processed)
    {
        oGeneratePrimitives(thisptr, object, a3, render_buf);
        return false;
    }

    return true;
}