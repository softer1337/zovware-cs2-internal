#pragma once
#include "../core/mem.hpp"
#include "mesh_sdk.h"

enum e_particle_setting : unsigned int {
    particle_setting_position = 0,
    particle_setting_density = 2,
    particle_setting_info = 3,
    particle_setting_color = 16
};

class c_particle_snapshot {
public:
    void draw(int count, void* data) {
        MEM::CallVFunc<void, 1>(this, count, data);
    }
};

class c_particle_system_mgr {
public:
    void create_snapshot(CStrongHandle<c_particle_snapshot>* snapshot) {
        int64_t unknown = 0;
        MEM::CallVFunc<void, 41>(this, snapshot, &unknown);
    }
};

class c_create_particle {
public:
    void create_particle(CStrongHandle<c_particle_snapshot>* snapshot, uint64_t unknown) {
        MEM::CallVFunc<void, 42>(this, snapshot, &unknown);
    }
};

struct particle_color_t {
    float r;
    float g;
    float b;
};

struct particle_information_t {
    float m_time;
    float m_width;
    float m_alpha;
};

class particle_effect_t {
public:
    const char* m_name;
    char pad[0x30];
};

struct particle_data_t {
    Vec3* m_positions;
    char pad[0x74];
    float* m_times;
    char nigger[192];
};

struct particle_info_t {
    uint32_t m_effect_index;
    Vec3* m_positions;
    float* m_times;
    CStrongHandle<c_particle_snapshot> m_snapshot_handle;
    particle_data_t m_data;
};

class C_GameParticleManager {
public:
    bool init_effect(unsigned int effect_index, unsigned int unknown, CStrongHandle<c_particle_snapshot>* particle_snapshot) {
        using fn_init_effect = bool(__fastcall*)(C_GameParticleManager*, unsigned int, unsigned int, CStrongHandle<c_particle_snapshot>*);
        static fn_init_effect init_effect = reinterpret_cast<fn_init_effect>(PatternScan("client.dll", "48 89 74 24 10 57 48 83 EC 30 4C 8B D9 49 8B F9 33 C9 41 8B F0 83 FA FF 0F"));
        return init_effect(this, effect_index, unknown, particle_snapshot);
    }

    int* create_particle_effect(unsigned int* effect_index, const char* name) {
        using fn_create_particle_effect = int* (__fastcall*)(C_GameParticleManager*, unsigned int*, const char*, int, __int64, __int64, __int64, int);

        static uintptr_t addr = PatternScan("client.dll", "E8 ? ? ? ? 8B 08 89 8B ? ? ? ? E8 ? ? ? ? 40 80 FF");
        if (!addr) return nullptr;
        
        addr += *reinterpret_cast<int32_t*>(addr + 1);
        addr += 5;

        static fn_create_particle_effect create_particle_effect = reinterpret_cast<fn_create_particle_effect>(addr);

        return create_particle_effect(this, effect_index, name, 8, 0ll, 0ll, 0ll, 0);
    }

    bool set_particle_data(unsigned int effect_index, int unknown, void* data) {
        using fn_set_particle_data = bool(__fastcall*)(C_GameParticleManager*, unsigned int, int, void*, int);
        static uintptr_t addr = PatternScan("client.dll", "48 89 5C 24 ? 48 89 74 24 10 57 48 83 EC ? ? ? ? ? ? ? ? ? 41 8B F8 8B DA 4C");
        if (!addr) {
            printf("[Particles] set_particle_data pattern not found\n");
            return false;
        }
       

        fn_set_particle_data set_particle_data = reinterpret_cast<fn_set_particle_data>(addr);

        return set_particle_data(this, effect_index, unknown, data, 0);
    }

    void destroy_particle(int index, bool a1, bool a2) {
        using destroy_particle_t = void(__fastcall*)(C_GameParticleManager*, int, char, char);
        static const destroy_particle_t fn = reinterpret_cast<const destroy_particle_t>(PatternScan("client.dll", "83 FA ? 0F 84 ? ? ? ? 41 54"));
        return fn(this, index, a1, a2);
    }

    void release_particle_index(int index) {
        MEM::CallVFunc<void, 3>(this, index);
    }
};
