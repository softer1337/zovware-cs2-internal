#include "../../features.h"
#include "../../../core/mem.hpp"
#include "../../../sdk/math.hpp"
#include "../../../sdk/bone.hpp"
#include <algorithm>
#include <cstdio>
#include <cstdarg>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Simple logging helper for debugging
inline void debug_log(const char* fmt, ...) {
    //char buffer[512];
    //va_list args;
    //va_start(args, fmt);
    //vsnprintf(buffer, sizeof(buffer), fmt, args);
    //va_end(args);
    //OutputDebugStringA(buffer);
    //OutputDebugStringA("\n");
}

#include "../../../sdk/trace/trace.h"
#include "penetration.h"
#include "../../../sdk/schema.h"
#include "hitchance.h"
#include "../../../sdk/utl.hpp"
#include "../../../sdk/ctx.hpp"
#include "../../entity/entity_cache.hpp"
#include "../../entity/lag comp/lag_comp.h"


// Hitbox priority for target selection
static int get_hitbox_priority(int hit_group) {
    switch (hit_group) {
    case HITGROUP_HEAD:    return 400;
    case HITGROUP_CHEST:   return 200;
    case HITGROUP_STOMACH: return 100;
    default:               return 0;
    }
}

static float distance(const Vec3& p1, const Vec3& p2) {
    return std::sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2) + pow(p1.z - p2.z, 2));
}

static Vec3 calcAngle(Vec3 localPos, Vec3 enemyPos) {
    Vec3 delta = enemyPos - localPos;
    float hyp = sqrtf(delta.x * delta.x + delta.y * delta.y);

    Vec3 angles;
    angles.x = atan2f(-delta.z, hyp) * (180.0f / M_PI);
    angles.y = atan2f(delta.y, delta.x) * (180.0f / M_PI);
    angles.z = 0.0f;
    return angles;
}

static Vec3 AngleToVector(const Vec3& angles) {
    float pitch = angles.x * (M_PI / 180.0f);
    float yaw = angles.y * (M_PI / 180.0f);

    float cp = cosf(pitch);
    float sp = sinf(pitch);
    float cy = cosf(yaw);
    float sy = sinf(yaw);

    return Vec3(cp * cy, cp * sy, -sp);
}

static float getFov(const Vec3& viewAngles, const Vec3& aimAngles)
{
    Vec3 viewDir = AngleToVector(viewAngles);
    Vec3 aimDir = AngleToVector(aimAngles);

    float vLen = std::sqrt(viewDir.x * viewDir.x +
        viewDir.y * viewDir.y +
        viewDir.z * viewDir.z);

    float aLen = std::sqrt(aimDir.x * aimDir.x +
        aimDir.y * aimDir.y +
        aimDir.z * aimDir.z);

    if (vLen <= 0.f || aLen <= 0.f)
        return 0.f;

    viewDir.x /= vLen;
    viewDir.y /= vLen;
    viewDir.z /= vLen;

    aimDir.x /= aLen;
    aimDir.y /= aLen;
    aimDir.z /= aLen;

    float dotProduct =
        viewDir.x * aimDir.x +
        viewDir.y * aimDir.y +
        viewDir.z * aimDir.z;

    dotProduct = std::clamp(dotProduct, -1.0f, 1.0f);

    return std::acos(dotProduct) * (180.0f / 3.14159265f);
}

bool should_stop(C_CSPlayerPawn* pLocal) {
    float velocity = pLocal->m_vecAbsVelocity().Length_2d();
    if (velocity < 220.f * 0.34f)
        return false;
    return true;
}

// Check if a point on target is occluded by other hitboxes between start position and target
bool is_point_occluded(const Vec3& start, const Vec3& point, c_hitbox_data* target_hitbox) {
    if (!target_hitbox)
        return false;

    Vec3 direction = point - start;
    float ray_length = direction.Length();
    if (ray_length < 0.1f)
        return false;

    Vec3 ray_dir = direction / ray_length;

    // Check if hitbox center is close enough to the ray to block it
    // Using ray-sphere intersection simplified test
    Vec3 to_hitbox = target_hitbox->m_center - start;
    
    // Project hitbox center onto ray
    float proj_length = (to_hitbox.x * ray_dir.x + to_hitbox.y * ray_dir.y + to_hitbox.z * ray_dir.z);
    
    // Hitbox doesn't occlude if it's behind start or beyond target
    if (proj_length <= 0.0f || proj_length >= ray_length)
        return false;
    
    // Find closest point on ray to hitbox center
    Vec3 closest_on_ray = start + ray_dir * proj_length;
    Vec3 to_closest = target_hitbox->m_center - closest_on_ray;
    float distance_to_ray = to_closest.Length();
    
    // Occluded if hitbox is blocking the ray (within its radius)
    // Using 0.8f factor to give some tolerance for edge cases
    return distance_to_ray < (target_hitbox->m_radius * 0.8f);
}

// Scan a single lag record for the best point
void scan_record(scan_result_t& result, lag_record_t* record, c_penetration::player_context_t* penetration_ctx, const Vec3& start) {
    if (!record || !record->m_pawn || !penetration_ctx)
        return;

    const int target_health = record->m_pawn->m_iHealth();
    int dmg_threshold = CFG::AIM::RAGEBOT::minDamage;
    if (dmg_threshold > target_health)
        dmg_threshold = target_health;
    else if (dmg_threshold > 100)
        dmg_threshold = target_health + CFG::AIM::RAGEBOT::minDamage - 100;

    result.m_record = record;
    result.m_penetration_context = penetration_ctx;
    result.m_damage = -1;
    result.m_hitbox = nullptr;
    result.m_damage_threshold = dmg_threshold;
    result.m_start = start;
    result.m_world_dist = (record->m_origin - start).Length();

    // Build list of all hitboxes to check for occlusion
    // Try to find best hitbox from rage hitboxes with priority and occlusion checking
    int best_priority = -1;
    
    for (size_t i = 0; i < record->m_rage_hitboxes.size(); ++i) {
        c_hitbox_data* hitbox = &record->m_rage_hitboxes[i];
        if (!hitbox)
            continue;

        Vec3 point = hitbox->m_center;

        // Check occlusion from other hitboxes
        bool occluded = false;
        for (size_t j = 0; j < record->m_all_hitboxes.size(); ++j) {
            c_hitbox_data* occluder = &record->m_all_hitboxes[j];
            if (occluder == hitbox || !occluder)
                continue;
            
            if (is_point_occluded(start, point, occluder)) {
                occluded = true;
                break;
            }
        }

        // Skip if occluded by another hitbox
        if (occluded) {
            debug_log("[Rage]       hitbox[%zu]: OCCLUDED", i);
            continue;
        }

        // Check penetration and damage
        HandleBulletPenetrationData_t pen_data;
        pen_data.damage = m_local_context.m_damage;
        pen_data.penetration = m_local_context.m_penetration;
        pen_data.rangeModifier = m_local_context.m_range_mod;
        pen_data.penetrationCount = CFG::AIM::RAGEBOT::isPenetrationEnabled ? 4 : 0;

        int current_damage = 0;
        //if (penetration_ctx->fire_bullet(start, (Vec3&)point, record->m_pawn, pen_data))
        //    current_damage = (int)pen_data.damage;

        CGameTrace tr = traceRay(start, point, (uintptr_t)context->localPawn);


        if (tr.IsVisible())
            current_damage = 100;

        debug_log("[Rage]       hitbox[%zu] (group %d): fire_bullet damage=%d (threshold=%d)", i, hitbox->m_hit_group, current_damage, dmg_threshold);

        if (current_damage >= dmg_threshold) {
            int hitbox_priority = get_hitbox_priority(hitbox->m_hit_group);
            
            if (!result.m_hitbox) {
                result.m_point = point;
                result.m_hitbox = hitbox;
                result.m_damage = current_damage;
                result.m_point_projected_radius = hitbox->m_radius;
                best_priority = hitbox_priority;
            }
            else if (hitbox_priority > best_priority) {
                // Better hitbox priority
                result.m_point = point;
                result.m_hitbox = hitbox;
                result.m_damage = current_damage;
                result.m_point_projected_radius = hitbox->m_radius;
                best_priority = hitbox_priority;
            }
            else if (hitbox_priority == best_priority && current_damage > result.m_damage) {
                // Same priority but more damage
                result.m_point = point;
                result.m_hitbox = hitbox;
                result.m_damage = current_damage;
                result.m_point_projected_radius = hitbox->m_radius;
            }
        }
    }

    if (result.m_hitbox) {
        debug_log("[Rage]     scan_record result: damage=%d", result.m_damage);
    } else {
        debug_log("[Rage]     scan_record: NO VALID HITBOX (checked %zu hitboxes, threshold=%d)", record->m_rage_hitboxes.size(), dmg_threshold);
    }
}

// Scan all records of a single player
void scan_player(scan_result_t& result, CachedPlayer_t* player, const Vec3& start) {
    if (!player || !player->pawn)
        return;
    debug_log("[scan_player] lagcomp ptr=%p records=%zu valid=%d",
        &player->m_lagcomp_data,
        player->m_lagcomp_data.m_lag_records.size(),
        player->m_lagcomp_data.is_valid());
    auto& lagcomp_data = player->m_lagcomp_data;
    if (!lagcomp_data.is_valid())
        return;

    const float newest_sim = lagcomp_data.m_lag_records.size() > 0 ? 
        lagcomp_data.m_lag_records.back().m_simulation_time : 0.0f;

    if (!newest_sim) {
        debug_log("[Rage] scan_player: newest_sim is 0!");
        return;
    }

    int records_scanned = 0;
    const int max_records = 3;
    debug_log("[Rage]   scan_player: testing %d lag records (max %d)", (int)lagcomp_data.m_lag_records.size(), max_records);

    // Iterate from newest to oldest records
    for (int i = (int)lagcomp_data.m_lag_records.size() - 1; i >= 0 && records_scanned < max_records; --i) {
        lag_record_t& record = lagcomp_data.m_lag_records[i];

        scan_result_t current = {};
        scan_record(current, &record, &player->m_penetration_context, start);

        // Compare and pick better target
        if (!current.m_hitbox) {
            debug_log("[Rage]     record[%d]: no hitbox", records_scanned);
            records_scanned++;
            continue;
        }

        debug_log("[Rage]     record[%d]: damage=%d", records_scanned, current.m_damage);

        if (!result.m_hitbox) {
            result = current;
        }
        else if (current.m_damage > result.m_damage) {
            result = current;
        }
        else if (current.m_damage == result.m_damage) {
            float current_dist = (current.m_point - start).Length();
            float result_dist = (result.m_point - start).Length();
            if (current_dist < result_dist)
                result = current;
        }

        records_scanned++;
    }
}

// Main player scanning function
void player_scan(scan_result_t& best_target, const Vec3& localPos) {
    best_target = {};

    debug_log("[Rage] player_scan() called, CachedPlayers size: %zu", CachedPlayers.size());

    if (CachedPlayers.size() == 0) {
        debug_log("[Rage] No cached players!");
        return;
    }

    int localTeam = context->localPawn->m_iTeamNum();

    scan_result_t best_result = {};
    int valid_enemies = 0;

    for (size_t i = 0; i < CachedPlayers.size(); i++) {
        CachedPlayer_t& cached_player = CachedPlayers[i];
        C_CSPlayerPawn* playerPawn = cached_player.pawn;

        if (!playerPawn || playerPawn == context->localPawn)
            continue;

        if (playerPawn->m_iHealth() <= 0)
            continue;

        if (playerPawn->m_iTeamNum() == localTeam)
            continue;

        if (!cached_player.m_lagcomp_data.is_valid()) {
            debug_log("[Rage] Player %zu: lagcomp_data invalid!", i);
            continue;
        }

        valid_enemies++;
        debug_log("[Rage] Scanning player %zu (health: %d, lagrecords: %zu)", i, playerPawn->m_iHealth(), cached_player.m_lagcomp_data.m_lag_records.size());

        scan_result_t player_result = {};
        scan_player(player_result, &cached_player, localPos);

        if (!player_result.m_hitbox) {
            debug_log("[Rage]   -> No hitbox found for player %zu", i);
            continue;
        }

        debug_log("[Rage]   -> Found target: damage=%d, priority=%d", player_result.m_damage, get_hitbox_priority(player_result.m_hitbox->m_hit_group));

        if (!best_result.m_hitbox) {
            best_result = player_result;
        }
        else {
            // Prioritize by damage and hitbox
            int current_priority = get_hitbox_priority(player_result.m_hitbox->m_hit_group);
            int best_priority = get_hitbox_priority(best_result.m_hitbox->m_hit_group);

            if (current_priority > best_priority) {
                best_result = player_result;
            }
            else if (current_priority == best_priority) {
                if (player_result.m_damage > best_result.m_damage) {
                    best_result = player_result;
                }
                else if (player_result.m_damage == best_result.m_damage) {
                    if (player_result.m_world_dist < best_result.m_world_dist) {
                        best_result = player_result;
                    }
                }
            }
        }
    }

    if (best_result.m_hitbox) {
        debug_log("[Rage] FINAL TARGET SELECTED: damage=%d, priority=%d (from %d valid enemies)", 
            best_result.m_damage, get_hitbox_priority(best_result.m_hitbox->m_hit_group), valid_enemies);
    } else {
        debug_log("[Rage] NO TARGET FOUND (valid_enemies: %d)", valid_enemies);
    }

    best_target = best_result;
}

void FEATURES::AIM::RAGEBOT::onMove(CUserCmd* pCmd)
{
    debug_log("[onMove] tick start");

    if (!CFG::AIM::RAGEBOT::isAimEnabled) {
        debug_log("[onMove] exit: aim disabled");
        return;
    }

    if (!MEM::IsInGame()) {
        debug_log("[onMove] exit: not in game");
        return;
    }

    if (!pCmd || !pCmd->csgoUserCmd.pBaseCmd || !pCmd->csgoUserCmd.pBaseCmd->pViewAngles) {
        debug_log("[onMove] exit: invalid cmd/viewangles");
        return;
    }

    if (!context->localPawn) {
        debug_log("[onMove] exit: no localPawn");
        return;
    }

    debug_log("[onMove] localPawn=%p", context->localPawn);

    // Update penetration context with weapon data
    if (context->activeWeaponData) {
        m_local_context.m_damage = context->activeWeaponData->m_nDamage();
        m_local_context.m_penetration = context->activeWeaponData->m_flPenetration();
        m_local_context.m_range_mod = 1.0f;
        m_local_context.m_local_team = context->localPawn->m_iTeamNum();

        debug_log("[onMove] weapon dmg=%d pen=%.3f",
            m_local_context.m_damage,
            m_local_context.m_penetration);
    }
    else {
        debug_log("[onMove] no weapon data");
    }

    Vec3 localPos = context->localPawn->m_vOldOrigin() +
        context->localPawn->m_vecViewOffset();

    debug_log("[onMove] localPos=(%.2f %.2f %.2f)",
        localPos.x, localPos.y, localPos.z);

    QAngle qView = pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue;
    Vec3 viewAngles(qView.x, qView.y, 0.f);

    debug_log("[onMove] viewAngles=(%.2f %.2f)",
        viewAngles.x, viewAngles.y);

    // Scan and find best target using lag compensation
    scan_result_t best_target = {};
    player_scan(best_target, localPos);

    CFG::AIM::RAGEBOT::internal_scanRes = best_target;

    debug_log("[onMove] scan done: hitbox=%p dmg=%d dist=%.2f record=%p",
        best_target.m_hitbox,
        best_target.m_damage,
        best_target.m_world_dist,
        best_target.m_record);

    if (!best_target.m_hitbox || !best_target.m_record || best_target.m_damage <= 0) {
        debug_log("[onMove] exit: no valid target");
        return;
    }

    Vec3 bestAngle = calcAngle(localPos, best_target.m_point);

    debug_log("[onMove] bestAngle=(%.2f %.2f)", bestAngle.x, bestAngle.y);

    // Check FOV
    float fov = getFov(viewAngles, bestAngle);
    debug_log("[onMove] fov=%.2f limit=%.2f", fov, CFG::AIM::RAGEBOT::fov);

    if (fov > CFG::AIM::RAGEBOT::fov) {
        debug_log("[onMove] exit: FOV too high");
        return;
    }

    // Check min damage
    debug_log("[onMove] damage=%d minDamage=%d",
        best_target.m_damage,
        CFG::AIM::RAGEBOT::minDamage);

    if (best_target.m_damage < CFG::AIM::RAGEBOT::minDamage) {
        debug_log("[onMove] exit: minDamage");
        return;
    }

    if (!CFG::AIM::RAGEBOT::isSilentEnabled)
    {
        debug_log("[onMove] applying viewangles (non-silent)");
        pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue.x = bestAngle.x;
        pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue.y = bestAngle.y;
    }
    else {
        debug_log("[onMove] applying viewangles (silent)");
        pCmd->SetSubTickAngle(QAngle(bestAngle.x, bestAngle.y, 0.f));
    }

    CFG::AIM::RAGEBOT::internal_wantsStop = should_stop(context->localPawn);

    debug_log("[onMove] wantsStop=%d forceShoot=%d",
        CFG::AIM::RAGEBOT::internal_wantsStop,
        CFG::AIM::RAGEBOT::isForceShootEnabled);

    if (!CFG::AIM::RAGEBOT::isForceShootEnabled) {
        debug_log("[onMove] exit: forceShoot disabled");
        return;
    }

    bool accurate = IsAccurate(
        QAngle(bestAngle.x, bestAngle.y, bestAngle.z),
        best_target.m_record->m_pawn,
        localPos
    );

    debug_log("[onMove] IsAccurate=%d", accurate);

    if (!accurate) {
        debug_log("[onMove] exit: not accurate");
        return;
    }

    debug_log("[onMove] SHOOT");

    pCmd->nButtons.nValue |= 1 << 0;
}