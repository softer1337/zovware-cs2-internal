#include "../../../features.h"



void FEATURES::VISUAL::WORLD::CAMERA::onOverrideMove(__int64 a1, CViewSetup* a2)
{
	static float originalAspectRatio = a2->flAspectRatio;

	if (CFG::VISUAL::WORLD::isAspectRatioEnabled && CFG::VISUAL::WORLD::isWorldEnabled) {
		a2->flAspectRatio = CFG::VISUAL::WORLD::aspectRatio;
		a2->nSomeFlags |= 2;
	}
	else {
		a2->flAspectRatio = originalAspectRatio;
	}

	if (1 /*todo:check for cfg*/) {
		CGameTrace trace;
	}
}