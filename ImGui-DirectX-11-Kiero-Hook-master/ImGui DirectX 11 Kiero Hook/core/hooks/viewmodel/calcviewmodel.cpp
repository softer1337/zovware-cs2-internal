#include "calcviewmodel.h"
#include "../../../state.hpp"
#include "../pattern.h"
#include "../../../kiero/minhook/include/MinHook.h"
#include "../hooks.h"
#include "../../../features/features.h"


void* __fastcall hkCalcViewmodel(float* flUnk, float* flOffsets, float* flFov)
{
	if (!g_state.running || g_state.unloading)
		return oCalcViewModel(flUnk, flOffsets, flFov);
	
	void* ret = oCalcViewModel(flUnk, flOffsets, flFov);

	FEATURES::VISUAL::VIEWMODEL::onCalcViewModel(flUnk, flOffsets, flFov);

	return ret;

}

void Hook_CalcViewModel()
{
	uintptr_t addr = PatternScan("client.dll", CALCVIEWMODEL_PATTERN);
	if (!addr)
	{
		return;
	}
	MH_CreateHook(reinterpret_cast<void*>(addr), &hkCalcViewmodel, reinterpret_cast<void**>(&oCalcViewModel));
	MH_EnableHook(reinterpret_cast<void*>(addr));
}