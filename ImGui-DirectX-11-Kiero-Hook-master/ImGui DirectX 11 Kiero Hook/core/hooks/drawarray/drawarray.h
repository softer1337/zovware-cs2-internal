#pragma once
#include "../../../sdk/mesh_sdk.h"
#include "../../../kiero/minhook/include/MinHook.h"
#include "../../../features/features.h"
#include "../../../state.hpp"

typedef bool(__fastcall* DrawArrayFn)(CSceneAnimatableObjectDesc*, void*, CMeshDrawPrimitive_t*, int, void*, void*, void*);
inline DrawArrayFn oDrawArray = nullptr;

void Hook_drawarray();