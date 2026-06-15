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
inline static LoadedMaterial m_CurrentMaterialArmsInvisible;
inline static LoadedMaterial m_CurrentMaterialHudWeapon;
inline static LoadedMaterial m_CurrentMaterialHudWeaponInvisible;
inline static LoadedMaterial m_CurrentMaterialWeapon;
inline static LoadedMaterial m_CurrentMaterialWeaponInvisible;

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
        m_CurrentMaterialArmsInvisible.mat = solidInvis;
        break;

    case ChamsMat::FLAT:
        m_CurrentMaterialArms.mat = flatVis;
        m_CurrentMaterialArmsInvisible.mat = flatInvis;
        break;

    case ChamsMat::LATEX:
        m_CurrentMaterialArms.mat = latexVis;
        m_CurrentMaterialArmsInvisible.mat = latexInvis;
        break;

    case ChamsMat::BLOOM:
        m_CurrentMaterialArms.mat = bloomVis;
        m_CurrentMaterialArmsInvisible.mat = bloomInvis;
        break;

    case ChamsMat::GLOW:
        m_CurrentMaterialArms.mat = glowVis;
        m_CurrentMaterialArmsInvisible.mat = glowInvis;
        break;

    case ChamsMat::GHOST:
        m_CurrentMaterialArms.mat = ghostVis;
        m_CurrentMaterialArmsInvisible.mat = ghostInvis;
        break;

    default:
        m_CurrentMaterialArms.mat = flatVis;
        m_CurrentMaterialArmsInvisible.mat = flatInvis;
        break;
    }

    switch (CFG::VISUAL::CHAMS::curHudWeaponMat)
    {
    case ChamsMat::SOLID:
        m_CurrentMaterialHudWeapon.mat = solidVis;
        m_CurrentMaterialHudWeaponInvisible.mat = solidInvis;
        break;

    case ChamsMat::FLAT:
        m_CurrentMaterialHudWeapon.mat = flatVis;
        m_CurrentMaterialHudWeaponInvisible.mat = flatInvis;
        break;

    case ChamsMat::LATEX:
        m_CurrentMaterialHudWeapon.mat = latexVis;
        m_CurrentMaterialHudWeaponInvisible.mat = latexInvis;
        break;

    case ChamsMat::BLOOM:
        m_CurrentMaterialHudWeapon.mat = bloomVis;
        m_CurrentMaterialHudWeaponInvisible.mat = bloomInvis;
        break;

    case ChamsMat::GLOW:
        m_CurrentMaterialHudWeapon.mat = glowVis;
        m_CurrentMaterialHudWeaponInvisible.mat = glowInvis;
        break;

    case ChamsMat::GHOST:
        m_CurrentMaterialHudWeapon.mat = ghostVis;
        m_CurrentMaterialHudWeaponInvisible.mat = ghostInvis;
        break;

    default:
        m_CurrentMaterialHudWeapon.mat = flatVis;
        m_CurrentMaterialHudWeaponInvisible.mat = flatInvis;
        break;
    }

    switch (CFG::VISUAL::CHAMS::curWeaponMat)
    {
    case ChamsMat::SOLID:
        m_CurrentMaterialWeapon.mat = solidVis;
        m_CurrentMaterialWeaponInvisible.mat = solidInvis;
        break;

    case ChamsMat::FLAT:
        m_CurrentMaterialWeapon.mat = flatVis;
        m_CurrentMaterialWeaponInvisible.mat = flatInvis;
        break;

    case ChamsMat::LATEX:
        m_CurrentMaterialWeapon.mat = latexVis;
        m_CurrentMaterialWeaponInvisible.mat = latexInvis;
        break;

    case ChamsMat::BLOOM:
        m_CurrentMaterialWeapon.mat = bloomVis;
        m_CurrentMaterialWeaponInvisible.mat = bloomInvis;
        break;

    case ChamsMat::GLOW:
        m_CurrentMaterialWeapon.mat = glowVis;
        m_CurrentMaterialWeaponInvisible.mat = glowInvis;
        break;

    case ChamsMat::GHOST:
        m_CurrentMaterialWeapon.mat = ghostVis;
        m_CurrentMaterialWeaponInvisible.mat = ghostInvis;
        break;

    default:
        m_CurrentMaterialWeapon.mat = flatVis;
        m_CurrentMaterialWeaponInvisible.mat = flatInvis;
        break;
    }
}

void FEATURES::VISUAL::CHAMS::onDrawArray(CSceneAnimatableObjectDesc* desc, void* a2, CMeshDrawPrimitive_t* mesh_draw, int a4, void* a5, void* a6, void* a7)
{
    if (!CFG::VISUAL::CHAMS::isChamsEnabled)
        return;

    // --- ���������: �������� �� ������ ��������� ������� ---
    if (!mesh_draw->pSceneAnimatableObject)
        return;


    // --- ���������� ������� ���������� (�� �������) ---
    updateCurrMat();

    // ��������� ������������ �������� � ����
    CMaterial2* old_mat1 = mesh_draw->pMaterial;
    CMaterial2* old_mat2 = mesh_draw->pMaterial2;
    Color_t old_col = mesh_draw->colValue;

    // --- ��������� ���� (���������� Z-�����) ---
    if (CFG::VISUAL::CHAMS::isInvisEnabled && m_CurrentMaterialInvisible.mat.pBinding)
    {
        mesh_draw->pMaterial = m_CurrentMaterialInvisible.mat;
        mesh_draw->pMaterial2 = m_CurrentMaterialInvisible.mat;
        // ���������� �������������� ImColor -> Color_t
        ImColor invCol = CFG::VISUAL::CHAMS::chamsInvisColor;
        mesh_draw->colValue = Color_t(invCol.Value.x, invCol.Value.y, invCol.Value.z, invCol.Value.w);
        oDrawArray(desc, a2, mesh_draw, a4, a5, a6, a7);
    }

    // --- ������� ���� (� ������ Z-������) ---
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
        // ���� ������� ���� �������� � ������ ������������ ��������
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

    // Track primitive counts before/after each call
    int invis_start = 0, invis_end = 0;
    int vis_start = 0, vis_end = 0;

    // Invisible Chams (rendered first, behind visible)
    if (CFG::VISUAL::CHAMS::isInvisEnabled && m_CurrentMaterialInvisible.mat.pBinding)
    {
        uint32_t* flags = reinterpret_cast<uint32_t*>(reinterpret_cast<char*>(object) + 0x78);
        uint32_t old_flags = *flags;
        *flags &= ~(1u << 3);

        invis_start = render_buf->m_start_primitive;
        oGeneratePrimitives(thisptr, object, a3, render_buf);
        invis_end = render_buf->m_start_primitive;

        *flags = old_flags;

        for (int i = invis_start; i < invis_end; ++i)
        {
            auto mesh = &render_buf->m_out[i];
            if (!mesh || !mesh->pSceneAnimatableObject)
                continue;

            if (type == ChamsObjectType::PLAYER)
            {
                ApplyChams(mesh, m_CurrentMaterialInvisible.mat, ImColorToColor(CFG::VISUAL::CHAMS::chamsInvisColor));
                processed = true;
            }
            else if (type == ChamsObjectType::HUDARMS && CFG::VISUAL::CHAMS::isArmsEnabled)
            {
                ApplyChams(mesh, m_CurrentMaterialArmsInvisible.mat, ImColorToColor(CFG::VISUAL::CHAMS::chamsArmsColor));
                processed = true;
            }
            else if (type == ChamsObjectType::HUDWEAPON && CFG::VISUAL::CHAMS::isHudWeaponEnabled)
            {
                ApplyChams(mesh, m_CurrentMaterialHudWeaponInvisible.mat, ImColorToColor(CFG::VISUAL::CHAMS::chamsHudWeaponColor));
                processed = true;
            }
            else if (type == ChamsObjectType::WEAPONS && CFG::VISUAL::CHAMS::isWeaponEnabled)
            {
                ApplyChams(mesh, m_CurrentMaterialWeaponInvisible.mat, ImColorToColor(CFG::VISUAL::CHAMS::chamsWeaponColor));
                processed = true;
            }
        }
    }

    // Visible Chams (rendered second, on top of invisible)
    if (CFG::VISUAL::CHAMS::isVisEnabled && m_CurrentMaterialVisible.mat.pBinding)
    {
        vis_start = render_buf->m_start_primitive;
        oGeneratePrimitives(thisptr, object, a3, render_buf);
        vis_end = render_buf->m_start_primitive;

        for (int i = vis_start; i < vis_end; ++i)
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
            else if (type == ChamsObjectType::HUDWEAPON && CFG::VISUAL::CHAMS::isHudWeaponEnabled)
            {
                ApplyChams(mesh, m_CurrentMaterialHudWeapon.mat, ImColorToColor(CFG::VISUAL::CHAMS::chamsHudWeaponColor));
                processed = true;
            }
            else if (type == ChamsObjectType::WEAPONS && CFG::VISUAL::CHAMS::isWeaponEnabled)
            {
                ApplyChams(mesh, m_CurrentMaterialWeapon.mat, ImColorToColor(CFG::VISUAL::CHAMS::chamsWeaponColor));
                processed = true;
            }
        }
    }

    if (!processed)
        return false;

    return true;
}