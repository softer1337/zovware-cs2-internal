#pragma once

using TonemapUpdate_t = float*(__fastcall*)(
    __int64,
    __int64,
    __int64
    );
inline TonemapUpdate_t oTonemapUpdate = nullptr;

void Hook_tonemapUpdate();