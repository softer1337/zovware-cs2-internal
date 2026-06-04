#include "../../../features.h"
#include "../../../../sdk/mesh_sdk.h"
#include "../../../../core/hooks/skybox/updateskybox.h"


void FEATURES::VISUAL::WORLD::SKYBOX::onUpdateSkyBox(__int64 pObjectDesc, __int64 a2, __int64 pSceneData, int iDataCount, int a5, __int64 a6, __int64 a7)
{
    if (iDataCount > 0)
    {
        uintptr_t skybox_data_addr = 0x68 * iDataCount + pSceneData - 0x50;
        auto SkyBoxData = *(CSkyBoxSceneObject**)(skybox_data_addr);
        if (SkyBoxData)
        {
            if (CFG::VISUAL::WORLD::isSkyboxEnabled && CFG::VISUAL::WORLD::isWorldEnabled)
                SkyBoxData->skyColor = CFG::VISUAL::WORLD::skyboxColor;
            else {
                SkyBoxData->skyColor = { 1.f, 1.f, 1.f };
            }
        }
    }

    oUpdateSkyBox(pObjectDesc, a2, pSceneData, iDataCount, a5, a6, a7);
}