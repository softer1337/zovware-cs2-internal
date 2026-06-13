#include "tonemapupdate.h"
#include "../hooks.h"
#include "../../../kiero/minhook/include/MinHook.h"
#include "../pattern.h"
#include <cstdint>
#include "../../../state.hpp"
#include "../../../features/features.h"


float* hkTonemapUpdate(__int64 a1, __int64 a2, __int64 a3) {
	if (!g_state.running || g_state.unloading)
		return oTonemapUpdate(a1, a2, a3);

	FEATURES::VISUAL::WORLD::WORLDCOLOR::onTonemapUpdate(a1);

	return oTonemapUpdate(a1, a2, a3);
}

void Hook_tonemapUpdate() {
	uintptr_t addr = PatternScan("scenesystem.dll", TONEMAPUPDATE_PATTERN);
	if (!addr)
	{
		return;
	}
	MH_CreateHook(reinterpret_cast<void*>(addr), &hkTonemapUpdate, reinterpret_cast<void**>(&oTonemapUpdate));
	MH_EnableHook(reinterpret_cast<void*>(addr));
}