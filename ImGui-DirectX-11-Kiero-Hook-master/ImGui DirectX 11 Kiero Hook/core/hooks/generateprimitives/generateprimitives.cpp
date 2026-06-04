#include "generateprimitives.h"
#include "../../../state.hpp"
#include "../../../features/features.h"
#include "../../../kiero/minhook/include/MinHook.h"
#include "../hooks.h"

__int64 __fastcall hkGeneratePrimitives(
    void* __this,
    CSceneAnimatableObject* object,
    void* a3,
    CMeshPrimitiveOutputBuffer* render_buf)
{
    if (!g_state.running || g_state.unloading)
        return (oGeneratePrimitives(__this, object, a3, render_buf));

    if (FEATURES::VISUAL::CHAMS::onGeneratePrimitives(object, a3, render_buf, __this))
        return 0;

    return (oGeneratePrimitives(__this, object, a3, render_buf));
}

void Hook_generatePrimitives() {
	uintptr_t addr = PatternScan("scenesystem.dll", GENERATEPRIMITIVES_PATTERN);
	if (!addr)
	{
		return;
	}
	MH_CreateHook(reinterpret_cast<void*>(addr), &hkGeneratePrimitives, reinterpret_cast<void**>(&oGeneratePrimitives));
	MH_EnableHook(reinterpret_cast<void*>(addr));
}