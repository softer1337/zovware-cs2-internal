#include "../../../features.h"


void FEATURES::VISUAL::WORLD::WORLDCOLOR::onDrawAggregateSceneObject(__int64 a1, __int64 a2, CAggregateObjectArray* a3)
{
    oDrawAggregateSceneObjectArray(a1, a2, a3);

    if (!a3 || !a3->data)
        return;

    if (!CFG::VISUAL::WORLD::isWorldModulationEnabled || !CFG::VISUAL::WORLD::isWorldEnabled)
        return;

    auto scene = Interface::GetCSceneSystem();
    if (!scene || !scene->pLightDataQueue || !scene->pLightDataQueue->pLightData)
        return;

    for (int i = 0; i < a3->data->count; i++)
    {
        int index = (a3->data->index + i) << 5;

        auto lightPtr = reinterpret_cast<Color_t*>(
            reinterpret_cast<uintptr_t>(scene->pLightDataQueue->pLightData) + index
            );

		Vec3 clr = CFG::VISUAL::WORLD::worldModulationColor;

        *lightPtr = Color_t(clr.x * 255, clr.y * 255, clr.z * 255, 255);
    }
}