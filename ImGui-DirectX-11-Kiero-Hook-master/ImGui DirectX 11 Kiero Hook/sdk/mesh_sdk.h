#pragma once
#include <cstdint>
#include <D3D11.h>
#include "../core/mem.hpp"
#include <vector>
#include <string>
#include "math.hpp"
#include "schema.h" 
#include "../core/hooks/hooks.h"

using SetModel_t = __int64(__fastcall*)(uintptr_t, const char*);
inline SetModel_t oSetModel = (SetModel_t)PatternScan("client.dll", CHANGEMODEL_PATTERN);

class CMaterial2
{
public:
	char pad0[0x10];          // 0x00

	int m_nParameterCount;    // 0x10
	char pad1[0x4];           // 0x14

	void* m_pParameter;       // 0x18

	char pad2[0x3D8];         // 0x20

	void* pMaterialMode;      // 0x3F8

	const char* getName()
	{
		using Fn = const char* (__thiscall*)(CMaterial2*);
		return (*reinterpret_cast<Fn**>(this))[0](this);
	}
};


class Color_t
{
public:
	uint8_t r, g, b, a;

public:
	Color_t()
		: r(0), g(0), b(0), a(255) {
	}
	Color_t(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
		: r(red), g(green), b(blue), a(alpha) {
	}
	Color_t(const float* col)
		: r((uint8_t)(col[0] * 255.f)), g((uint8_t)(col[1] * 255.f)), b((uint8_t)(col[2] * 255.f)), a((uint8_t)(col[3] * 255.f)) {
	}
	Color_t(const Color_t& col)
		: r(col.r), g(col.g), b(col.b), a(col.a) {
	}
};
inline Color_t ImColorToColor(const ImVec4& c)
{
	return Color_t(
		static_cast<uint8_t>(c.x * 255.f),
		static_cast<uint8_t>(c.y * 255.f),
		static_cast<uint8_t>(c.z * 255.f),
		static_cast<uint8_t>(c.w * 255.f)
	);
}
struct CSkyBoxSceneObject
{
	char  pad1[0xE8];

	Vec3  skyColor;      // 0x100

	float intensity;     // 0x108

	int   skyType;       // 0x134
};



#define INVALID_EHANDLE_INDEX 0xFFFFFFFF
#define ENT_ENTRY_MASK 0x7FFF
#define NUM_SERIAL_NUM_SHIFT_BITS 15

class CEntityInstance;

template <typename T = CEntityInstance>
class CHandle
{
public:
	uint32_t nIndex;

public:
	inline CHandle() :
		nIndex(INVALID_EHANDLE_INDEX)
	{
	}

	inline CHandle(const int nEntry, const int nSerial)
		: nIndex(nEntry | (nSerial << NUM_SERIAL_NUM_SHIFT_BITS))
	{
	}

	template <typename U>
	inline CHandle(const CHandle<U>& other)
	{
		nIndex = other.nIndex;
	}

	template <typename U>
	inline CHandle<T> operator=(const CHandle<U> other)
	{
		nIndex = other.nIndex;
		return *this;
	}

	inline CHandle<T> operator=(const int other)
	{
		nIndex = other;
		return *this;
	}

	inline operator bool() const
	{
		return isValid();
	}

	template <typename U>
	inline bool operator!=(const CHandle<U> other) const
	{
		return nIndex != other.nIndex;
	}

	template <typename U>
	inline bool operator==(const CHandle<U> other) const
	{
		return nIndex == other.nIndex;
	}

	template <typename U>
	inline bool operator<(const CHandle<U> other) const
	{
		return nIndex < other.nIndex;
	}

	inline bool operator!() const
	{
		return !isValid();
	}

	inline bool isValid() const
	{
		return nIndex != INVALID_EHANDLE_INDEX;
	}

	inline int getEntryIndex() const
	{
		return static_cast<int>(nIndex & ENT_ENTRY_MASK);
	}

	inline int getSerialNumber() const
	{
		return static_cast<int>(nIndex >> NUM_SERIAL_NUM_SHIFT_BITS);
	}

private:
	inline CEntityInstance* getEntity(int nEntryIndex) const
	{

		return (CEntityInstance*)MEM::GetEntityByIndex(MEM::read<uintptr_t>(MEM::GetClient() + offsets::dwEntityList), nEntryIndex);
	}


public:
	template <typename U = T>
	inline U* get() const
	{
		return reinterpret_cast<U*>(getEntity(getEntryIndex()));
	}
};

class CSkeletonInstance;
class CMeshInstance_Imp;
class CSceneAnimatableObjectDesc;
class CSchemaClassInfo;




class C_GameSceneNode
{
public:
	void set_mesh_group_mask(uint64_t mask) {
		using fn_t = void(__fastcall*)(void*, uint64_t);
		static auto fn = reinterpret_cast<fn_t>(
			PatternScan("client.dll",
				"48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8D 99 ? ? ? ? 48 8B 71")
			);
		if (fn) {
			fn(this, mask);
		}
	}
	SCHEMA(m_child, C_GameSceneNode*, "CGameSceneNode", "m_pChild");
	SCHEMA(m_next_sibling, C_GameSceneNode*, "CGameSceneNode", "m_pNextSibling");
	SCHEMA(m_owner, CEntityInstance*, "CGameSceneNode", "m_pOwner");
};
class c_base_handle;
class C_BaseEntity
{
public:


	CSchemaClassInfo* getSchemaClassInfo()
	{
		CSchemaClassInfo* ret = nullptr;
		MEM::CallVFunc<void, 42U>(this, &ret);

		return ret;
	}
	
	SCHEMA(m_nSubclassID, uint32_t, "C_BaseEntity", "m_nSubclassID");
	SCHEMA(m_scene_node, C_GameSceneNode*, "C_BaseEntity", "m_pGameSceneNode");
	SCHEMA(m_owner_entity, c_base_handle, "C_BaseEntity", "m_hOwnerEntity");

	void SetModel(const char* modelName)
	{
		if (oSetModel)
			oSetModel((uintptr_t)this, modelName);
	}

};
struct UnkData_t {
	char pad_0[0x98];
	const char* m_panelName;
};

struct CSceneAnimatableObject {
	char pad_0[0xB8];
	UnkData_t* m_Unk;
	uintptr_t hOwner;
	char pad_1[0x4C];
};


enum MaterialVarFlags_t
{
	MATERIAL_VAR_DEBUG = (1 << 0),
	MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
	MATERIAL_VAR_NO_DRAW = (1 << 2),
	MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),

	MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
	MATERIAL_VAR_VERTEXALPHA = (1 << 5),
	MATERIAL_VAR_SELFILLUM = (1 << 6),
	MATERIAL_VAR_ADDITIVE = (1 << 7),
	MATERIAL_VAR_ALPHATEST = (1 << 8),
	MATERIAL_VAR_ZNEARER = (1 << 10),
	MATERIAL_VAR_MODEL = (1 << 11),
	MATERIAL_VAR_FLAT = (1 << 12),
	MATERIAL_VAR_NOCULL = (1 << 13),
	MATERIAL_VAR_NOFOG = (1 << 14),
	MATERIAL_VAR_IGNOREZ = (1 << 15),
	MATERIAL_VAR_DECAL = (1 << 16),
	MATERIAL_VAR_ENVMAPSPHERE = (1 << 17), // OBSOLETE
	//	MATERIAL_VAR_UNUSED					  = (1 << 18),
	MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19), // OBSOLETE
	MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
	MATERIAL_VAR_TRANSLUCENT = (1 << 21),
	MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
	MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23), // OBSOLETE
	MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
	MATERIAL_VAR_MULTIPLY = (1 << 25),
	MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
	MATERIAL_VAR_HALFLAMBERT = (1 << 27),
	MATERIAL_VAR_WIREFRAME = (1 << 28),
	MATERIAL_VAR_ALLOWALPHATOCOVERAGE = (1 << 29),
	MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY = (1 << 30),
	MATERIAL_VAR_VERTEXFOG = (1 << 31),
};

class CMeshDrawPrimitive_t
{
public:
	void* pData;                             // 0x00

	char pad0[0x10];                         // 0x08

	CSceneAnimatableObject* pSceneAnimatableObject; // 0x18

	CMaterial2* pMaterial;                   // 0x20
	CMaterial2* pMaterial2;                  // 0x28

	char pad1[0x10];                         // 0x30

	void* some_data;                         // 0x40

	char pad2[0x8];                          // 0x48

	Color_t colValue;                        // 0x50

	char pad3[0x14];                         // 0x54
}; // sizeof = 0x68
struct ResourceBinding_t
{
	void* pData;           // 0x0
	const char** szName;   // 0x8
	char pad0[0x8];        // 0x10
	int nRefCount;          // 0x18
	char pad1[0x14];       // 0x1C
}; // size: 0x30

template <typename T>
class CStrongHandle
{
public:
	CStrongHandle() : pBinding(nullptr) {}

	operator T* () const
	{
		return pBinding ? static_cast<T*>(pBinding->pData) : nullptr;
	}

	T* operator->() const
	{
		return pBinding ? static_cast<T*>(pBinding->pData) : nullptr;
	}

	ResourceBinding_t* pBinding = nullptr;
};

class CTextureDx11
{
public:
	char pad0[0x10];                  // 0x0
	ID3D11ShaderResourceView* m_pTextureSRV0; // 0x10
	ID3D11ShaderResourceView* m_pTextureSRV1; // 0x18
};


struct LoadedTexture
{
	CStrongHandle<CTextureDx11> tex;
	bool need_destroy = false;
};

struct LoadedMaterial
{
	CStrongHandle<CMaterial2> mat;
	std::vector<LoadedTexture> loaded_textures;
};
class CUtlBuffer
{
public:
	inline CUtlBuffer(int a1, int nSize, int a3)
	{
		using InitFn = void(__fastcall*)(CUtlBuffer*, int, int, int);
		auto hTier0 = GetModuleHandleA("tier0.dll");
		if (!hTier0) return;

		auto oInit = reinterpret_cast<InitFn>(
			GetProcAddress(hTier0, "??0CUtlBuffer@@QEAA@HHW4BufferFlags_t@0@@Z")
			);
		if (!oInit) return;

		oInit(this, a1, nSize, a3);
	}

	inline CUtlBuffer(char* data, int nSize, int flag)
	{
		using Init2Fn = void(__fastcall*)(CUtlBuffer*, char*, int, int);
		auto hTier0 = GetModuleHandleA("tier0.dll");
		if (!hTier0) return;

		auto oInit2 = reinterpret_cast<Init2Fn>(
			GetProcAddress(hTier0, "??0CUtlBuffer@@QEAA@PEBXHW4BufferFlags_t@0@@Z")
			);
		if (!oInit2) return;

		oInit2(this, data, nSize, flag);
	}


	inline void putString(const char* szString)
	{
		using PutStringFn = void(__fastcall*)(CUtlBuffer*, const char*);

		HMODULE hTier0 = GetModuleHandleA("tier0.dll");
		if (!hTier0)
			return;

		auto oUtlBufferPutString = reinterpret_cast<PutStringFn>(
			GetProcAddress(hTier0, "?PutString@CUtlBuffer@@QEAAXPEBD@Z")
			);

		if (!oUtlBufferPutString)
			return;

		oUtlBufferPutString(this, szString);
	}


	//inline void ensureCapacity(int nSize)
	//{
	//	static auto oUtlBufferEnsureCapacity = functions::get<
	//		void(__fastcall*)(CUtlBuffer*, int)>
	//		("CUtlBuffer::EnsureCapacity");

	//	oUtlBufferEnsureCapacity(this, nSize);
	//}

private:
	char pad[0x80]; // 128 байт вместо MEM_PAD
};
struct KV3ID_t
{
	const char* szName;
	uint64_t unk0;
	uint64_t unk1;
};
class CKeyValues3
{
public:
	char pad[0x170]; // 368 байт

	bool loadFromBuffer(const char* szString)
	{
		CUtlBuffer buffer(0, static_cast<int>(strlen(szString)) + 10, 1);
		buffer.putString(szString);

		return loadKV3(&buffer);
	}

	bool loadKV3(CUtlBuffer* buffer)
	{
		using LoadKV3Fn = bool(__fastcall*)(CKeyValues3*, void*, CUtlBuffer*, KV3ID_t*, void*, void*, void*, void*, const char*, int);

		
		static auto oLoadKeyValues = []() -> LoadKV3Fn {
			uintptr_t addr = PatternScan("tier0.dll", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 54 41 56 41 57 48 83 EC ? 45 33 E4");
			if (!addr) {
				MessageBoxA(NULL, "LoadKV3 pattern not found in tier0.dll", "KV3 Error", MB_OK);
				return nullptr;
			}
			return reinterpret_cast<LoadKV3Fn>(addr);
			}();

		if (!oLoadKeyValues) return false;

		KV3ID_t kv3ID = { "generic", 0x41B818518343427E, 0xB5F447C23C0CDF8C };
		return oLoadKeyValues(this, nullptr, buffer, &kv3ID, nullptr, nullptr, nullptr, nullptr, "", 0);
	}


	bool create()
	{
		using SetTypeKV3Fn = CKeyValues3 * (__fastcall*)(CKeyValues3*, uint32_t, uint32_t);

		static SetTypeKV3Fn oSetTypeKV3 = []() -> SetTypeKV3Fn
			{
				auto callAddr = PatternScan(
					"client.dll",
					"40 53 48 83 EC ? ? ? ? 41 B9 ? ? ? ? 49 83 CA"
				);

				if (!callAddr)
				{
					
					return nullptr;
				}

				// rel32 resolve
				/*int32_t rel = *reinterpret_cast<int32_t*>(callAddr + 1);
				auto funcAddr = callAddr + 5 + rel;*/

				return reinterpret_cast<SetTypeKV3Fn>(callAddr);
			}();

		if (!oSetTypeKV3)
			return false;

		return oSetTypeKV3(this, 1u, 6u) != nullptr;
	}

};


class CVSoundTypeManager; // forward declaration
#pragma once
#include <Windows.h>
#include <cstdint>

class CBufferString
{
private:
	std::uint32_t m_nUnknown1 = 0;
	std::uint32_t m_nUnknown2 = 0xC00000C8;

	union
	{
		std::uintptr_t m_uStrPtr;
		std::uint8_t m_szBuffer[0xC8];
	};

	std::uint64_t m_nUnknown3 = 0;
	std::uint64_t m_nUnknown4 = 0;

public:
	using InitFn = void(__fastcall*)(CBufferString*, const char*);
	using PurgeFn = void(__fastcall*)(CBufferString*, unsigned int);
	using LoadPathFn = void(__fastcall*)(CBufferString*, const char*);
	using InsertFn = void(__fastcall*)(CBufferString*, int, const char*, int, bool);

	static inline InitFn     fnInit = nullptr;
	static inline PurgeFn    fnPurge = nullptr;
	static inline LoadPathFn fnLoadPath = nullptr;
	static inline InsertFn   fnInsert = (InsertFn)GetProcAddress(GetModuleHandleA("tier0.dll"), ("?Insert@CBufferString@@QEAAPEBDHPEBDH_N@Z"));

public:
	CBufferString() = default;

	CBufferString(const char* string, uint64_t tag) {
		reinterpret_cast<bool(*)(CBufferString*, const char*)>(PatternScan("client.dll", "48 89 5C 24 ? 57 48 83 EC ? 8B 41 ? 48 8D 79"))(this, string);
		m_nUnknown4 = tag;
	}

	~CBufferString() {
		reinterpret_cast<void(*)(CBufferString*, uint32_t)>(GetProcAddress(GetModuleHandleA("tier0.dll"), ("?Purge@CBufferString@@QEAAXH@Z")))(this, 0);
	}

	void Insert(int index, const char* str, int length, bool b1) {
		if (fnInsert) fnInsert(this, index, str, length, b1);
	}

	void LoadPath(const char* szPath)
	{
		if (fnLoadPath)
			fnLoadPath(this, szPath);
	}
};
class CResourceSystem
{
public:
	/*void* QueryInterface(const char* szInterfaceName) const
	{
		return CallVFunc<void*, 2U>(this, szInterfaceName);
	}

	uint32_t ResourceLoaded(CBufferString* buffer)
	{
		return mem::CallVFunc<uint32_t, 55>(this, buffer);
	}

	ResourceBinding_t* getResource(CBufferString* buffer, uint64_t unk) const
	{
		return mem::CallVFunc<ResourceBinding_t*, 80>(this, buffer, unk);
	}

	ResourceBinding_t* loadResource(CBufferString* buffer, const char* load_function = "")
	{
		return mem::CallVFunc<ResourceBinding_t*, 45>(this, buffer, load_function);
	}*/

	void preCache(const char* buffer)
	{
		CBufferString names;
		names.Insert(0, buffer, -1, false);
		return MEM::CallVFunc<void, 40>(this, &names, "");
	}

	//ResourceBinding_t* getOrLoad(CBufferString* buffer)
	//{
	//	int handle = ResourceLoaded(buffer);
	//	if (handle)
	//		return getResource(buffer, 0LL);

	//	return loadResource(buffer);
	//}

	ResourceBinding_t* load_vtex(const char* name) {
		constexpr uint64_t value = 0x78746576;
		auto names = CBufferString(name, value);
		return MEM::CallVFunc<ResourceBinding_t*, 47>(this, & names, "");
	}

	ResourceBinding_t* BlockingLoadResourceByName(const char* szName) {
		return MEM::CallVFunc<ResourceBinding_t*, 40>(this, szName, "");
	}

	ResourceBinding_t* FindOrRegisterResourceByName(const char* szName) {
		return MEM::CallVFunc<ResourceBinding_t*, 75>(this, szName, 0LL);
	}

	static CStrongHandle<CMaterial2> createMaterial(const char* szMaterialName, const char szVmatBuffer[])
	{
		// Паттерн для функции CreateMaterial в materialsystem2.dll
		uintptr_t addr = PatternScan(
			"materialsystem2.dll",
			"48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 8B F2"
		);

		if (!addr)
		{
			MessageBoxA(NULL, "Failed to pattern scan CreateMaterial in materialsystem2.dll", "CreateMaterial Error", MB_OK | MB_ICONERROR);
			return {};
		}

		// Приводим к типу функции
		using CreateMaterialFn = void* (__fastcall*)(void*, void*, const char*, void*, unsigned int, unsigned int);
		auto oCreateMaterial = reinterpret_cast<CreateMaterialFn>(addr);
		if (!oCreateMaterial)
		{
			MessageBoxA(NULL, "oCreateMaterial is null after cast", "CreateMaterial Error", MB_OK | MB_ICONERROR);
			return {};
		}

		// Создаем KeyValues
		CKeyValues3 key_values;
		if (!key_values.create())
		{
			MessageBoxA(NULL, "CKeyValues3::create() failed", "CreateMaterial Error", MB_OK | MB_ICONERROR);
			return {};
		}

		if (!key_values.loadFromBuffer(szVmatBuffer))
		{
			MessageBoxA(NULL, "CKeyValues3::loadFromBuffer() failed", "CreateMaterial Error", MB_OK | MB_ICONERROR);
			return {};
		}

		// Выделяем CStrongHandle
		CStrongHandle<CMaterial2> pCustomMaterial = {};

		void* ret = oCreateMaterial(nullptr, &pCustomMaterial, szMaterialName, &key_values, 0, 1);
		if (!ret)
		{
			MessageBoxA(NULL, "oCreateMaterial returned nullptr", "CreateMaterial Error", MB_OK | MB_ICONERROR);
			return {};
		}

		return pCustomMaterial;
	}
};



class CAggregateSceneObjectData {
public:
	char pad0[0x38];   // 0x00
	Color_t m_rgba;    // 0x38
	char pad1[0x08];   // 0x48
}; // size: 0x50

class CAggregateSceneObject {
public:
	char pad0[0x120]; // 0x00
	int count;        // 0x120
	char pad1[0x04];  // 0x124
	CAggregateSceneObjectData* data; // 0x128
}; // size: 0x130

class CMeshDrawPrimitive_t2 {
public:
	char pad0[0x18];               // 0x00
	CAggregateSceneObject* m_pObject; // 0x18
	CMaterial2* m_pMaterial;       // 0x20
	char pad1[0x28];               // 0x28
	Color_t m_rgba;                // 0x50
}; // size: 0x60
class CMeshPrimitiveOutputBuffer
{
public:
	CMeshDrawPrimitive_t* m_out;
	int m_max_output_primitives;
	int m_start_primitive;
};
const std::string key_values_sig = R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
)";

inline CStrongHandle<CMaterial2> compile(const std::string& mat_name, const std::string& mat_contents)
{
	return CResourceSystem::createMaterial(
		mat_name.c_str(),
		(key_values_sig + mat_contents + "\n}").c_str()
	);
}

template <typename T>
class CNetworkUtlVector {
public:
	unsigned int m_size;
	T* m_elements;
};
