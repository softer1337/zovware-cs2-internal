//#include "../../features.h"
//#include "../../../sdk/c_csplayerpawn.hpp"
//#include "../../../sdk/ctx.hpp"
//#include "../../../sdk/cvar.hpp"
//#include <algorithm>
//#include "../../features.h"
//
//
//auto rotate_movement = [](float target_yaw) {
//	auto base_cmd = context->pCmd->csgoUserCmd.pBaseCmd;
//	float rad_rotation = DEG2RAD(base_cmd->pViewAngles->angValue.y - target_yaw);
//
//	float new_forward_move = cosf(rad_rotation) * base_cmd->flForwardMove - sinf(rad_rotation) * base_cmd->flSideMove;
//	float new_side_move = sinf(rad_rotation) * base_cmd->flForwardMove + cosf(rad_rotation) * base_cmd->flSideMove;
//
//	base_cmd->set_forward_move(std::clamp(new_forward_move, -1.f, 1.f));
//	base_cmd->set_side_move(std::clamp(new_side_move * -1.f, -1.f, 1.f));
//	};
//
//
//void air_strafer(Vec3& velocity, float frame_time, Vec3 move) {
//	static auto sv_airaccelerate = Interface::GetConVars()->find_by_name("sv_airaccelerate");
//	static auto sv_air_max_wishspeed = Interface::GetConVars()->find_by_name("sv_air_max_wishspeed");
//
//	auto base_cmd = context->pCmd->csgoUserCmd.pBaseCmd;
//	auto local_pawn = context->localPawn;
//	auto move_service = local_pawn->m_pMovementServices();
//
//	static uint64_t last_pressed = 0;
//	static uint64_t last_buttons = 0;
//
//	uint64_t current_buttons = context->pCmd->nButtons.nValue;
//
//	auto check_button = [&](uint64_t button) {
//		if (current_buttons & button && (!(last_buttons & button)
//			|| (button & IN_MOVELEFT && !(last_pressed & IN_MOVERIGHT))
//			|| (button & IN_MOVERIGHT && !(last_pressed & IN_MOVELEFT))
//			|| (button & IN_FORWARD && !(last_pressed & IN_BACK))
//			|| (button & IN_BACK && !(last_pressed & IN_FORWARD)))) {
//
//			// easy strafe, can be by checkbox
//			if (button & IN_MOVELEFT)
//				last_pressed &= ~IN_MOVERIGHT;
//			else if (button & IN_MOVERIGHT)
//				last_pressed &= ~IN_MOVELEFT;
//			else if (button & IN_FORWARD)
//				last_pressed &= ~IN_BACK;
//			else if (button & IN_BACK)
//				last_pressed &= ~IN_FORWARD;
//			// end easy strafe
//
//			last_pressed |= button;
//		}
//		else if (!(current_buttons & button))
//			last_pressed &= ~button;
//		};
//
//	check_button(IN_MOVELEFT);
//	check_button(IN_MOVERIGHT);
//	check_button(IN_FORWARD);
//	check_button(IN_BACK);
//
//	last_buttons = current_buttons;
//
//	auto move_type = local_pawn->m_nActualMoveType();
//	if (move_type == MOVETYPE_LADDER || move_type == MOVETYPE_NOCLIP /* || g_prediction->is_probably_on_ground() || wants_stop */ )
//		return;
//
//	auto velocity_angle = RAD2DEG(std::atan2f(velocity.y, velocity.x));
//
//	if (velocity_angle < 0.0f)
//		velocity_angle += 360.0f;
//
//	if (velocity_angle < 0.0f)
//		velocity_angle += 360.0f;
//
//	velocity_angle -= floorf(velocity_angle / 360.0f + 0.5f) * 360.0f;
//	float speed = velocity.Length_2d();
//
//	float offset = 0.f;
//
//	if (last_pressed & IN_MOVELEFT)
//		offset = 90.f;
//	if (last_pressed & IN_MOVERIGHT)
//		offset = -90.f;
//
//	if (last_pressed & IN_FORWARD)
//		offset *= 0.5f;
//	else if (last_pressed & IN_BACK)
//		offset = -offset * 0.5f + 180.f;
//
//	QAngle m_camera_angle = context->pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue;
//
//	float yaw = normalize_float(m_camera_angle.y);
//	yaw += offset;
//
//	base_cmd->set_forward_move(0.f);
//	base_cmd->set_side_move(0.f);
//
//	float ideal = std::clamp(
//		RAD2DEG(
//			atanf(
//				std::fmax(15.f, sv_air_max_wishspeed->get_float() - (
//					speed *
//					sv_airaccelerate->get_float() *
//					context->localPawn->m_pMovementServices()->m_flSurfaceFriction() *
//					frame_time
//					)) / speed
//			)
//		),
//		0.f,
//		45.f
//	);
//
//	const auto correct = std::clamp(100.f * (1.0f / frame_time), 50.f, 200.f);
//
//	float velocity_delta = normalize_float(yaw - velocity_angle);
//	static bool side_switch = false;
//	side_switch = !side_switch;
//
//	if (-correct <= velocity_delta || speed <= 80.f) {
//		if (side_switch) {
//			yaw -= ideal;
//			context->pCmd->csgoUserCmd.pBaseCmd->set_side_move(-1.f);
//		}
//		else {
//			yaw += ideal;
//			context->pCmd->csgoUserCmd.pBaseCmd->set_side_move(1.f);
//		}
//		rotate_movement(normalize_float(yaw));
//	}
//	else {
//		yaw = velocity_angle - correct;
//		context->pCmd->csgoUserCmd.pBaseCmd->set_side_move(1.f);
//
//		rotate_movement(normalize_float(yaw));
//	}
//
//	//if ((fabsf(velocity_delta) > 170.f || velocity_delta > ideal) && speed > 80.f) {
//	//	yaw = ideal + velocity_angle;
//	//	base_cmd->set_side_move(-1.f);
//	//}
//	//else {
//	//	if (-ideal <= velocity_delta || speed <= 80.f) {
//	//		if (side_switch) {
//	//			yaw -= ideal;
//	//			base_cmd->set_side_move(-1.f);
//	//		}
//	//		else {
//	//			yaw += ideal;
//	//			base_cmd->set_side_move(1.f);
//	//		}
//	//	}
//	//	else {
//	//		yaw = velocity_angle - ideal;
//	//		base_cmd->set_side_move(1.f);
//	//	}
//	//}
//
//	//rotate_movement(g_math->normalize_float(yaw));
//}
//
//void air_accelerate(Vec3 angles, Vec3& velocity, Vec3& move, float frame_time) {
//	static auto sv_airaccelerate = Interface::GetConVars()->find_by_name("sv_airaccelerate");
//	static auto sv_air_max_wishspeed = Interface::GetConVars()->find_by_name("sv_air_max_wishspeed");
//
//	Vec3 forward, right, up;
//	angles.to_directions(&forward, &right, &up);
//
//	forward.z = 0.0f;
//	right.z = 0.0f;
//	forward.NormalizeInPlace();
//	right.NormalizeInPlace();
//
//	Vec3 wishDir;
//	wishDir.x = (forward.x * move.x) + (right.x * move.y);
//	wishDir.y = (forward.y * move.x) + (right.y * move.y);
//	wishDir.z = 0.0f;
//
//	float wishSpeed = wishDir.NormalizeInPlace();
//	wishSpeed = std::fminf(wishSpeed, sv_air_max_wishspeed->get_float());
//
//	float currentSpeed = velocity.dot(wishDir);
//	float speedAdd = wishSpeed - currentSpeed;
//
//	if (speedAdd <= 0.0f)
//		return;
//
//	float accelLimit = sv_airaccelerate->get_float() * wishSpeed * frame_time;
//
//	if (accelLimit > speedAdd)
//		accelLimit = speedAdd;
//
//	velocity.x += wishDir.x * accelLimit;
//	velocity.y += wishDir.y * accelLimit;
//
//	if (context->localPawn->m_pMovementServices()->m_flStamina() > 70.0f)
//		velocity.z += 0.3f * frame_time;
//}
//
//void FEATURES::MOVEMENT::STRAFE::subtick_air_strafer(bool stop) {
//
//	if (!context->pCmd)
//		return;
//
//	C_CSPlayerPawn* local_pawn = context->localPawn;
//	auto move_type = local_pawn->m_nActualMoveType();
//
//	if (move_type == MOVETYPE_LADDER || move_type == MOVETYPE_NOCLIP /* || g_prediction->is_probably_on_ground()*/)
//		return;
//
//	const uint64_t move_keys = IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT;
//	if (!(context->pCmd->nButtons.nValue & move_keys))
//		return;
//
//	CPlayer_MovementServices* move_service = local_pawn->m_pMovementServices();
//	CBaseUserCmdPB* base = context->pCmd->csgoUserCmd.pBaseCmd;
//
//	CUserCmd* last_command = context->pCmdManager->get_cmd_by_sequence(context->pCmd->m_sequence_number - 1);
//	CBaseUserCmdPB* last_base = last_command->csgoUserCmd.pBaseCmd;
//
//	Vec3 last_move = { last_base->flForwardMove, last_base->flSideMove, last_base->flUpMove };
//
//	int ticks_to_simulate = std::clamp(32 - base->subtickMovesField.nCurrentSize, 0, 32);
//	float subtick_frame_time = interval_per_tick / (float)ticks_to_simulate;
//
//	Vec3 abs_velocity = local_pawn->m_vecAbsVelocity();
//
//	Vec3 vec_last = Vec3(base->pViewAngles->angValue.x, base->pViewAngles->angValue.y, base->pViewAngles->angValue.z);
//
//	for (int i = 0; i < ticks_to_simulate; ++i) {
//		air_accelerate(Vec3(base->pViewAngles->angValue.x, base->pViewAngles->angValue.y, base->pViewAngles->angValue.z), abs_velocity, last_move, subtick_frame_time);
//		air_strafer(abs_velocity, subtick_frame_time, last_move);
//
//		CSubtickMoveStep* subtick = base->create_subtick_move();
//		if (!subtick)
//			return;
//
//		subtick->set_when((float)i / (float)ticks_to_simulate);
//
//		subtick->set_analog_forward_delta(base->flForwardMove - last_move.x);
//		subtick->set_analog_left_delta(base->flSideMove - last_move.y);
//
//		subtick->set_analog_pitch_delta(base->pViewAngles->angValue.x - vec_last.x);
//		subtick->set_analog_yaw_delta(base->pViewAngles->angValue.y - vec_last.y);
//
//		vec_last = Vec3(base->pViewAngles->angValue.x, base->pViewAngles->angValue.y, base->pViewAngles->angValue.z);
//
//		last_move.x += subtick->flAnalogForwardDelta;
//		last_move.y += subtick->flAnalogLeftDelta;
//	}
//
//	auto& buttons = context->pCmd->nButtons.nValue;
//	bool player_holds_side = (buttons & (IN_MOVELEFT | IN_MOVERIGHT)) != 0;
//	if (!player_holds_side) {
//		if (base->flSideMove > 0.0f)
//			buttons |= IN_MOVERIGHT;
//		else if (base->flSideMove < 0.0f)
//			buttons |= IN_MOVELEFT;
//	}
//
//	bool player_holds_fwd = (buttons & (IN_FORWARD | IN_BACK)) != 0;
//	if (!player_holds_fwd) {
//		if (base->flForwardMove > 0.0f)
//			buttons |= IN_FORWARD;
//		else if (base->flForwardMove < 0.0f)
//			buttons |= IN_BACK;
//	}
//}

#include "../../features.h"
#include "../../../sdk/c_csplayerpawn.hpp"
#include "../../../sdk/ctx.hpp"
#include "../../../sdk/cvar.hpp"
#include <algorithm>

// ������ ��� ����������� ������ (�������� � � �������, � � ���������)
#define AIR_STRAFE_LOG(fmt, ...) ((void)0)



auto rotate_movement = [](float target_yaw) {
	auto base_cmd = context->pCmd->csgoUserCmd.pBaseCmd;
	float rad_rotation = DEG2RAD(base_cmd->pViewAngles->angValue.y - target_yaw);

	float new_forward_move = cosf(rad_rotation) * base_cmd->flForwardMove - sinf(rad_rotation) * base_cmd->flSideMove;
	float new_side_move = sinf(rad_rotation) * base_cmd->flForwardMove + cosf(rad_rotation) * base_cmd->flSideMove;

	base_cmd->set_forward_move(std::clamp(new_forward_move, -1.f, 1.f));
	base_cmd->set_side_move(std::clamp(new_side_move * -1.f, -1.f, 1.f));
	};

void air_strafer(Vec3& velocity, float frame_time, Vec3 move) {
	static auto sv_airaccelerate = Interface::GetConVars()->find_by_name("sv_airaccelerate");
	static auto sv_air_max_wishspeed = Interface::GetConVars()->find_by_name("sv_air_max_wishspeed");

	auto base_cmd = context->pCmd->csgoUserCmd.pBaseCmd;
	auto local_pawn = context->localPawn;
	auto move_service = local_pawn->m_pMovementServices();

	static uint64_t last_pressed = 0;
	static uint64_t last_buttons = 0;

	uint64_t current_buttons = context->pCmd->nButtons.nValue;

	auto check_button = [&](uint64_t button) {
		if (current_buttons & button && (!(last_buttons & button)
			|| (button & IN_MOVELEFT && !(last_pressed & IN_MOVERIGHT))
			|| (button & IN_MOVERIGHT && !(last_pressed & IN_MOVELEFT))
			|| (button & IN_FORWARD && !(last_pressed & IN_BACK))
			|| (button & IN_BACK && !(last_pressed & IN_FORWARD)))) {

			// easy strafe, can be by checkbox
			if (button & IN_MOVELEFT)
				last_pressed &= ~IN_MOVERIGHT;
			else if (button & IN_MOVERIGHT)
				last_pressed &= ~IN_MOVELEFT;
			else if (button & IN_FORWARD)
				last_pressed &= ~IN_BACK;
			else if (button & IN_BACK)
				last_pressed &= ~IN_FORWARD;
			// end easy strafe

			last_pressed |= button;
		}
		else if (!(current_buttons & button))
			last_pressed &= ~button;
		};

	check_button(IN_MOVELEFT);
	check_button(IN_MOVERIGHT);
	check_button(IN_FORWARD);
	check_button(IN_BACK);

	last_buttons = current_buttons;

	auto move_type = local_pawn->m_nActualMoveType();
	if (move_type == MOVETYPE_LADDER || move_type == MOVETYPE_NOCLIP)
		return;

	auto velocity_angle = RAD2DEG(std::atan2f(velocity.y, velocity.x));

	if (velocity_angle < 0.0f)
		velocity_angle += 360.0f;

	if (velocity_angle < 0.0f)
		velocity_angle += 360.0f;

	velocity_angle -= floorf(velocity_angle / 360.0f + 0.5f) * 360.0f;
	float speed = velocity.Length_2d();

	float offset = 0.f;
	if (last_pressed & IN_MOVELEFT) offset = 90.f;
	if (last_pressed & IN_MOVERIGHT) offset = -90.f;
	if (last_pressed & IN_FORWARD) offset *= 0.5f;
	else if (last_pressed & IN_BACK) offset = -offset * 0.5f + 180.f;

	QAngle m_camera_angle = base_cmd->pViewAngles->angValue;
	float yaw = normalize_float(m_camera_angle.y);
	yaw += offset;

	base_cmd->set_forward_move(0.f);
	base_cmd->set_side_move(0.f);

	float ideal = std::clamp(
		RAD2DEG(
			atanf(
				std::fmax(15.f, sv_air_max_wishspeed->get_float() - (
					speed *
					sv_airaccelerate->get_float() *
					move_service->m_flSurfaceFriction() *
					frame_time
					)) / speed
			)
		),
		0.f,
		45.f
	);

	const auto correct = std::clamp(100.f * (1.0f / frame_time), 50.f, 200.f);

	float velocity_delta = normalize_float(yaw - velocity_angle);
	static bool side_switch = false;
	side_switch = !side_switch;

	if (-correct <= velocity_delta || speed <= 80.f) {
		if (side_switch) {
			yaw -= ideal;
			base_cmd->set_side_move(-1.f);
		}
		else {
			yaw += ideal;
			base_cmd->set_side_move(1.f);
		}
		rotate_movement(normalize_float(yaw));
	}
	else {
		yaw = velocity_angle - correct;
		base_cmd->set_side_move(1.f);
		rotate_movement(normalize_float(yaw));
	}
}

void air_accelerate(Vec3 angles, Vec3& velocity, Vec3& move, float frame_time) {
	static auto sv_airaccelerate = Interface::GetConVars()->find_by_name("sv_airaccelerate");
	static auto sv_air_max_wishspeed = Interface::GetConVars()->find_by_name("sv_air_max_wishspeed");

	Vec3 forward, right, up;
	angles.to_directions(&forward, &right, &up);

	forward.z = 0.0f;
	right.z = 0.0f;
	forward.NormalizeInPlace();
	right.NormalizeInPlace();

	Vec3 wishDir;
	wishDir.x = (forward.x * move.x) + (right.x * move.y);
	wishDir.y = (forward.y * move.x) + (right.y * move.y);
	wishDir.z = 0.0f;

	float wishSpeed = wishDir.NormalizeInPlace();
	wishSpeed = std::fminf(wishSpeed, sv_air_max_wishspeed->get_float());

	float currentSpeed = velocity.dot(wishDir);
	float speedAdd = wishSpeed - currentSpeed;

	if (speedAdd <= 0.0f)
		return;

	float accelLimit = sv_airaccelerate->get_float() * wishSpeed * frame_time;

	if (accelLimit > speedAdd)
		accelLimit = speedAdd;

	velocity.x += wishDir.x * accelLimit;
	velocity.y += wishDir.y * accelLimit;

	if (context->localPawn->m_pMovementServices()->m_flStamina() > 70.0f)
		velocity.z += 0.3f * frame_time;
}

void FEATURES::MOVEMENT::STRAFE::subtick_air_strafer(bool stop) {
	if (!CFG::MOVEMENT::isAutoStraferEnabled) {
		return;
	}

	if (!context->pCmd)
		return;

	C_CSPlayerPawn* local_pawn = context->localPawn;
	auto move_type = local_pawn->m_nActualMoveType();

	if (move_type == MOVETYPE_LADDER || move_type == MOVETYPE_NOCLIP)
		return;

	const uint64_t move_keys = IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT;
	if (!(context->pCmd->nButtons.nValue & move_keys))
		return;

	CBaseUserCmdPB* base = context->pCmd->csgoUserCmd.pBaseCmd;

	CUserCmd* last_command = context->pCmdManager->get_cmd_by_sequence(context->pCmd->m_sequence_number - 1);
	CBaseUserCmdPB* last_base = last_command->csgoUserCmd.pBaseCmd;

	Vec3 last_move = { last_base->flForwardMove, last_base->flSideMove, last_base->flUpMove };

	int ticks_to_simulate = std::clamp(32 - base->subtickMovesField.nCurrentSize, 0, 32);
	float subtick_frame_time = interval_per_tick / (float)ticks_to_simulate;

	Vec3 abs_velocity = local_pawn->m_vecAbsVelocity();

	Vec3 vec_last = Vec3(base->pViewAngles->angValue.x, base->pViewAngles->angValue.y, base->pViewAngles->angValue.z);

	for (int i = 0; i < ticks_to_simulate; ++i) {
		air_accelerate(Vec3(base->pViewAngles->angValue.x, base->pViewAngles->angValue.y, base->pViewAngles->angValue.z), abs_velocity, last_move, subtick_frame_time);
		air_strafer(abs_velocity, subtick_frame_time, last_move);

		CSubtickMoveStep* subtick = base->create_subtick_move();
		if (!subtick)
			return;

		subtick->set_when((float)i / (float)ticks_to_simulate);

		subtick->set_analog_forward_delta(base->flForwardMove - last_move.x);
		subtick->set_analog_left_delta(base->flSideMove - last_move.y);

		subtick->set_analog_pitch_delta(base->pViewAngles->angValue.x - vec_last.x);
		subtick->set_analog_yaw_delta(base->pViewAngles->angValue.y - vec_last.y);

		vec_last = Vec3(base->pViewAngles->angValue.x, base->pViewAngles->angValue.y, base->pViewAngles->angValue.z);

		last_move.x += subtick->flAnalogForwardDelta;
		last_move.y += subtick->flAnalogLeftDelta;
	}

	auto& buttons = context->pCmd->nButtons.nValue;
	bool player_holds_side = (buttons & (IN_MOVELEFT | IN_MOVERIGHT)) != 0;
	if (!player_holds_side) {
		if (base->flSideMove > 0.0f)
			buttons |= IN_MOVERIGHT;
		else if (base->flSideMove < 0.0f)
			buttons |= IN_MOVELEFT;
	}

	bool player_holds_fwd = (buttons & (IN_FORWARD | IN_BACK)) != 0;
	if (!player_holds_fwd) {
		if (base->flForwardMove > 0.0f)
			buttons |= IN_FORWARD;
		else if (base->flForwardMove < 0.0f)
			buttons |= IN_BACK;
	}
}