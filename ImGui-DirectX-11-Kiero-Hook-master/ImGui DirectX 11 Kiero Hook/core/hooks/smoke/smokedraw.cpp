#include "smokedraw.h"
#include "../pattern.h"
#include "../../../kiero/minhook/include/MinHook.h"
#include "../../../state.hpp"
#include "../../mem.hpp"
#include <string>
#include "../../../features/cfg.hpp"
#include "../../../features/features.h"


void __fastcall hkProcessSmoke(__int64 a1)
{
    if (!g_state.running || g_state.unloading) {
        oProcessSmoke(a1);
        
    }
    
    FEATURES::VISUAL::WORLD::SMOKE::onDrawSmoke(a1);
}



void Hook_ProcessSmoke()
{
	uintptr_t addr = PatternScan(
		"client.dll",
		"83 B9 ? ? ? ? ? 74 11"
	);
	if (!addr)
	{
		return;
	}
    MH_CreateHook(
        reinterpret_cast<void*>(addr),
        &hkProcessSmoke,
        reinterpret_cast<void**>(&oProcessSmoke)
    );

    MH_EnableHook(reinterpret_cast<void*>(addr));
}