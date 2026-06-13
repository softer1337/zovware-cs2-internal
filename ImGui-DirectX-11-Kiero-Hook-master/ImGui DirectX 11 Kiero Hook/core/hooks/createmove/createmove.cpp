#include "createmove.h"
#include "../../../sdk/input/usercmd.h"
#include "../../../features/features.h"
#include "../../mem.hpp"
#include <thread>
#include "../../../state.hpp"
#include "../../../kiero/minhook/include/MinHook.h"
#include "../hooks.h"
#include "../../../sdk/ctx.hpp"

using CreateMoveFn = double(__fastcall*)(void* a1, unsigned int a2, CUserCmd* pCmd);
inline CreateMoveFn oCreateMove = nullptr;
double __fastcall hkCreateMove(void* a1, unsigned int a2, CUserCmd* pCmd)
{
    const auto res = oCreateMove(a1, a2, pCmd);

    if (!g_state.running || g_state.unloading)
        return res;

    if (!context->update())
        return res;

    FEATURES::MOVEMENT::BHOP::onMove(pCmd);
    FEATURES::AIM::LEGITBOT::onMove(pCmd);
	FEATURES::AIM::RAGEBOT::onMove(pCmd);
    FEATURES::MOVEMENT::AUTOSTOP::onMove(pCmd, CFG::AIM::RAGEBOT::internal_wantsStop);

    return res;
}

void Hook_createmove() {
    uintptr_t addr = PatternScan("client.dll", CREATEMOVE_PATTERN);

    if (!addr)
    {
        return;
    }

    MH_CreateHook(reinterpret_cast<void*>(addr), &hkCreateMove, reinterpret_cast<void**>(&oCreateMove));
    MH_EnableHook(reinterpret_cast<void*>(addr));
}