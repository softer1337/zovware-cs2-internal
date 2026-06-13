#include "mouseinp.h"
#include "../../../menu/menu.h"
#include "../pattern.h"
#include "../../../kiero/minhook/include/MinHook.h"
#include "../hooks.h"

bool hkMouseInputEnabled(void* RCX)
{
	if (MENU::menu_open)
		return false; 
	return MouseInputEnabled_o(RCX);
}

bool hkIsRelativeMouseMode(void* a1, bool a2)
{
	MENU::mainActive = a2;

	if (MENU::menu_open)
		return IsRelativeMouseMode_o(a1, false);

	return IsRelativeMouseMode_o(a1, a2);
}

void Hook_MouseInputEnabled()
{
	uintptr_t addr = PatternScan("client.dll", "40 53 48 83 EC ? 80 B9 ? ? ? ? ? 48 8B D9 75 ? 48 8B 0D ? ? ? ? 48 8B 01");

	uintptr_t addr2 = PatternScan("inputsystem.dll", "48 89 6C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 83 EC ? 0F B6 F2");

	if (addr && MH_CreateHook(reinterpret_cast<void*>(addr),
		hkMouseInputEnabled,
		reinterpret_cast<void**>(&MouseInputEnabled_o)) == MH_OK)
	{
		MH_EnableHook(reinterpret_cast<void*>(addr));
	}

	if (addr2 && MH_CreateHook(reinterpret_cast<void*>(addr2),
		hkIsRelativeMouseMode,
		reinterpret_cast<void**>(&IsRelativeMouseMode_o)) == MH_OK)
	{
		MH_EnableHook(reinterpret_cast<void*>(addr2));
	}
	else {
		MessageBoxA(0, "test", "debug", 0);
	}
}