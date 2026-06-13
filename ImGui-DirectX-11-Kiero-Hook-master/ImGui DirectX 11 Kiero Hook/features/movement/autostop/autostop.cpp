#include "../../features.h"
#include "../../../sdk/c_csplayerpawn.hpp"
#include "../../../sdk/ctx.hpp"

constexpr double A_PI = 3.14159265358979323846;
constexpr double A_2PI = A_PI * 2.f;
constexpr double A_HALFPI = A_PI / 2.f;

constexpr double A_RAD2DEG = 180.0 / A_PI;
constexpr double A_DEG2RAD = A_PI / 180.0;

Vec3 calculate_angle(const Vec3& origin, const Vec3& destination) {
	Vec3 delta = destination - origin;
	float hypotenuse = std::sqrt(delta.x * delta.x + delta.y * delta.y);

	Vec3 angles;
	angles.x = -std::atan2(delta.z, hypotenuse) * A_RAD2DEG; // pitch
	angles.y = std::atan2(delta.y, delta.x) * A_RAD2DEG;     // yaw
	angles.z = 0.0f;

	return angles;
}

void fix_cmd_buttons(CUserCmd* pCmd)
{
	if (!MEM::GetLocalPawn() || !pCmd || !pCmd->csgoUserCmd.pBaseCmd)
		return;

	if (!(((C_CSPlayerPawn*)MEM::GetLocalPawn())->m_fFlags() & FL_ONGROUND))
		return;

	CBaseUserCmdPB* pBaseCmd = pCmd->csgoUserCmd.pBaseCmd;
	CInButtonStatePB* pButtons = pBaseCmd->pInButtonState;
	if (!pButtons)
		return;

	if (pBaseCmd->flForwardMove > 0.0f)
		pButtons->set_button_state(IN_FORWARD, e_button_state::in_button_down);
	else if (pBaseCmd->flForwardMove < 0.0f)
		pButtons->set_button_state(IN_BACK, e_button_state::in_button_down);

	if (pBaseCmd->flSideMove > 0.0f)
		pButtons->set_button_state(IN_MOVERIGHT, e_button_state::in_button_down);
	else if (pBaseCmd->flSideMove < 0.0f)
		pButtons->set_button_state(IN_MOVELEFT, e_button_state::in_button_down);
}

void FEATURES::MOVEMENT::AUTOSTOP::onMove(CUserCmd* pCmd, bool want) {
	static bool wants_stop = want;

	//static auto weapon_accuracy_nospread = g_interfaces->m_engine_convar->find_by_name("weapon_accuracy_nospread");
	//if (weapon_accuracy_nospread->get_bool())
	//	return;


	if (!want) {
		return;
	}

	if (!context->localPawn) {
		return;
	}

	Vec3 velocity = context->localPawn->m_vecVelocity();

	if (velocity.Length_2d() < 1.0f) {
		return;
	}

	if (!(context->localPawn->m_fFlags() & FL_ONGROUND)) {
		return;
	}

	if (pCmd->nButtons.nValue & IN_JUMP) {
		return;
	}

	auto base = pCmd->csgoUserCmd.pBaseCmd;
	if (!base) {
		return;
	}

	QAngle m_camera_angle = pCmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue;

	// furryware ground autostop: counter-strafe by rotating a full forward move against
	// the velocity direction. No walk_move � directly drive forward/side move.
	base->set_side_move(0.f);
	base->set_forward_move(velocity.Length_2d() > 20.0f ? 1.0f : 0.0f);

	const float yaw = calculate_angle(Vec3(0.f, 0.f, 0.f), velocity).y + 180.0f;
	const float rotation = (m_camera_angle.y - yaw) * (A_PI / 180.0f);

	const float cos_rotation = cosf(rotation);
	const float sin_rotation = sinf(rotation);

	const float forwardmove = cos_rotation * base->flForwardMove - sin_rotation * base->flSideMove;
	const float sidemove = sin_rotation * base->flForwardMove + cos_rotation * base->flSideMove;

	base->set_forward_move(forwardmove);
	base->set_side_move(-sidemove);

	fix_cmd_buttons(pCmd);
}
