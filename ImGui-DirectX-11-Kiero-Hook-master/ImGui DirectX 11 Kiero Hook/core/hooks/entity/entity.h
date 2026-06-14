#pragma once
#include "../../../sdk/mesh_sdk.h"

using OnAddEntity_t = __int64(__fastcall*)(__int64, CEntityInstance*, c_base_handle);
using OnRemoveEntity_t = __int64(__fastcall*)(__int64, CEntityInstance*, c_base_handle);

inline OnAddEntity_t oOnAddEntity = nullptr;
inline OnRemoveEntity_t oOnRemoveEntity = nullptr;

void Hook_onAddEntity();
void Hook_onRemoveEntity();