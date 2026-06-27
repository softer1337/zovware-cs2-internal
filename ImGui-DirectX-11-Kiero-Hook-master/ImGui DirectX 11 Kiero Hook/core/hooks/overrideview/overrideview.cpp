#include "overrideview.h"
#include "../hooks.h"
#include "../pattern.h"
#include "../../../kiero/minhook/include/MinHook.h"
#include "../../../state.hpp"
#include "../../../features/features.h"


void hkOverrideView(__int64 a1, CViewSetup* a2) {
    if (!g_state.running || g_state.unloading)
        return oOverrideView(a1, a2);

    FEATURES::VISUAL::WORLD::CAMERA::onOverrideMove(a1, a2);

    return oOverrideView(a1, a2);
}




void Hook_overrideView()
{
    uintptr_t addr = PatternScan("client.dll", OVERRIDEVIEW_PATTERN);

    if (!addr)
    {
        return;
    }

    MH_CreateHook(reinterpret_cast<void*>(addr), &hkOverrideView, reinterpret_cast<void**>(&oOverrideView));
    MH_EnableHook(reinterpret_cast<void*>(addr));
}
