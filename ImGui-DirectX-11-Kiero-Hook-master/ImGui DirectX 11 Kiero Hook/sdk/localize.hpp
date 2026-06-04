#pragma once
#include "Windows.h"
#include "../core/mem.hpp"

class i_localize {
public:
	const char* find_safe(const char* token) {
		if (!token)
			return "";
		const auto* result = MEM::CallVFunc<const char*, 18>(this, token);
		return result ? result : token;
	}
};


namespace {
	using create_interface_fn_t = void* (*)(const char* name, int* return_code);

	i_localize* resolve() {
		static i_localize* cached = nullptr;
		if (cached)
			return cached;

		HMODULE dll = GetModuleHandleA("localize.dll");
		if (!dll)
			return nullptr;

		const auto factory = reinterpret_cast<create_interface_fn_t>(
			GetProcAddress(dll, "CreateInterface"));
		if (!factory)
			return nullptr;

		cached = static_cast<i_localize*>(factory("Localize_001", nullptr));
		return cached;
	}
}

namespace localize {
	const char* find_safe(const char* token) {
		if (!token || !*token)
			return token ? token : "";
		auto* loc = resolve();
		if (!loc)
			return token;
		return loc->find_safe(token);
	}
}
