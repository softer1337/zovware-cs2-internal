#include "penetration.h"
#include "../../../sdk/ctx.hpp"
#include "../../../sdk/cvar.hpp"

void c_penetration::update_local_ctx() {

    auto ptr = context->activeWeaponData;
    if (!ptr || !context->localPawn)
        return;

    m_local_context.m_armor_ratio = ptr->m_flArmorRatio();
    m_local_context.m_headshot_multiplier = ptr->m_flHeadshotMultiplier();
    m_local_context.m_damage = ptr->m_nDamage();
    m_local_context.m_penetration = ptr->m_flPenetration();
    m_local_context.m_range_mod = ptr->m_flRangeModifier();
    m_local_context.m_local_team = context->localPawn->m_iTeamNum();
}

void c_penetration::player_context_t::fill(C_CSPlayerPawn* pawn) {
    static auto mp_damage_scale_ct_head = Interface::GetConVars()->find_by_name("mp_damage_scale_ct_head");
    static auto mp_damage_scale_t_head = Interface::GetConVars()->find_by_name("mp_damage_scale_t_head");
    static auto mp_damage_scale_ct_body = Interface::GetConVars()->find_by_name("mp_damage_scale_ct_body");
    static auto mp_damage_scale_t_body = Interface::GetConVars()->find_by_name("mp_damage_scale_t_body");

    m_pawn = pawn;
    m_skeleton = pawn->m_pGameSceneNode()->get_skeleton_instace();

    const bool is_ct = pawn->m_iTeamNum() == 3;
    const bool is_t = pawn->m_iTeamNum() == 2;

    m_head_scale = is_ct ? mp_damage_scale_ct_head->get_float() : mp_damage_scale_t_head->get_float();
    m_body_scale = is_ct ? mp_damage_scale_ct_body->get_float() : mp_damage_scale_t_body->get_float();

    m_armor_value = static_cast<float>(pawn->m_ArmorValue());

    auto item_service = pawn->m_pItemServices();

    m_has_heavy_armor = item_service->m_bHasHeavyArmor();
    m_has_helmet = item_service->m_bHasHelmet();

    if (m_has_heavy_armor)
        m_head_scale *= 0.5f;

    m_head_scale *= m_local_context.m_headshot_multiplier;
    m_stomach_scale = 1.25f * m_body_scale;
    m_legs_scale = 0.75f * m_body_scale;

    m_armor_ratio = m_local_context.m_armor_ratio * 0.5f;
    m_armor_bonus = 0.5f;
    m_heavy_armor_bonus = 1.0f;

    if (m_has_heavy_armor) {
        m_armor_ratio *= 0.20f;
        m_armor_bonus = 0.33f;
        m_heavy_armor_bonus = 0.25f;
    }

    m_hitgroup_scale[HITGROUP_HEAD] = m_head_scale;
    m_hitgroup_scale[HITGROUP_CHEST] = m_body_scale;
    m_hitgroup_scale[HITGROUP_LEFTARM] = m_body_scale;
    m_hitgroup_scale[HITGROUP_RIGHTARM] = m_body_scale;
    m_hitgroup_scale[HITGROUP_NECK] = m_body_scale;
    m_hitgroup_scale[HITGROUP_STOMACH] = m_stomach_scale;
    m_hitgroup_scale[HITGROUP_LEFTLEG] = m_legs_scale;
    m_hitgroup_scale[HITGROUP_RIGHTLEG] = m_legs_scale;

    for (int i = 0; i < 8; ++i)
        m_hitgroup_has_armor[i] = false;

    if (m_armor_value > 0.f) {
        m_hitgroup_has_armor[HITGROUP_CHEST] = true;
        m_hitgroup_has_armor[HITGROUP_LEFTARM] = true;
        m_hitgroup_has_armor[HITGROUP_RIGHTARM] = true;
        m_hitgroup_has_armor[HITGROUP_NECK] = true;
        m_hitgroup_has_armor[HITGROUP_STOMACH] = true;
        m_hitgroup_has_armor[HITGROUP_HEAD] = m_has_helmet;
    }
}

int c_penetration::player_context_t::scale_damage(const int& hitgroup, float damage) {
    float original_damage = damage;

    damage *= m_hitgroup_scale[hitgroup];

    if (!m_hitgroup_has_armor[hitgroup])
        return static_cast<int>(damage);

    float damage_to_health = damage * m_armor_ratio;
    float damage_to_armor = (damage - damage_to_health) * (m_heavy_armor_bonus * m_armor_bonus);

    if (damage_to_armor > m_armor_value)
        damage_to_health = damage - m_armor_value / m_armor_bonus;

    return static_cast<int>(damage_to_health);
}



bool c_penetration::player_context_t::fire_bullet(Vec3 start, Vec3& end, C_CSPlayerPawn* target, HandleBulletPenetrationData_t& data) {
    Vec3 direction = end - start;
    direction.normalize();

    TraceData_t trace_data;
    Interface::GetTraceManager()->InitTraceData(&trace_data);

    // Aim the damage trace straight at the requested point. Previously a random spread
    // offset was baked into the direction, which pushed the ray off the hitbox and made
    // fire_bullet under-report or miss damage entirely. Spread is the game's runtime
    // randomness — it must NOT be part of the ragebot's damage estimate.
    Vec3 delta = direction * context->activeWeaponData->m_flRange();
    Vec3 end_pos = start + delta;

    trace_data.m_start = start;
    trace_data.m_end = end_pos;

    Trace_Filter_t filter;
    Interface::GetTraceManager()->InitializeTraceFilter(&filter, context->localPawn, 0x1C300B, 3, 15);
    filter.m_ptr4 |= 2u;
    filter.m_ptr[0] |= 0x4000000000uLL;

    Interface::GetTraceManager()->CreateTrace(&trace_data, start, delta, &filter, 4);

    Interface::GetTraceManager()->DamageToPoint(&trace_data, context->activeWeaponData->m_nDamage(), context->activeWeaponData->m_flPenetration(), context->activeWeaponData->m_flRangeModifier(), context->localPawn->m_iTeamNum());

    for (int i = 0; i < trace_data.m_surfaces_count; i++) {
        c_trace_info& info = trace_data.m_trace_info[i];

        CGameTrace trace;
        Interface::GetTraceManager()->GetTraceInfo(&trace_data, &trace, info.m_distance, &trace_data.m_trace_segments_ptr[(info.m_handle.to_int() >> 16) & 0xFFFF]);

        if (trace.m_pHitEntity && trace.m_pHitEntity == target) {
            data.damage = scale_damage(trace.m_p_hitbox_data->m_hitgroup, info.m_damage);
            return true;
        }
    }

    return false;
}