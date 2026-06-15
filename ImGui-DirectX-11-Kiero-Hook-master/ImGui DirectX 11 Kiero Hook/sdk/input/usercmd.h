#pragma once
#define NOMINMAX
#define _HAS_STD_BYTE 0
#undef max
#include <Windows.h>
#include <cstdint>
#include <limits>
#include <string>
#include "../qangle.h"
#include "../../core/mem.hpp"
#include "../enums.h"
#include "../../core/hooks/hooks.h"

template <typename T>
struct RepeatedPtrField_t
{
	struct Rep_t
	{
		int nAllocatedSize;
		T* tElements[(std::numeric_limits<int>::max() - 2 * sizeof(int)) / sizeof(void*)];
	};

	void* pArena;
	int nCurrentSize;
	int nTotalSize;
	Rep_t* pRep;

	T* operator[](int index)
	{
		return pRep->tElements[index];
	}

	const T* operator[](int index) const
	{
		return pRep->tElements[index];
	}

	inline int& size() {
		return nCurrentSize;
	}

	inline int& capacity() {
		return nTotalSize;
	}

	inline int& max_size() {
		return pRep->nAllocatedSize;
	}
	inline operator bool() const {
		return pRep != nullptr;
	}
	T* add(T* element) {
		using fn_add_to_container = T * (__fastcall*)(RepeatedPtrField_t*, T*);
		static auto fn = reinterpret_cast<fn_add_to_container>( PatternScan("client.dll", "48 89 5C 24 ? 57 48 83 EC ? 48 8B D9 48 8B FA 48 8B 49 ? 48 85 C9 74 ? 8B 01"));
		return fn(this, element);
	}
};

class CBasePB
{
public:
	BYTE pad01[0x8];
	std::uint32_t nHasBits; // 0x8
	std::uint64_t nCachedBits; // 0xC

	void CheckAndSetBits(std::uint32_t nBits)
	{
		if (!(nHasBits & nBits))
			nHasBits |= nBits;
	}
	void set_bits(uint64_t n_bits) {
		nCachedBits |= n_bits;
	}
};//size: 0x18

class CMsgQAngle : public CBasePB
{
public:
	QAngle angValue;
}; //size: 0x28

class CMsgVector : public CBasePB
{
public:
	void* vecValue; // 0x18 //vec4
};

class CCSGOInterpolationInfoPB : public CBasePB
{
public:
	float flFraction; // 0x18
	//int nSrcTick; // 0x1C
	//int nDstTick; // 0x20
};

class CCSGOInputHistoryEntryPB : public CBasePB
{
public:
	CMsgQAngle* pViewAngles; // 0x18
	CMsgVector* pShootPosition; // 0x20
	CMsgVector* pTargetHeadPositionCheck; // 0x28
	CMsgVector* pTargetAbsPositionCheck; // 0x30
	CMsgQAngle* pTargetAngPositionCheck; // 0x38
	CCSGOInterpolationInfoPB* cl_interp; // 0x40
	CCSGOInterpolationInfoPB* sv_interp0; // 0x48
	CCSGOInterpolationInfoPB* sv_interp1; // 0x50
	CCSGOInterpolationInfoPB* player_interp; // 0x58
	int nRenderTickCount; // 0x60
	float flRenderTickFraction; // 0x64
	int nPlayerTickCount; // 0x68
	float flPlayerTickFraction; // 0x6C
	int nFrameNumber; // 0x70
	int nTargetEntIndex; // 0x74
};

struct CInButtonStatePB : CBasePB
{
	std::uint64_t nValue;
	std::uint64_t nValueChanged;
	std::uint64_t nValueScroll;


	void set_button_state(const uint64_t& u_value, e_button_state e_button_state) {
		switch (e_button_state) {
		case e_button_state::in_button_up: {
			nValue &= ~u_value;
			nValueChanged &= ~u_value;
			nValueScroll &= ~u_value;
			break;
		}
		case e_button_state::in_button_down: {
			nValue |= u_value;
			nValueChanged &= ~u_value;
			nValueScroll &= ~u_value;
			break;
		}
		case e_button_state::in_button_down_up: {
			nValue &= ~u_value;
			nValueChanged |= u_value;
			nValueScroll &= ~u_value;
			break;
		}
		case e_button_state::in_button_up_down: {
			nValue |= u_value;
			nValueChanged |= u_value;
			nValueScroll &= ~u_value;
			break;
		}
		case e_button_state::in_button_up_down_up: {
			nValue &= ~u_value;
			nValueChanged &= ~u_value;
			nValueScroll |= u_value;
			break;
		}
		case e_button_state::in_button_down_up_down: {
			nValue |= u_value;
			nValueChanged &= ~u_value;
			nValueScroll |= u_value;
			break;
		}
		case e_button_state::in_button_down_up_down_up: {
			nValue &= ~u_value;
			nValueChanged |= u_value;
			nValueScroll |= u_value;
			break;
		}
		case e_button_state::in_button_up_down_up_down: {
			nValue |= u_value;
			nValueChanged |= u_value;
			nValueScroll |= u_value;
			break;
		}
		}
	}
};

struct CSubtickMoveStep : CBasePB
{
public:
	std::uint64_t nButton;
	bool bPressed;
	float flWhen;
	float flAnalogForwardDelta;
	float flAnalogLeftDelta;
	float m_analog_pitch_delta;
	float m_analog_yaw_delta;

	void set_button(const uint64_t& button) {
		nButton = button;
		set_bits(e_protobuf_bits_t::protoslot_1);
	}

	void set_pressed(const bool& pressed) {
		bPressed = pressed;
		set_bits(e_protobuf_bits_t::protoslot_2);
	}

	void set_when(const float& when) {
		flWhen = when;
		set_bits(e_protobuf_bits_t::protoslot_3);
	}

	void set_analog_forward_delta(const float& forward_delta) {
		flAnalogForwardDelta = forward_delta;
		set_bits(e_protobuf_bits_t::protoslot_4);
	}

	void set_analog_left_delta(const float& analog_left_delta) {
		flAnalogLeftDelta = analog_left_delta;
		set_bits(e_protobuf_bits_t::protoslot_5);
	}

	void set_analog_pitch_delta(const float& analog_pitch_delta) {
		m_analog_pitch_delta = analog_pitch_delta;
		set_bits(e_protobuf_bits_t::protoslot_6);
	}

	void set_analog_yaw_delta(const float& analog_yaw_delta) {
		m_analog_yaw_delta = analog_yaw_delta;
		set_bits(e_protobuf_bits_t::protoslot_7);
	}
};

static_assert(sizeof(CSubtickMoveStep) == 56);

class CBaseUserCmdPB : public CBasePB
{
public:
	RepeatedPtrField_t<CSubtickMoveStep> subtickMovesField; //0x18
	std::string* strMoveCrc; //0x30
	CInButtonStatePB* pInButtonState; //0x38
	CMsgQAngle* pViewAngles;
	std::int32_t nLegacyCommandNumber;
	std::int32_t nClientTick;
	float flForwardMove; //Forwardmove, sidemove (and upmove?) - Range: -1.0 - 1.0
	float flSideMove;
	float flUpMove;
	std::int32_t nImpulse;
	std::int32_t nWeaponSelect;
	std::int32_t nRandomSeed;
	std::int32_t nMousedX;
	std::int32_t nMousedY;
	std::uint32_t nConsumedServerAngleChanges;
	std::int32_t nCmdFlags;
	std::uint32_t nPawnEntityHandle;

	CSubtickMoveStep* create_subtick_move()
	{
		if (subtickMovesField && subtickMovesField.size() < subtickMovesField.max_size())
			return subtickMovesField[subtickMovesField.size()++];

		using fn_create_subtick_move = CSubtickMoveStep * (__fastcall*)(void*);

		static uintptr_t addr = PatternScan("client.dll",CREATENEWSUBTICKMOVESTEP_PATTERN);
		static auto fn = reinterpret_cast<fn_create_subtick_move>(addr);

		CSubtickMoveStep* subtick_move = fn(subtickMovesField.pArena);
		subtickMovesField.add(subtick_move);

		return subtick_move;
	}

	void set_legacy_command_number(const int& value) {
		nLegacyCommandNumber = value;
		set_bits(e_protobuf_bits_t::protoslot_4);
	}

	void set_client_tick(const int& value) {
		nClientTick = value;
		set_bits(e_protobuf_bits_t::protoslot_5);
	}

	void set_forward_move(const float& value) {
		flForwardMove = value;
		set_bits(e_protobuf_bits_t::protoslot_6);
	}

	void set_side_move(const float& value) {
		flSideMove = value;
		set_bits(e_protobuf_bits_t::protoslot_7);
	}

	void set_up_move(const float& value) {
		flUpMove = value;
		set_bits(e_protobuf_bits_t::protoslot_8);
	}

	void set_impulse(const int& value) {
		nImpulse = value;
		set_bits(e_protobuf_bits_t::protoslot_9);
	}

	void set_weapon_select(const int& value) {
		nWeaponSelect = value;
		set_bits(e_protobuf_bits_t::protoslot_10);
	}

	void set_random_seed(const int& value) {
		nRandomSeed = value;
		set_bits(e_protobuf_bits_t::protoslot_11);
	}

	void set_moused_x(const int& value) {
		nMousedX = value;
		set_bits(e_protobuf_bits_t::protoslot_12);
	}

	void set_moused_y(const int& value) {
		nMousedY = value;
		set_bits(e_protobuf_bits_t::protoslot_13);
	}

	void set_consumed_server_angle_changes(const uint32_t& value) {
		nConsumedServerAngleChanges = value;
		set_bits(e_protobuf_bits_t::protoslot_14);
	}

	void set_cmd_flags(const int& value) {
		nCmdFlags = value;
		set_bits(e_protobuf_bits_t::protoslot_15);
	}

	void set_pawn_entity_handle(const uint32_t& value) {
		nPawnEntityHandle = value;
		set_bits(e_protobuf_bits_t::protoslot_16);
	}

}; //size: 0x68?

class CCSGOUserCmdPB
{
public:
	void* vtable;
	std::uint32_t nHasBits; //0x0
	std::uint64_t nCachedSize; //0x4
	RepeatedPtrField_t<CCSGOInputHistoryEntryPB> inputHistoryField; //0xC
	CBaseUserCmdPB* pBaseCmd; //0x14 //0x40
	bool bLeftHandDesired; //0x1C
	bool bIsPredictingBodyShotFX;
	bool bIsPredictingHeadShotFX;
	bool bIsPredictingKillRagdolls;
	std::int32_t nAttack3StartHistoryIndex; //0x20
	std::int32_t nAttack1StartHistoryIndex; //0x24
	std::int32_t nAttack2StartHistoryIndex; //0x28

	void CheckAndSetBits(std::uint32_t nBits)
	{
		if (!(nHasBits & nBits))
			nHasBits |= nBits;
	}
};
static_assert(sizeof(CCSGOUserCmdPB) == 0x48); // 72 байта


struct CInButtonState
{
public:
	void* m_vtable;
	std::uint64_t nValue; // 0x8
	std::uint64_t nValueChanged; // 0x10
	std::uint64_t nValueScroll; // 0x18
};

class CUserCmd {
public:
	void* vtable;
	int m_sequence_number;
	CCSGOUserCmdPB csgoUserCmd;
	CInButtonState nButtons;
	char pad1[8];
	double m_real_time;
	float m_curtime;
	bool m_has_been_predicted;
	int m_previous_flags;
	int m_current_flags;

	CCSGOInputHistoryEntryPB* GetInputHistoryEntry(int nIndex) {
		if (nIndex >= csgoUserCmd.inputHistoryField.pRep->nAllocatedSize || nIndex >= csgoUserCmd.inputHistoryField.nCurrentSize)
			return nullptr;

		return csgoUserCmd.inputHistoryField.pRep->tElements[nIndex];
	}

	void SetSubTickAngle(const QAngle& angView) {
		for (int i = 0; i < this->csgoUserCmd.inputHistoryField.pRep->nAllocatedSize; i++) {
			CCSGOInputHistoryEntryPB* pInputEntry = this->GetInputHistoryEntry(i);
			if (!pInputEntry || !pInputEntry->pViewAngles)
				continue;

			pInputEntry->pViewAngles->angValue = angView;
		}
	}

	bool IsButtonPressed(uint64_t button) const {
		if (!csgoUserCmd.pBaseCmd)
			return false;

		return csgoUserCmd.pBaseCmd->pInButtonState->nValue & button;
	}
};
static_assert(sizeof(CUserCmd) == 0x98);


class c_cs_input_message {
public:
	int32_t m_render_tick_count; //0x0000
	float m_render_tick_fraction; //0x0004
	int32_t m_player_tick_count; //0x0008
	float m_player_tick_fraction; //0x000C
	Vec3 m_view_angles; //0x0010
	Vec3 m_origin; //0x001C
	char pad_0028[56]; //0x0028
}; //Size: 0x0060

class CCSGOInput
{
public:
	char pad_0000[0x228]; //0x0000
	bool block_shot; //0x0228
	bool in_thirdperson; //0x0229
	char pad_022A[0x6]; //0x022A
	QAngle third_person_angles; //0x0230 note: Vec3
	char pad_023C[0x14]; //0x023C
	uint64_t button_pressed; //0x0250
	uint64_t mouse_button_pressed; //0x0258
	uint64_t button_un_pressed; //0x0260
	uint64_t keyboard_copy; //0x0268
	float forward_move; //0x0270
	float left_move; //0x0274
	float up_move; //0x0278
	int mouse_delta_x; //0x027C
	int mouse_delta_y; //0x0280
	int32_t subtick_count; //0x0284
	void* subticks[0xC];//c_subtick_input subticks[12]; //0x0288
	QAngle view_angles; //0x03A8 note: QAngle_t
	int32_t target_entity_index; //0x03B4
	char pad_03B8[0x258]; //0x03B8
	int32_t attack_history_1; //0x0610
	int32_t attack_history_2; //0x0614
	int32_t attack_history_3; //0x0618
	char pad_061C[0x4]; //0x061C
	int32_t message_size; //0x0620
	char pad_0624[0x4]; //0x0624
	void* message;//c_cs_input_message* message; //0x0628
};

class CUserCmdManager
{
public:
	CUserCmd m_commands[150]; //0x0000
	//c_user_cmd m_global_cmd; //0x5910
	int32_t m_sequence; //0x59A8
	char pad_59AC[4]; //0x59AC
	double m_real_time; //0x59B0
	char pad_59B8[2080]; //0x59B8

	CUserCmd* get_cmd()
	{
		return &m_commands[m_sequence % 150];
	}

	CUserCmd* get_cmd_by_sequence(int sequence)
	{
		return &m_commands[sequence % 150];
	}

};