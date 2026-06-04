#include "drawarray.h"


bool __fastcall hkDrawArray(CSceneAnimatableObjectDesc* desc, void* a2, CMeshDrawPrimitive_t* mesh_draw, int a4, void* a5, void* a6, void* a7)
{
    if (!g_state.running || g_state.unloading)
        return oDrawArray(desc, a2, mesh_draw, a4, a5, a6, a7);

    FEATURES::VISUAL::CHAMS::onDrawArray(desc, a2, mesh_draw, a4, a5, a6, a7);
    return oDrawArray(desc, a2, mesh_draw, a4, a5, a6, a7);
}

void Hook_drawarray() {
    uintptr_t addr = PatternScan("scenesystem.dll", "48 8B C4 53 57 41 54");

    if (!addr)
    {
  
        return;
    }

    MH_CreateHook(reinterpret_cast<void*>(addr), &hkDrawArray, reinterpret_cast<void**>(&oDrawArray));
    MH_EnableHook(reinterpret_cast<void*>(addr));
}