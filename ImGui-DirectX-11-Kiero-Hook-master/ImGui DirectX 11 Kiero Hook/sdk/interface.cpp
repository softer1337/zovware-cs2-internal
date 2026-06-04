#include "interface.h"
#include "../core/mem.hpp"
#include "schema.h"
#include <Windows.h>
#include <cstdint>
#include "../core/hooks/pattern.h"
#include "trace/trace.h"
#include "mesh_sdk.h"

namespace IVEngineToClient_Search
{
    inline uintptr_t IsInGameFn = PatternScan("engine2.dll", "48 8B ? ? ? ? ? 48 85 C0 74 15 80 B8 ? ? ? ? ? 75 0C 83 B8 ? ? ? ? 06");
    inline uintptr_t GetLevelNameShortFn = PatternScan("engine2.dll", "48 83 EC 28 E8 ? ? ? ? 84 C0 74 0C 48 8D ? ? ? ? ? 48 83 C4 28 C3 48 8B ? ? ? ? ? 48 85 C9 74 23 83 B9 20 02 00 00 02 7C 1A 48 8B 89 08 02 00 00 48 8D ? ? ? ? ? 48 85 C9 48 0F 45 C1 48 83 C4 28 C3 48 8D ? ? ? ? ? 48 83 C4 28 C3");
    inline uintptr_t IsConnectedFn = PatternScan("engine2.dll", "48 8B 05 ? ? ? ? 48 85 C0 74 ? 83 B8 ? ? ? ? ? 0F 9D C0");
    /*inline CBasePattern GetLevelNameFn = { VmpStr("IVEngineToClient::GetLevelName") , VmpStr("48 83 EC 28 E8 ? ? ? ? 84 C0 74 0C 48 8D ? ? ? ? ? 48 83 C4 28 C3 48 8B ? ? ? ? ? 48 85 C9 74 23 83 B9 20 02 00 00 02 7C 1A 48 8B 89 00 02 00 00 48 8D ? ? ? ? ? 48 85 C9 48 0F 45 C1 48 83 C4 28 C3 48 8D ? ? ? ? ? 48 83 C4 28 C3") , ENGINE2_DLL };
    inline CBasePattern GetLevelNameShortFn = { VmpStr("IVEngineToClient::GetLevelNameShort") , VmpStr("48 83 EC 28 E8 ? ? ? ? 84 C0 74 0C 48 8D ? ? ? ? ? 48 83 C4 28 C3 48 8B ? ? ? ? ? 48 85 C9 74 23 83 B9 20 02 00 00 02 7C 1A 48 8B 89 08 02 00 00 48 8D ? ? ? ? ? 48 85 C9 48 0F 45 C1 48 83 C4 28 C3 48 8D ? ? ? ? ? 48 83 C4 28 C3") , ENGINE2_DLL };*/
}

bool IVEngineToClient::IsInGame()
{
    using Fn = bool(__fastcall*)(IVEngineToClient*);
    Fn Original = reinterpret_cast<Fn>(IVEngineToClient_Search::IsInGameFn);
    return Original(this);
}
bool IVEngineToClient::IsConnected()
{
    using Fn = bool(__fastcall*)(IVEngineToClient*);
    Fn Original = reinterpret_cast<Fn>(IVEngineToClient_Search::IsConnectedFn);
    return Original(this);
}
const char* IVEngineToClient::GetLevelNameShort()
{
    using Fn = const char* (__fastcall*)(IVEngineToClient*);
    Fn Original = reinterpret_cast<Fn>(IVEngineToClient_Search::GetLevelNameShortFn);
    return Original(this);
}

C_EconItemSystem* IEngine2Client::GetEconItemSystem()
{
	return MEM::CallVFunc<C_EconItemSystem*, 128>(this);
}

bool IFileSystem::exists(const char* file_name, const char* path_id)
{
    return MEM::CallVFunc<bool, 21>(this, file_name, path_id);
}
namespace Interface {

    static CreateInterfaceFn engineFactory = nullptr;
    static CreateInterfaceFn clientFactory = nullptr;
    static CreateInterfaceFn sceneFactory = nullptr;
    static CreateInterfaceFn resourceFactory = nullptr;
    static CreateInterfaceFn schemaFactory = nullptr;
    static CreateInterfaceFn inputFactory = nullptr;
	static CreateInterfaceFn fileFactory = nullptr;

    static IVEngineToClient* eng = nullptr;
    static Tracing* tracing = nullptr;
    static ISchemaSystem* schema = nullptr;
    static CResourceSystem* res = nullptr;
    static CSceneSystem* scene = nullptr;
    static InputSystem* inp = nullptr;
	static IEntitySystem* entity = nullptr;
	static IFileSystem* filesystem = nullptr;
    static IEngine2Client* eng2cl = nullptr;

    void Init()
    {
        engineFactory = CaptureFactory("engine2.dll");
        clientFactory = CaptureFactory("client.dll");
        sceneFactory = CaptureFactory("scenesystem.dll");
        schemaFactory = CaptureFactory("schemasystem.dll");
        resourceFactory = CaptureFactory("resourcesystem.dll");
		inputFactory = CaptureFactory("inputsystem.dll");
        fileFactory = CaptureFactory("filesystem_stdio.dll");

        schema = Get<ISchemaSystem>(schemaFactory, "SchemaSystem_001");
    }

    CreateInterfaceFn CaptureFactory(const char* module)
    {
        HMODULE mod = GetModuleHandleA(module);
        if (!mod) return nullptr;

        return reinterpret_cast<CreateInterfaceFn>(
            GetProcAddress(mod, "CreateInterface")
            );
    }

    void* GetPattern(const char* module, const char* pattern, int offset, int deref)
    {
        uintptr_t addr = PatternScan(module, pattern);
        if (!addr) return nullptr;

        uint8_t* ptr = reinterpret_cast<uint8_t*>(addr + offset);

        if (deref)
            ptr = *reinterpret_cast<uint8_t**>(ptr);

        return ptr;
    }
    IVEngineToClient* GetIVEngineToClient()
    {
        if (!eng) {
            eng = Get<IVEngineToClient>(engineFactory, "Source2EngineToClient001");
        }
        return eng;
    }
    Tracing* GetTraceManager()
    {
        if (!tracing) {
            uintptr_t addr = PatternScan(
                "client.dll",
                "48 8B 0D ? ? ? ? 48 8D 44 24 ? 48 89 74 24"
            );

            tracing = *reinterpret_cast<Tracing**>(
                MEM::GetAbsoluteAddress<uint8_t>(
                    reinterpret_cast<uint8_t*>(addr),
                    3,
                    0
                )
                );
        }
        return tracing;
    }
    ISchemaSystem* GetISchemaSystem()
    {
        if (!schema) {
            schema = Get<ISchemaSystem>(schemaFactory, "SchemaSystem_001");
        }
        return schema;
    }

    CResourceSystem* GetCResourceSystem()
    {
        if (!res) {
            res = Get<CResourceSystem>(resourceFactory, "ResourceSystem013");
        }
        return res;
    }
    CSceneSystem* GetCSceneSystem()
    {
        if (!scene) {
            scene = Get<CSceneSystem>(sceneFactory, "SceneSystem_002");
        }
        return scene;
    }
    InputSystem* GetInputSystem()
    {
        if (!inp) {
			inp = Get<InputSystem>(inputFactory, "InputSystemVersion001");
        }
		return inp;
    }
    IEntitySystem* GetEntitySystem()
    {
        if (!entity) {
            uintptr_t addr = PatternScan(
                "client.dll",
                "48 8B 0D ? ? ? ? 48 89 7C 24 ? 8B FA C1 EB"
            );
			printf("addr: %p\n", (void*)addr);
            entity = *reinterpret_cast<IEntitySystem**>(
                MEM::GetAbsoluteAddress<uint8_t>(
                    reinterpret_cast<uint8_t*>(addr),
                    0x3,
                    0
                )
                );
			printf("entity: %p\n", (void*)entity);
			printf("base: %p\n", (void*)MEM::GetClient());
            //entity = *reinterpret_cast<IEntitySystem**>(
            //    MEM::GetClient() + offsets::dwGameEntitySystem
            //    );

        }
        return entity;
    }

    IFileSystem* GetIFileSystem()
    {
        if (!filesystem) {
			filesystem = Get<IFileSystem>(fileFactory, "VFileSystem017");
        }
		return filesystem;
    }

    IEngine2Client* Interface::GetIEngine2Client()
    {
		if (!eng2cl) {
			eng2cl = Get<IEngine2Client>(clientFactory, "Source2Client002");
		}
        return eng2cl;
    }
}

