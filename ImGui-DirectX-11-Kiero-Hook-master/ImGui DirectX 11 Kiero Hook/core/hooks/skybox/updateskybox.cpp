#include "updateskybox.h"
#include <cstdint>
#include "../../../features/features.h"
#include "../../../state.hpp"
#include "../../../kiero/minhook/include/MinHook.h"
#include "../hooks.h"


void __fastcall hkUpdateSkyBox(__int64 a1, __int64 a2, __int64 a3, int nCount, int render_flags, __int64 a6, __int64 a7)
{
    if (!g_state.running || g_state.unloading)
        return oUpdateSkyBox(a1, a2, a3, nCount, render_flags, a6, a7);

    FEATURES::VISUAL::WORLD::SKYBOX::onUpdateSkyBox(a1, a2, a3, nCount, render_flags, a6, a7);
}

void Hook_updateSkybox() {
    uintptr_t addr = PatternScan("scenesystem.dll", SKYBOXDRAWARRAY_PATTERN);

    if (!addr)
    {
        return;
    }

    MH_CreateHook(reinterpret_cast<void*>(addr), &hkUpdateSkyBox, reinterpret_cast<void**>(&oUpdateSkyBox));
    MH_EnableHook(reinterpret_cast<void*>(addr));
}