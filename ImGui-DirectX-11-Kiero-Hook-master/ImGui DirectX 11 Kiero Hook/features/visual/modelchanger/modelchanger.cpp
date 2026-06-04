#include "../../features.h"
#include "../../../core/hooks/hooks.h"




void FEATURES::VISUAL::MODELCHANGER::onFrameStage(int stage)
{
	if (!MEM::IsInGame())
		return;

	if (!CFG::VISUAL::MODELCHANGER::isModelChangerEnabled)
		return;

	static const char* szModelPath = "characters/models/kolka/nano_girl/nano_girl.vmdl";
	Interface::GetCResourceSystem()->preCache(szModelPath);
	if (!oSetModel)
		return;
		
	uintptr_t localPawn = MEM::GetLocalPawn();
	if (!localPawn)
		return;

	oSetModel(localPawn, szModelPath);
 
}