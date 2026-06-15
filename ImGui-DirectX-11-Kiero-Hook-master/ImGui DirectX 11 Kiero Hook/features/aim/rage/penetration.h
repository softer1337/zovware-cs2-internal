#pragma once
#include "../../../sdk/math.hpp"
#include "../../../sdk/mesh_sdk.h"
#include "../../../sdk/trace/trace.h"
#include <memory>

class C_CSPlayerPawn;

struct local_context_t {
	// weapon data
	float m_headshot_multiplier;
	float m_armor_ratio;
	float m_damage;
	float m_penetration;
	float m_range_mod;
	int   m_local_team;
};
static local_context_t m_local_context;

class c_penetration{
public:
	struct player_context_t {
		C_CSPlayerPawn* m_pawn = nullptr;
		CSkeletonInstance* m_skeleton = nullptr;
		bool m_has_helmet{};
		bool m_has_heavy_armor{};
		float m_head_scale{};
		float m_body_scale{};
		float m_stomach_scale{};
		float m_legs_scale{};
		float m_armor_value{};
		float m_armor_ratio{};
		float m_armor_bonus{};
		float m_heavy_armor_bonus{};
		float m_hitgroup_scale[8]{};
		bool  m_hitgroup_has_armor[8]{};

		int scale_damage(const int& hitgroup, float damage);
		void fill(C_CSPlayerPawn* pawn);

		bool fire_bullet(Vec3 start, Vec3& end, C_CSPlayerPawn* target, HandleBulletPenetrationData_t& data);
	} m_player_context;

	void update_local_ctx();
};


inline auto g_penetration = std::make_unique<c_penetration>();