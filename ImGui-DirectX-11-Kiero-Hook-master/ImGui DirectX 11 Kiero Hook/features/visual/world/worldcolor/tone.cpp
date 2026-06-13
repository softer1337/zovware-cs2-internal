#include "../../../features.h"


void FEATURES::VISUAL::WORLD::WORLDCOLOR::onTonemapUpdate(__int64 a1)
{
	float value = 1.f;
	if (CFG::VISUAL::WORLD::isWorldEnabled && CFG::VISUAL::WORLD::isNightmodeEnabled) {
		value = 1.01f - (CFG::VISUAL::WORLD::nightmodeValue / 100.f);
	}

	MEM::CallVFunc<void, 5>((void*)a1, value);
}