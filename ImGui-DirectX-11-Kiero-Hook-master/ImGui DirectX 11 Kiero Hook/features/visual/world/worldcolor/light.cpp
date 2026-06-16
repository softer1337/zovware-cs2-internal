#include "../../../features.h"
#include "../../../../core/hooks/lightupdate/lightupdate.h"

__int64 FEATURES::VISUAL::WORLD::WORLDCOLOR::onUpdateLightScene(__int64 a1, C_LightScene* object, __int64 a3)
{
	if (!object)
		return oUpdateLightScene_t(a1, object, a3);

	ImColor old_color = object->m_color;

	if (CFG::VISUAL::WORLD::isWorldEnabled && CFG::VISUAL::WORLD::isLightModulationEnabled) {
		object->m_color = CFG::VISUAL::WORLD::lightModulationColor;
	}
	__int64 res = oUpdateLightScene_t(a1, object, a3);

	object->m_color = old_color;

	return res;
}