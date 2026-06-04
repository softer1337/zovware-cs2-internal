#pragma once
#include "../sdk/offsets.h"
#include <cstddef>
#include <Windows.h>
#include "hooks/pattern.h"
#include "../sdk/interface.h"

#define CLIENT_DLL "client.dll"

namespace MEM {

    inline uintptr_t client_base = 0;

    inline uintptr_t GetModule(const char* name) {
        HMODULE mod = GetModuleHandleA(name);
        return reinterpret_cast<uintptr_t>(mod);
    }

    inline void CacheModules() {
        client_base = reinterpret_cast<uintptr_t>(GetModuleHandleA(CLIENT_DLL));
    }

    inline uintptr_t GetClient() {
        if (!client_base) CacheModules();
        return client_base;
    }

    template<typename T>
    T read(uintptr_t addr) {
        return addr ? *reinterpret_cast<T*>(addr) : T{};
    }

    template<typename T>
    bool write(uintptr_t addr, const T& value) {
        if (!addr) return false;
        *reinterpret_cast<T*>(addr) = value;
        return true;
    }
    
    inline uintptr_t GetLocalPawn() {
        return MEM::read<uintptr_t>(MEM::GetClient() + offsets::dwLocalPlayerPawn);
    }

    inline bool HookVTable(void* instance, size_t index, void* new_func, void** original_out = nullptr) {
        if (!instance || !new_func) return false;

        uintptr_t* vtable = *reinterpret_cast<uintptr_t**>(instance);
        if (!vtable) return false;

        DWORD old_protect;
        VirtualProtect(&vtable[index], sizeof(uintptr_t), PAGE_EXECUTE_READWRITE, &old_protect);

        if (original_out) {
            *original_out = reinterpret_cast<void*>(vtable[index]);
        }

        vtable[index] = reinterpret_cast<uintptr_t>(new_func);

        VirtualProtect(&vtable[index], sizeof(uintptr_t), old_protect, &old_protect);
        return true;
    }

    template<typename T>
    T GetVFunc(void* instance, int index)
    {
        return reinterpret_cast<T>(
            (*reinterpret_cast<void***>(instance))[index]
            );
    }

    __forceinline uintptr_t GetEntityByHandle(uintptr_t entityList, int handle)
    {
        if (!entityList || !handle)
            return 0;

        int index = handle & 0x7FFF;

        uintptr_t listEntry = read<uintptr_t>(
            entityList + 0x8 * (index >> 9) + 0x10
        );

        if (!listEntry)
            return 0;

        return read<uintptr_t>(
            listEntry + 0x70 * (index & 0x1FF)
        );
    }
    inline uintptr_t GetEntityByIndex(uintptr_t entityList, int index) {
        if (!entityList) return 0;
        uintptr_t entry = MEM::read<uintptr_t>(entityList + (8 * (index & 0x7FFF) >> 9) + 16);
        if (!entry) return 0;
        return MEM::read<uintptr_t>(entry + 112 * (index & 0x1FF));
    }


    inline bool IsInGame() {
        if (Interface::GetIVEngineToClient()->IsInGame() && Interface::GetIVEngineToClient()->IsConnected())
            return true;
        return false;
        return true;
    }
    template <typename T = std::uint8_t>
    inline[[nodiscard]] T* GetAbsoluteAddress(T* pRelativeAddress, int nPreOffset = 0x0, int nPostOffset = 0x0)
    {
        pRelativeAddress += nPreOffset;
        pRelativeAddress += sizeof(std::int32_t) + *reinterpret_cast<std::int32_t*>(pRelativeAddress);
        pRelativeAddress += nPostOffset;
        return pRelativeAddress;
    }
    template<typename T, int Index, typename... Args>
    T CallVFunc(void* instance, Args... args)
    {
        using FuncType = T(__thiscall*)(void*, Args...);
        FuncType func = reinterpret_cast<FuncType>(
            (*reinterpret_cast<void***>(instance))[Index]
            );
        return func(instance, args...);
    }
    inline bool validPtr(const void* p) {
        return reinterpret_cast<std::uintptr_t>(p) >= 0x10000;
    }
    inline bool validPtr(std::uintptr_t p) {
        return p >= 0x10000;
    }
}