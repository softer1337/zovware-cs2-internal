#pragma once
#include "../../../sdk/math.hpp"


using ProcessSmoke_t = void(__fastcall*)(__int64 a1);

inline ProcessSmoke_t oProcessSmoke = nullptr;

void Hook_ProcessSmoke();