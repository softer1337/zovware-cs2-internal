#pragma once
#include "math.hpp"
#include "../core/mem.hpp"
#include "enums.h"

inline Vec3 GetBonePosition(int boneIndex, uintptr_t targetPlayer) {
	uintptr_t node = MEM::read<uintptr_t>(offsets::m_pGameSceneNode + targetPlayer);
	if (!node) return Vec3(0, 0, 0);

	uintptr_t modelState = node + 0x150;

	uintptr_t boneArray = *(uintptr_t*)(modelState + 0x80);


	if (!boneArray) return Vec3(0, 0, 0);

	uintptr_t boneAddress = boneArray + (boneIndex * 32);
	Vec3 bonePos = *(Vec3*)boneAddress;

	return bonePos;
}

enum hitbox_orientation {
	hitbox_vertical = 0,
	hitbox_horizontal,
	hitbox_head_identity // kinda annoying but whatever
};

class c_hitbox_data {
public:
	hitbox_orientation m_orientation;
	Vec3 m_mins, m_maxs, m_center;
	float m_radius;
	int m_hitbox_index, m_hit_group;
	float m_pointscale; // 0.0 - 1.0
	bool m_multipoint; // for ragenz

	Vec3   m_axis;
	Vec3   m_dir;
	float    m_axis_len;
	float    m_axis_len_sqr;
	float    m_half_height;
	float    m_capsule_profile_length;

	bool segment_intersects_capsule(const Vec3& start, const Vec3& end);
	float projected_valid_radius(const Vec3& view_point, const Vec3& point_on_capsule);
};

class c_hitbox {
public:
	const char* m_name;               // 0x0000
	const char* m_surface_property;   // 0x0008
	const char* m_bone_name;          // 0x0010
	Vec3 m_min_bounds;                // 0x0018
	Vec3 m_max_bounds;                // 0x0024
	float m_shape_radius;             // 0x0030
	std::uint32_t m_bone_name_hash;   // 0x0034
	std::int32_t m_group_id;          // 0x0038
	std::uint8_t m_shape_type;        // 0x003C
	bool m_translation_only;          // 0x003D
	std::uint32_t m_crc;              // 0x0040
	char pad_0044[0x4];               // 0x0044
	std::uint16_t m_hitbox_index;     // 0x0048
	char pad_004A[0x22];              // 0x004A
};

class c_hitbox_sets {
public:
	char pad_0000[16];           // 0x0000
	void* N00000221;               // 0x0010
	char pad_0018[24];           // 0x0018
	c_hitbox* m_hitbox;            // 0x0030
	std::int32_t m_hitbox_count;   // 0x0038
	char pad_003C[1028];          // 0x003C (1028 decimal = 0x404)
};
class c_render_mesh {
public:
	char pad_0000[0x150];            // 0x0000
	c_hitbox_sets* m_hitbox_sets;    // 0x0118
	std::int32_t m_hitbox_sets_count;// 0x0120
};
class c_render_meshes {
public:
	c_render_mesh* m_meshes;        // 0x0000
};

class c_model {
public:
	c_hitbox* get_hitbox(const std::int32_t index);
	const char* get_hitbox_name(const std::int32_t index);
	int get_hitbox_count();

public:
	char pad_0000[0x70];              // 0x0000
	std::int32_t m_rendermesh_count;  // 0x0070
	char pad_0074[0x4];               // 0x0074
	c_render_meshes* m_render_meshes; // 0x0078
};



inline static c_hitbox_data construct_hitbox_data(const c_hitbox& hitbox, const bone_data_t& bone_data, const bool& multipoint, const int& pointscale) {
	c_hitbox_data data = {};
	data.m_radius = hitbox.m_shape_radius;
	data.m_hitbox_index = hitbox.m_hitbox_index;
	data.m_mins = bone_data.m_rot.rotate_vector(hitbox.m_min_bounds * bone_data.m_scale) + bone_data.m_pos;
	data.m_maxs = bone_data.m_rot.rotate_vector(hitbox.m_max_bounds * bone_data.m_scale) + bone_data.m_pos;
	data.m_center = (data.m_mins + data.m_maxs) / 2.f;
	data.m_multipoint = multipoint;
	data.m_pointscale = static_cast<float>(pointscale) / 100.f;
	Vec3 axis = data.m_maxs - data.m_mins;
	data.m_axis = axis;
	data.m_axis_len = axis.Length();
	data.m_axis_len_sqr = axis.LengthSqr();
	data.m_dir = axis.IsZero() ? Vec3(0, 0, 1) : axis.normalized();
	data.m_half_height = data.m_axis_len * 0.5f;
	data.m_capsule_profile_length = data.m_axis_len + A_PI * (data.m_pointscale * data.m_radius);
	data.m_hit_group = hitbox_to_hitgroup_table[data.m_hitbox_index];

	const float vertical = std::abs(data.m_dir.z);
	const float horizontal = std::sqrt(data.m_dir.x * data.m_dir.x + data.m_dir.y * data.m_dir.y);

	if (data.m_hitbox_index == HITBOX_HEAD) {
		data.m_orientation = hitbox_orientation::hitbox_head_identity;
	}
	else {
		const float vertical = std::abs(data.m_dir.z);
		const float horizontal = std::sqrt(data.m_dir.x * data.m_dir.x + data.m_dir.y * data.m_dir.y);

		data.m_orientation = (vertical > horizontal)
			? hitbox_orientation::hitbox_vertical
			: hitbox_orientation::hitbox_horizontal;
	}

	return data;
}

