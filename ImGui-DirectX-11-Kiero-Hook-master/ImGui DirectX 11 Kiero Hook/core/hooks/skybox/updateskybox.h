#pragma once

using UpdateSkyBox_t = void(__fastcall*)(
    __int64 a1,
    __int64 a2,
    __int64 a3,
    int   bDrawSkybox,
    int   a5,
    __int64 a6,
    __int64 a7
    );
inline UpdateSkyBox_t oUpdateSkyBox = nullptr;

void Hook_updateSkybox();