#include "lightupdate.h"
#include "../pattern.h"
#include "../hooks.h"
#include "../../../kiero/minhook/include/MinHook.h"
#include "../../../state.hpp"
#include "../../../features/features.h"

__int64 hkUpdateLightScene(__int64 thisptr, C_LightScene* object, __int64 a3) {
    if (!g_state.running || g_state.unloading)
        return oUpdateLightScene_t(thisptr, object, a3);

    return FEATURES::VISUAL::WORLD::WORLDCOLOR::onUpdateLightScene(thisptr, object, a3);    
}





void Hook_updateLightScene()
{
    uintptr_t addr = PatternScan("scenesystem.dll", UPDATELIGHTOBJECT_PATTERN);

    if (!addr)
    {
        return;
    }

    MH_CreateHook(reinterpret_cast<void*>(addr), &hkUpdateLightScene, reinterpret_cast<void**>(&oUpdateLightScene_t));
    MH_EnableHook(reinterpret_cast<void*>(addr));
}
