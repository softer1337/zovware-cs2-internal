#pragma once



using MouseInputEnabled_t = bool(*)(void*);
inline MouseInputEnabled_t MouseInputEnabled_o = nullptr;

using IsRelativeMouseMode_t = bool(*)(void* a1, bool a2);
inline IsRelativeMouseMode_t IsRelativeMouseMode_o = nullptr;

void Hook_MouseInputEnabled();