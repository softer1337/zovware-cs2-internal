#pragma once
#include "math.hpp"
#include "../core/mem.hpp"

enum class bone {
	Head = 6,
	Neck = 5,
	UpperChest = 4,
	LowerChest = 3,
	Stomach = 2,
	Pelivs = 1,
	LeftShoulder = 8,
	LeftElbow = 9,
	LeftArm = 10,
	RightShoulder = 13,
	RightElbow = 14,
	RightArm = 15,
	LeftThigh = 22,
	LeftKnee = 23,
	LeftLeg = 24,
	RightThigh = 25,
	RightKnee = 26,
	RightLeg = 27,
	RightFoot = 28
};

enum HitGroup
{
	HITGROUP_HEAD,
	HITGROUP_CHEST,
	HITGROUP_LEFTARM,
	HITGROUP_RIGHTARM,
	HITGROUP_NECK,
	HITGROUP_STOMACH,
	HITGROUP_LEFTLEG,
	HITGROUP_RIGHTLEG
};

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