#pragma once
#include "../../../sdk/mesh_sdk.h"

typedef __int64(__fastcall* GeneratePrimitivesFn)(
    void* __this,
    CSceneAnimatableObject* object,  
    void* a3,                        
    CMeshPrimitiveOutputBuffer* render_buf  
    );
inline GeneratePrimitivesFn oGeneratePrimitives = nullptr;
void Hook_generatePrimitives();