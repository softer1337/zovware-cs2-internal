#pragma once
#include "../../../sdk/c_csplayerpawn.hpp"
#include "../../../sdk/tight_array.h"





constexpr auto BONE_MATRIX_MEMORY_SIZE = sizeof(bone_data_t) * 128;
constexpr float VALID_LAGCOMP_CONE_COSINE = 0.707107f;    // 45 degree angle

struct lag_record_t {

	bool m_visual_only = false;

	C_CSPlayerPawn* m_pawn = nullptr;
	C_GameSceneNode* m_game_scene_node = nullptr;
	CSkeletonInstance* m_skeleton_instance = nullptr;

	bone_data_t m_bones[128];

	float m_simulation_time = 0.0f;

	Vec3 m_rotation, m_origin = {};

	tight_array< c_hitbox_data, 19 > m_all_hitboxes;
	tight_array< c_hitbox_data, 11 > m_rage_hitboxes;

	bool setup(C_CSPlayerPawn* pawn);
	bool should_skip_interpolation();
	void try_adjust_velocity();
	bool is_valid();
};

class c_lag_compensation {
public:
	void run();
	void force_input_history();
	bool wants_lag_compensation_on_entity(lag_record_t* victim);
};

inline auto LagCompensation = std::make_unique<c_lag_compensation>();