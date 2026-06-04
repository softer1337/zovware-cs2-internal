#pragma once
#include "../core/mem.hpp"

#include <unordered_map>
#include <cstdint>

constexpr uint32_t val_32_const = 0x811c9dc5;
constexpr uint32_t prime_32_const = 0x1000193;
constexpr uint64_t val_64_const = 0xcbf29ce484222325;
constexpr uint64_t prime_64_const = 0x100000001b3;

inline constexpr uint32_t hash_32(const char* const str, const uint32_t value = val_32_const) noexcept {
    return (str[0] == '\0') ? value : hash_32(&str[1], (value ^ uint32_t((uint8_t)str[0])) * prime_32_const);
}

inline constexpr uint64_t hash_64(const char* const str, const uint64_t value = val_64_const) noexcept {
    return (str[0] == '\0') ? value : hash_64(&str[1], (value ^ uint64_t((uint8_t)str[0])) * prime_64_const);
}

#define HASH64(str) hash_64(str)

// ------------------------------------------------------------------
// Типы данных SchemaSystem
// ------------------------------------------------------------------
using SchemaKeyValueMap_t = std::unordered_map<unsigned long long, short>;
using SchemaTableMap_t = std::unordered_map<unsigned long long, SchemaKeyValueMap_t>;

struct CSchemaClassField {
    const char* m_szName;
    char pad_0[0x8];
    int m_nOffset;
    char pad_1[0xC];
};

class CSchemaClassInfo {
public:
    const char* GetName() const {
        return *reinterpret_cast<const char* const*>(reinterpret_cast<uintptr_t>(this) + 0x8);
    }

    uint16_t GetFieldsSize() {
        return *reinterpret_cast<uint16_t*>(reinterpret_cast<uintptr_t>(this) + 0x24);
    }

    CSchemaClassField* GetFields() {
        return *reinterpret_cast<CSchemaClassField**>(reinterpret_cast<uintptr_t>(this) + 0x30);
    }
};

class CSchemaTypeScope {
public:
    CSchemaClassInfo* FindDeclaredClass(const char* className) {
        CSchemaClassInfo* rv = nullptr;
        MEM::CallVFunc<void, 2>(this, &rv, className);
        return rv;
    }

    void* GetClassBindTable() const {
        return *reinterpret_cast<void* const*>(reinterpret_cast<uintptr_t>(this) + 0x5C0);
    }
};

class ISchemaSystem {
public:
    CSchemaTypeScope* FindTypeScopeForModule(const char* moduleName) {
        return MEM::CallVFunc<CSchemaTypeScope*, 13>(this, moduleName, nullptr);
    }
};


// ------------------------------------------------------------------
// Утилиты SchemaSystem
// ------------------------------------------------------------------
namespace SchemaSystem {
    inline bool InitSchemaFieldsForClass(SchemaTableMap_t& tableMap, const char* className, uint64_t classKey) {
        CSchemaTypeScope* scope = Interface::GetISchemaSystem()->FindTypeScopeForModule("client.dll");
        if (!scope) return false;

        CSchemaClassInfo* info = scope->FindDeclaredClass(className);
        if (!info) return false;

        uint16_t fieldsSize = info->GetFieldsSize();
        CSchemaClassField* fields = info->GetFields();
        if (!fields) return false;

        auto& kvm = tableMap[classKey];
        for (uint16_t i = 0; i < fieldsSize; ++i) {
            CSchemaClassField& field = fields[i];
            if (field.m_szName && field.m_szName[0]) {
                kvm.emplace(HASH64(field.m_szName), field.m_nOffset);
            }
        }
        // Исправлено: убран лишний emplace, который дублировал запись
        return true;
    }

    inline int16_t GetOffset(const char* className, const char* keyName) {
        uint64_t classNameKey = HASH64(className);
        uint64_t keyNameKey = HASH64(keyName);

        static SchemaTableMap_t schemaTableMap;

        auto it = schemaTableMap.find(classNameKey);
        if (it == schemaTableMap.end()) {
            if (InitSchemaFieldsForClass(schemaTableMap, className, classNameKey)) {
                return GetOffset(className, keyName);  // рекурсивный вызов после инициализации
            }
            return 0;
        }

        const SchemaKeyValueMap_t& tableMap = it->second;
        auto keyIt = tableMap.find(keyNameKey);
        if (keyIt == tableMap.end())
            return 0;

        return keyIt->second;
    }
}


#define SCHEMA(varName, type, className, keyName) \
	type& varName() { \
		static const short offset = SchemaSystem::GetOffset(className, keyName); \
		return *reinterpret_cast<type*>(reinterpret_cast<unsigned __int64>(this) + offset); \
	}

#define SCHEMA_ARRAY(varName, type, className, keyName) \
	type* varName() { \
		static const short offset = SchemaSystem::GetOffset(className, keyName); \
		return reinterpret_cast<type*>(reinterpret_cast<unsigned __int64>(this) + offset); \
	}

#define SCHEMA_WITH_OFFSET(varName, type, className, keyName, offset2) \
	type& varName() { \
		static const short offset = SchemaSystem::GetOffset(className, keyName); \
		return *reinterpret_cast<type*>(reinterpret_cast<unsigned __int64>(this) + (offset + offset2)); \
	}

#define OFFSET(type, name, offset) \
    __forceinline std::add_lvalue_reference_t<type> name() const { \
        return *reinterpret_cast<type*>(reinterpret_cast<std::uintptr_t>(this) + offset); \
    }


