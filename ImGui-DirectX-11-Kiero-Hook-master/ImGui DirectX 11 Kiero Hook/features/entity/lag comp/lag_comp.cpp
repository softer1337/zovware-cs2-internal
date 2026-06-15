#include "lag_comp.h"
#include "../../../sdk/globalvars.hpp"
#include "../../../sdk/ctx.hpp"
#include "../../../sdk/cvar.hpp"
#include "../../../sdk/netclient.hpp"
#include "timestamp/timestamp.h"
#include "../entity_cache.hpp"
#include "../../cfg.hpp"
#include "../../../sdk/input/usercmd.h"

bool lag_record_t::should_skip_interpolation()
{
    if (m_pawn->m_nNoInterpolationTick() == Interface::GetGlobalVars()->m_tick_count)
        return true;

    return false;
}

void lag_record_t::try_adjust_velocity()
{
    if (should_skip_interpolation())
    {
        if (m_pawn->m_bSimulationTimeChanged())
        {
            float sim_time_diff = m_pawn->m_flSimulationTime() - m_pawn->m_flOldSimulationTime();
            if (sim_time_diff > 0.0001f)
            {
                float inv_dt = 1.0f / sim_time_diff;

                Vec3 current_origin = m_game_scene_node->m_vecOrigin();
                Vec3 old_origin = m_pawn->m_vOldOrigin();
                Vec3 velocity = (current_origin - old_origin) * inv_dt;

                m_pawn->set_velocity(&velocity);
            }
        }
    }
}

void store_hitboxes(lag_record_t* record, c_penetration::player_context_t& player_ctx) {
    if (!record || !record->m_skeleton_instance)
        return;

    c_model* model = record->m_skeleton_instance->m_modelState().m_model_handle;
    if (!model) {
        return;
    }

    if (model->m_rendermesh_count <= 0 || !model->m_render_meshes) {
        return;
    }

    auto render_meshes = model->m_render_meshes->m_meshes;
    if (!render_meshes) {
        return;
    }

    auto hitbox_sets = render_meshes[0].m_hitbox_sets;
    if (!hitbox_sets || hitbox_sets[0].m_hitbox_count <= 0) {
        return;
    }

    auto hitbox_arr = hitbox_sets[0].m_hitbox;
    if (!hitbox_arr) {
        return;
    }

    auto& bones = record->m_bones;
    int pointscale = 100; // CFG:: scale

    static constexpr int bone_indices[19] = { 6, 5, 0, 1, 2, 3, 4, 22, 25, 23, 26, 24, 27, 10, 15, 8, 9, 13, 14 };
    for (int i = 0; i < 19; ++i)
        record->m_all_hitboxes.emplace_back(construct_hitbox_data(hitbox_arr[i], bones[bone_indices[i]], false, pointscale));

    auto& store_hitboxes = record->m_rage_hitboxes;

    auto& mp_setting = CFG::AIM::RAGEBOT::m_multipointed_hitboxes;
    int dmg_threshold = CFG::AIM::RAGEBOT::minDamage > 100 ? record->m_pawn->m_iHealth() + CFG::AIM::RAGEBOT::minDamage - 100 : CFG::AIM::RAGEBOT::minDamage;
    //float weapon_damage = context->activeWeaponData ? context->activeWeaponData->m_nDamage() : 100.f;
    float weapon_damage = 100.f;

    for (int menu_hitbox : CFG::AIM::RAGEBOT::m_hitboxes) {
        bool is_multipointed = std::find(CFG::AIM::RAGEBOT::m_multipointed_hitboxes.begin(), CFG::AIM::RAGEBOT::m_multipointed_hitboxes.end(), menu_hitbox) != CFG::AIM::RAGEBOT::m_multipointed_hitboxes.end();

        switch (menu_hitbox) {
        case 0: // head
            if (player_ctx.scale_damage(HITGROUP_HEAD, weapon_damage) >= dmg_threshold) {
                record->m_all_hitboxes[0].m_multipoint = is_multipointed;
                store_hitboxes.emplace_back(record->m_all_hitboxes[0]);
            }
            break;
        case 1: // upper chest
            if (player_ctx.scale_damage(HITGROUP_CHEST, weapon_damage) >= dmg_threshold) {
                record->m_all_hitboxes[6].m_multipoint = is_multipointed;
                store_hitboxes.emplace_back(record->m_all_hitboxes[6]);
            }
            break;
        case 2: // chest
            if (player_ctx.scale_damage(HITGROUP_CHEST, weapon_damage) >= dmg_threshold) {
                record->m_all_hitboxes[4].m_multipoint = is_multipointed;
                store_hitboxes.emplace_back(record->m_all_hitboxes[4]);
            }
            break;
        case 3: // stomach
            if (player_ctx.scale_damage(HITGROUP_STOMACH, weapon_damage) >= dmg_threshold) {
                record->m_all_hitboxes[3].m_multipoint = is_multipointed;
                store_hitboxes.emplace_back(record->m_all_hitboxes[3]);
            }
            break;
        case 4: // pelvis
            if (player_ctx.scale_damage(HITGROUP_STOMACH, weapon_damage) >= dmg_threshold) {
                record->m_all_hitboxes[2].m_multipoint = is_multipointed;
                store_hitboxes.emplace_back(record->m_all_hitboxes[2]);
            }
            break;
        case 5: // arms
            if (player_ctx.scale_damage(HITGROUP_LEFTARM, weapon_damage) >= dmg_threshold) {
                record->m_all_hitboxes[16].m_multipoint = is_multipointed;
                store_hitboxes.emplace_back(record->m_all_hitboxes[16]);

                record->m_all_hitboxes[18].m_multipoint = is_multipointed;
                store_hitboxes.emplace_back(record->m_all_hitboxes[18]);
            }
            break;
        case 6: // legs
            if (player_ctx.scale_damage(HITGROUP_LEFTLEG, weapon_damage) >= dmg_threshold) {
                record->m_all_hitboxes[9].m_multipoint = is_multipointed;
                store_hitboxes.emplace_back(record->m_all_hitboxes[9]);

                record->m_all_hitboxes[10].m_multipoint = is_multipointed;
                store_hitboxes.emplace_back(record->m_all_hitboxes[10]);
            }
            break;
        case 7: //feet
            if (player_ctx.scale_damage(HITGROUP_LEFTLEG, weapon_damage) >= dmg_threshold) {
                record->m_all_hitboxes[11].m_multipoint = is_multipointed;
                store_hitboxes.emplace_back(record->m_all_hitboxes[11]);

                record->m_all_hitboxes[12].m_multipoint = is_multipointed;
                store_hitboxes.emplace_back(record->m_all_hitboxes[12]);
            }
            break;
        }
    }
}

bool lag_record_t::setup(C_CSPlayerPawn* pawn) {

    if (!pawn)
        return false;

    m_pawn = pawn;


    m_game_scene_node = pawn->m_pGameSceneNode();

    if (!m_game_scene_node) {
        return false;
    }

    m_skeleton_instance = m_game_scene_node->get_skeleton_instace();

    if (!m_skeleton_instance) {
        return false;
    }

    if (!m_skeleton_instance->m_bone_matrix) {
        return false;
    }

    m_origin = m_game_scene_node->m_vecAbsOrigin();
    m_rotation = m_game_scene_node->m_angAbsRotation();

    m_simulation_time = m_pawn->m_flSimulationTime();

    m_skeleton_instance->calc_world_space_bones(0xFFFFF);

    memcpy(
        m_bones,
        m_skeleton_instance->m_bone_matrix,
        BONE_MATRIX_MEMORY_SIZE
    );


    return true;
}

bool lag_record_t::is_valid() {
    if (!context->localController) {
        return false;
    }

    auto network_client = Interface::GetNetworkClientServices()->get_network_game_client();
    auto chan_info = Interface::GetIVEngineToClient()->GetNetChannelInfo(0);

    if (!chan_info) {
        return false;
    }

    if (!network_client) {
        return false;
    }


    auto fixed_sim_time = timestamp_t{ m_simulation_time };

    auto next_tick = context->localController->m_nTickBase() + 1;
    auto tick_count = timestamp_t{ next_tick, 0.f };

    auto net_latency = timestamp_t{ chan_info->get_net_latency() };

    static auto sv_maxunlag = Interface::GetConVars()->find_by_name("sv_maxunlag");

    auto max_unlag = timestamp_t{ sv_maxunlag->get_float() };

    auto sim_time = tick_count - net_latency;

    auto delta = fixed_sim_time - sim_time;

    delta.normalize();

    if (fixed_sim_time < tick_count - max_unlag) {
        return false;
    }

    if (tick_count < fixed_sim_time) {
        return false;
    }

    return fabsf(Interface::GetGlobalVars()->m_curtime - m_simulation_time) <= 0.2f;
}

void c_lag_compensation::run() {
    if (!MEM::IsInGame())
        return;

    if (!context->localPawn)
        return;

    g_penetration->update_local_ctx();

    for (auto& ref_player : CachedPlayers) {
        if (!ref_player.check_and_update_pawn()) {
            ref_player.m_lagcomp_data.invalidate();
            continue;
        }

        //g_chams->prune_old_onshot(ref_player.m_pawn);

        const bool is_local = (ref_player.pawn == context->localPawn);

        if (is_local) {
            const bool should_update = ref_player.m_lagcomp_data.m_lag_records.empty() ||
                ref_player.pawn->m_flSimulationTime() >
                ref_player.m_lagcomp_data.m_lag_records.newest().m_simulation_time;

            if (should_update) {
                lag_record_t new_record = {};
                if (new_record.setup(ref_player.pawn)) {
                    new_record.m_visual_only = true;
                    ref_player.m_penetration_context.fill(ref_player.pawn);
                    store_hitboxes(&new_record, ref_player.m_penetration_context);
                    ref_player.m_lagcomp_data.add_record(new_record);
                }
            }

            while (!ref_player.m_lagcomp_data.m_lag_records.empty() &&
                !ref_player.m_lagcomp_data.m_lag_records.oldest().is_valid()) {
                ref_player.m_lagcomp_data.remove_oldest();
            }

            //s_backtrack_models[ref_player.pawn].handle(ref_player.pawn);

            continue;
        }

        if (!ref_player.pawn->is_enemy()) {
            ref_player.m_lagcomp_data.invalidate();
            continue;
        }

        if (ref_player.pawn->is_alive() && !ref_player.pawn->m_bGunGameImmunity()) {
            ref_player.m_penetration_context.fill(ref_player.pawn);

            bool should_update = false;
            if (ref_player.m_lagcomp_data.m_lag_records.empty())
                should_update = true;
            else
                should_update = ref_player.pawn->m_flSimulationTime() >
                ref_player.m_lagcomp_data.m_lag_records.newest().m_simulation_time;

            if (should_update) {
                lag_record_t new_record = {};
                if (!new_record.setup(ref_player.pawn)) {
                    ref_player.m_lagcomp_data.invalidate();
                    continue;
                }
                store_hitboxes(&new_record, ref_player.m_penetration_context);
                ref_player.m_lagcomp_data.add_record(new_record);
            }

            while (!ref_player.m_lagcomp_data.m_lag_records.empty() &&
                !ref_player.m_lagcomp_data.m_lag_records.oldest().is_valid()) {
                ref_player.m_lagcomp_data.remove_oldest();
            }
        }
        else
            ref_player.m_lagcomp_data.invalidate();

        //s_backtrack_models[ref_player.m_pawn].handle(ref_player.m_pawn);
    }
}

void c_lag_compensation::force_input_history() {
    static auto parse_input_message = reinterpret_cast<__int64(__fastcall*)(c_cs_input_message*, CCSGOInputHistoryEntryPB*, bool, timestamp_t, timestamp_t, C_CSPlayerPawn*)>( PatternScan("client.dll", "48 89 5C 24 ? 55 57 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 8B 01 48 8B F9"));

    auto& target = CFG::AIM::RAGEBOT::internal_scanRes;

    if (!target.m_record || !target.m_record->m_pawn)
        return;

    auto command = context->pCmd;
    if (!command)
        return;

    timestamp_t interpolation_timing0 = timestamp_t(target.m_record->m_pawn->get_some_timing(0, 1));
    timestamp_t interpolation_timing1 = timestamp_t(target.m_record->m_pawn->get_some_timing(1, 1));

    c_cs_input_message input_message = {};

    input_message.m_view_angles = target.m_angle;

    input_message.m_player_tick_count = command->csgoUserCmd.pBaseCmd->nClientTick;
    input_message.m_render_tick_count = time_to_ticks(target.m_record->m_simulation_time) + interpolation_timing0.m_tick;

    input_message.m_player_tick_fraction = interpolation_timing0.m_frac;
    input_message.m_render_tick_fraction = interpolation_timing0.m_frac;

    for (int i = 0; i < command->csgoUserCmd.inputHistoryField.nTotalSize; i++) {

        CCSGOInputHistoryEntryPB* entry = command->csgoUserCmd.inputHistoryField[i];
        if (!entry)
            continue;

        parse_input_message(&input_message, entry, true, interpolation_timing0, interpolation_timing1, target.m_record->m_pawn);
    }
}

bool c_lag_compensation::wants_lag_compensation_on_entity(lag_record_t* victim) {

    Vec3 difference = victim->m_origin - context->localPawn->m_pGameSceneNode()->m_vecOrigin();
    difference.NormalizeInPlace();

    Vec3 forward;
    context->pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue.ToDirections(&forward, nullptr, nullptr);

    if (forward.dot(difference) < VALID_LAGCOMP_CONE_COSINE)
        return false;

    return true;
}