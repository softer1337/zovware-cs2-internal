#include "entity.h"
#include "../../../state.hpp"
#include "../../../features/features.h"
#include "../../../kiero/minhook/include/MinHook.h"


__int64 hkOnAddEntity(__int64 thisptr, CEntityInstance* ent_identity, c_base_handle handle)
{
    if (!g_state.running || g_state.unloading)
        return oOnAddEntity(thisptr, ent_identity, handle);

    FEATURES::ENTITY::onAddEntity(handle, ent_identity);

    return oOnAddEntity(thisptr, ent_identity, handle);
}


__int64 hkOnRemoveEntity(__int64 thisptr, CEntityInstance* ent_identity, c_base_handle handle)
{
	if (!g_state.running || g_state.unloading)
		return oOnRemoveEntity(thisptr, ent_identity, handle);

	FEATURES::ENTITY::onRemoveEntity(handle, ent_identity);

	return oOnRemoveEntity(thisptr, ent_identity, handle);
}

void Hook_onAddEntity()
{
	uintptr_t addr = PatternScan("client.dll", ONADDENTITY_PATTERN);
	if (!addr)
	{
		return;
	}
	MH_CreateHook(reinterpret_cast<void*>(addr), &hkOnAddEntity, reinterpret_cast<void**>(&oOnAddEntity));
	MH_EnableHook(reinterpret_cast<void*>(addr));
}
void Hook_onRemoveEntity()
{
	uintptr_t addr = PatternScan("client.dll", ONREMOVEENTITY_PATTERN);
	if (!addr)
	{
		return;
	}
	MH_CreateHook(reinterpret_cast<void*>(addr), &hkOnRemoveEntity, reinterpret_cast<void**>(&oOnRemoveEntity));
	MH_EnableHook(reinterpret_cast<void*>(addr));
}