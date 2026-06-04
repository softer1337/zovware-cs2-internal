#include "../../../../features/features.h"
#include "../../../../core/hooks/smoke/smokedraw.h"



void FEATURES::VISUAL::WORLD::SMOKE::onDrawSmoke(__int64 a1)
{
	if (CFG::VISUAL::WORLD::isSmokeEnabled && CFG::VISUAL::WORLD::isWorldEnabled) {
		*reinterpret_cast<Vec3*>(a1 + 0x125C) = CFG::VISUAL::WORLD::smokeColor;
	}
	oProcessSmoke(a1);
	return;
}