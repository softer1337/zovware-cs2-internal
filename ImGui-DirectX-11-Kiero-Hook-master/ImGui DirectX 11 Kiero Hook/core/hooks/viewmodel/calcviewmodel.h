#pragma once

using CalcViewmodel_t = void* (__fastcall*)(float* flUnk, float* flOffsets, float* flFov);
inline CalcViewmodel_t oCalcViewModel = nullptr;


void Hook_CalcViewModel();