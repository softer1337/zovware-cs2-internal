#include "drawaggregate.h"
#include "../pattern.h"
#include "../../../kiero/minhook/include/MinHook.h"
#include "../hooks.h"
#include "../../../state.hpp"
#include "../../../features/features.h"

__int64 hkDrawAggregateSceneObject(__int64 a1, __int64 a2, CAggregateObjectArray* a3)
{
	if (!g_state.running || g_state.unloading)
		return oDrawAggregateSceneObjectArray(a1, a2, a3);


	FEATURES::VISUAL::WORLD::WORLDCOLOR::onDrawAggregateSceneObject(a1, a2, a3);


}

void Hook_drawAggregateSceneObject()
{
	uintptr_t addr = PatternScan("scenesystem.dll", DRAWAGGREGATESCENEOBJECTARRAY_PATTERN);
	if (!addr)
	{
		return;
	}
	MH_CreateHook(reinterpret_cast<void*>(addr), &hkDrawAggregateSceneObject, reinterpret_cast<void**>(&oDrawAggregateSceneObjectArray));
	MH_EnableHook(reinterpret_cast<void*>(addr));
}