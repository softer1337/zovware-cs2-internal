#pragma once
#include "../../../imgui/imgui.h"

class C_LightScene;


using UpdateLightScene_t = __int64(__fastcall*)(__int64 thisptr, C_LightScene* object, __int64 a3);
inline UpdateLightScene_t oUpdateLightScene_t = nullptr;

void Hook_updateLightScene();