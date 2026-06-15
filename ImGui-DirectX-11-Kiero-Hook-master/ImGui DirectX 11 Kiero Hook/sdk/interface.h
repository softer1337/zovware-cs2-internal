#pragma once


class CLightDataQueue
{
public:
    char pad_0000[0x18];
    void* pLightData;
    char pad_0020[0x4];
};

class CSceneSystem
{
public:
    char pad_0000[0x2A28];
    CLightDataQueue* pLightDataQueue;
};







using CreateInterfaceFn = void* (*)(const char*, int*);
class Tracing;
class ISchemaSystem;
class InputSystem;
class CResourceSystem;
class CEngineConvar;
class IEntitySystem;
class c_network_channel;
class CNetworkGameClientServices;
class C_EconItemSystem;
class C_GlobalVariables;
class C_GameParticleManager;
class IEngine2Client {
public:
    C_EconItemSystem* GetEconItemSystem();
};

class IFileSystem {
public:
    bool exists(const char* file_name, const char* path_id);
};
class IVEngineToClient
{
public:
    bool IsInGame();
    bool IsConnected();
    const char* GetLevelNameShort();
    c_network_channel* GetNetChannelInfo(int split_slot);
};

namespace Interface {

    void Init();

    CreateInterfaceFn CaptureFactory(const char* module);

    template<typename T>
    T* Get(CreateInterfaceFn factory, const char* name)
    {
        if (!factory) return nullptr;
        return reinterpret_cast<T*>(factory(name, nullptr));
    }

    void* GetPattern(const char* module, const char* pattern, int offset = 0, int deref = 0);
    IVEngineToClient* GetIVEngineToClient();
    Tracing* GetTraceManager();
    CResourceSystem* GetCResourceSystem();
    CSceneSystem* GetCSceneSystem();
    ISchemaSystem* GetISchemaSystem();
	InputSystem* GetInputSystem();
	IEntitySystem* GetEntitySystem();
	IFileSystem* GetIFileSystem();
	IEngine2Client* GetIEngine2Client();
    C_GlobalVariables* GetGlobalVars();
    C_GameParticleManager* GetGameParticleManager();
    CEngineConvar* GetConVars();
    CNetworkGameClientServices* GetNetworkClientServices();
}