#include "framestage.h"
#include "../../mem.hpp"
#include "../../../sdk/interface.h"
#include <thread>
#include "../../../state.hpp"
#include "../../../features/features.h"
#include "../../../kiero/minhook/include/MinHook.h"
#include "../hooks.h"

using FrameStageNotify = __int64(__fastcall*)(__int64, int);
FrameStageNotify oFrameStageNotify = nullptr;

__int64 __fastcall hkFrameStageNotify(__int64 a1, int a2)
{
    if (!g_state.running || g_state.unloading)
        return oFrameStageNotify(a1, a2);

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