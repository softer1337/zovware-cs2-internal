#include "framestage.h"
#include "../../mem.hpp"
#include "../../../sdk/interface.h"
#include <thread>
#include <cstdio>
#include <cstdarg>
#include "../../../state.hpp"
#include "../../../features/features.h"
#include "../../../features/entity/lag comp/lag_comp.h"
#include "../../../kiero/minhook/include/MinHook.h"
#include "../hooks.h"

// Simple logging helper
inline void debug_log(const char* fmt, ...) {

}

using FrameStageNotify = __int64(__fastcall*)(__int64, int);
FrameStageNotify oFrameStageNotify = nullptr;

__int64 __fastcall hkFrameStageNotify(__int64 a1, int a2)
{
    if (!g_state.running || g_state.unloading)
        return oFrameStageNotify(a1, a2);

    // Update lag compensation first (must be before all other targeting features)
    if (a2 == 5) {
        debug_log("[FrameStage] Stage 5 detected, running LagCompensation");
        LagCompensation->run();
        debug_log("[FrameStage] LagCompensation->run() completed");
    }

    FEATURES::VISUAL::MODELCHANGER::onFrameStage(a2);
	FEATURES::SKINS::onFrameStage(a2);
	FEATURES::VISUAL::WORLD::PARTICLES::onFrameStage(a2);
	FEATURES::VISUAL::WORLD::SKYBOX::change_skybox_material();

    oFrameStageNotify(a1, a2);
}

void Hook_frameStage()
{
	uintptr_t addr = PatternScan("client.dll", FRAMESTAGENOTIFY_PATTERN);
	if (!addr)
	{
		return;
	}
	MH_CreateHook(reinterpret_cast<void*>(addr), &hkFrameStageNotify, reinterpret_cast<void**>(&oFrameStageNotify));
	MH_EnableHook(reinterpret_cast<void*>(addr));
}