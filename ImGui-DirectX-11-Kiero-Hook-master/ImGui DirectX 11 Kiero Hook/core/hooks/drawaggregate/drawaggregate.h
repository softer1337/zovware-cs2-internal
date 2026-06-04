#pragma once



class CAggregateObjectData
{
public:
    char pad_0000[0x4]; //0x0000
    int count; //0x0004
    char pad_0008[0x28]; //0x0008
    int index; //0x0030
}; //Size: 0x0034

class CAggregateObjectArray
{
public:
    void* object; //0x0000
    CAggregateObjectData* data; //0x0008
};

using DrawAggregateSceneObjectArray_t = __int64(__fastcall*)(__int64 a1, __int64 a2, CAggregateObjectArray* a3);
inline DrawAggregateSceneObjectArray_t oDrawAggregateSceneObjectArray = nullptr;


void Hook_drawAggregateSceneObject();