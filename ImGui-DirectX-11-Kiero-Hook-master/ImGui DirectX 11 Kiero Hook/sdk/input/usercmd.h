#pragma once
#define NOMINMAX
#define _HAS_STD_BYTE 0
#undef max
#include <Windows.h>
#include <cstdint>
#include <limits>
#include <string>
#include "../qangle.h"


#define MULTIPLAYER_BACKUP 150
#define FL_ONGROUND             (1 << 0)   // 1
#define FL_DUCKING              (1 << 1)   // 2
#define FL_WATERJUMP            (1 << 2)   // 4
#define FL_ON_TRAIN             (1 << 3)   // 8
#define FL_IN_RAIN              (1 << 4)   // 16
#define FL_FROZEN               (1 << 5)   // 32
#define FL_AT_CONTROLS          (1 << 6)   // 64
#define FL_CLIENT               (1 << 7)   // 128
#define FL_FAKE_CLIENT          (1 << 8)   // 256


enum ECommandButtons : int
{
	IN_ATTACK = (1 << 0),
	IN_JUMP = (1 << 1),
	IN_DUCK = (1 << 2),
	IN_FORWARD = (1 << 3),
	IN_BACK = (1 << 4),
	IN_USE = (1 << 5),
	IN_CANCEL = (1 << 6),
	IN_LEFT = (1 << 7),
	IN_RIGHT = (1 << 8),
	IN_MOVELEFT = (1 << 9),
	IN_MOVERIGHT = (1 << 10),
	IN_SECOND_ATTACK = (1 << 11),
	IN_RUN = (1 << 12),
	IN_RELOAD = (1 << 13),
	IN_LEFT_ALT = (1 << 14),
	IN_RIGHT_ALT = (1 << 15),
	IN_SCORE = (1 << 16),
	IN_SPEED = (1 << 17),
	IN_WALK = (1 << 18),
	IN_ZOOM = (1 << 19),
	IN_FIRST_WEAPON = (1 << 20),
	IN_SECOND_WEAPON = (1 << 21),
	IN_BULLRUSH = (1 << 22),
	IN_FIRST_GRENADE = (1 << 23),
	IN_SECOND_GRENADE = (1 << 24),
	IN_MIDDLE_ATTACK = (1 << 25),
	IN_USE_OR_RELOAD = (1 << 26)
};

enum ESubtickMoveStepBits : std::uint32_t
{
	MOVESTEP_BITS_BUTTON = 0x1U,
	MOVESTEP_BITS_PRESSED = 0x2U,
	MOVESTEP_BITS_WHEN = 0x4U,
	MOVESTEP_BITS_ANALOG_FORWARD_DELTA = 0x8U,
	MOVESTEP_BITS_ANALOG_LEFT_DELTA = 0x10U
};

enum EInputHistoryBits : std::uint32_t
{
	INPUT_HISTORY_BITS_VIEWANGLES = 0x1U,
	INPUT_HISTORY_BITS_SHOOTPOSITION = 0x2U,
	INPUT_HISTORY_BITS_TARGETHEADPOSITIONCHECK = 0x4U,
	INPUT_HISTORY_BITS_TARGETABSPOSITIONCHECK = 0x8U,
	INPUT_HISTORY_BITS_TARGETANGCHECK = 0x10U,
	INPUT_HISTORY_BITS_CL_INTERP = 0x20U,
	INPUT_HISTORY_BITS_SV_INTERP0 = 0x40U,
	INPUT_HISTORY_BITS_SV_INTERP1 = 0x80U,
	INPUT_HISTORY_BITS_PLAYER_INTERP = 0x100U,
	INPUT_HISTORY_BITS_RENDERTICKCOUNT = 0x200U,
	INPUT_HISTORY_BITS_RENDERTICKFRACTION = 0x400U,
	INPUT_HISTORY_BITS_PLAYERTICKCOUNT = 0x800U,
	INPUT_HISTORY_BITS_PLAYERTICKFRACTION = 0x1000U,
	INPUT_HISTORY_BITS_FRAMENUMBER = 0x2000U,
	INPUT_HISTORY_BITS_TARGETENTINDEX = 0x4000U
};

enum EButtonStatePBBits : uint32_t
{
	BUTTON_STATE_PB_BITS_BUTTONSTATE1 = 0x1U,
	BUTTON_STATE_PB_BITS_BUTTONSTATE2 = 0x2U,
	BUTTON_STATE_PB_BITS_BUTTONSTATE3 = 0x4U
};

enum EBaseCmdBits : std::uint32_t
{
	BASE_BITS_MOVE_CRC = 0x1U,
	BASE_BITS_BUTTONPB = 0x2U,
	BASE_BITS_VIEWANGLES = 0x4U,
	BASE_BITS_COMMAND_NUMBER = 0x8U,
	BASE_BITS_CLIENT_TICK = 0x10U,
	BASE_BITS_FORWARDMOVE = 0x20U,
	BASE_BITS_LEFTMOVE = 0x40U,
	BASE_BITS_UPMOVE = 0x80U,
	BASE_BITS_IMPULSE = 0x100U,
	BASE_BITS_WEAPON_SELECT = 0x200U,
	BASE_BITS_RANDOM_SEED = 0x400U,
	BASE_BITS_MOUSEDX = 0x800U,
	BASE_BITS_MOUSEDY = 0x1000U,
	BASE_BITS_CONSUMED_SERVER_ANGLE = 0x2000U,
	BASE_BITS_CMD_FLAGS = 0x4000U,
	BASE_BITS_ENTITY_HANDLE = 0x8000U
};

enum ECSGOUserCmdBits : std::uint32_t
{
	CSGOUSERCMD_BITS_BASECMD = 0x1U,
	CSGOUSERCMD_BITS_LEFTHAND = 0x2U,
	CSGOUSERCMD_BITS_PREDICTING_BODY_SHOT = 0x4U,
	CSGOUSERCMD_BITS_PREDICTING_HEAD_SHOT = 0x8U,
	CSGOUSERCMD_BITS_PREDICTING_KILL_RAGDOLLS = 0x10U,
	CSGOUSERCMD_BITS_ATTACK3START = 0x20U,
	CSGOUSERCMD_BITS_ATTACK1START = 0x40U,
	CSGOUSERCMD_BITS_ATTACK2START = 0x80U
};

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
};

class CBasePB
{
public:
	BYTE pad01[0x8];
	std::uint32_t nHasBits; // 0x8
	std::uint64_t nCachedBits; // 0xC
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
};

struct CSubtickMoveStep : CBasePB
{
public:
	std::uint64_t nButton;
	bool bPressed;
	float flWhen;
	float flAnalogForwardDelta;
	float flAnalogLeftDelta;
};

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

}; //size: 0x68?

class CCSGOUserCmdPB
{
public:
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



struct CInButtonState
{
public:
	std::uint64_t nValue; // 0x8
	std::uint64_t nValueChanged; // 0x10
	std::uint64_t nValueScroll; // 0x18
};

class CUserCmd {
public:
	char pad01[0x8];
	char pad02[0x10];
	CCSGOUserCmdPB csgoUserCmd; // 0x18

	// 0x18 + sizeof(CCSGOUserCmdPB) = 0x60
	char pad_buttons[0x60 - 0x18 - sizeof(CCSGOUserCmdPB)];

	CInButtonState nButtons;

	char pad03[0x20];

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