#include "Pattern.h"
#include <Windows.h>
#include <Psapi.h>
#include <cstdint>
#include <vector>
#include <string>


uintptr_t PatternScan(const char* moduleName, const char* pattern) {
    HMODULE module = GetModuleHandleA(moduleName);
    if (!module) return 0;

    MODULEINFO moduleInfo;
    GetModuleInformation(GetCurrentProcess(), module, &moduleInfo, sizeof(moduleInfo));

    uintptr_t start = reinterpret_cast<uintptr_t>(module);
    uintptr_t end = start + moduleInfo.SizeOfImage;

    // Парсим строку в байты и маску
    std::vector<BYTE> bytes;
    std::vector<bool> mask;

    const char* p = pattern;
    while (*p) {
        if (*p == ' ') { p++; continue; }
        if (*p == '?') {
            bytes.push_back(0);
            mask.push_back(false);
            p++;
            if (*p == '?') p++;
        }
        else {
            char hex[3] = { p[0], p[1], 0 };
            bytes.push_back(static_cast<BYTE>(strtoul(hex, nullptr, 16)));
            mask.push_back(true);
            p += 2;
        }
    }

    for (uintptr_t i = start; i < end - bytes.size(); i++) {
        bool found = true;
        for (size_t j = 0; j < bytes.size(); j++) {
            if (mask[j] && bytes[j] != *reinterpret_cast<BYTE*>(i + j)) {
                found = false;
                break;
            }
        }
        if (found) return i;
    }
    return 0;
}