#include "hooks.h"
#include "../../kiero/minhook/include/MinHook.h"
#include "drawarray/drawarray.h"
#include "skybox/updateskybox.h"
#include "generateprimitives/generateprimitives.h"
#include "mouseinp/mouseinp.h"
#include "drawaggregate/drawaggregate.h"
#include "../../core/hooks/smoke/smokedraw.h"
#include "chat/chat.h"
#include "viewmodel/calcviewmodel.h"
#include "tonemapupdate/tonemapupdate.h"
#include "entity/entity.h"

void InitHooks() {
	Hook_createmove();
	//Hook_drawarray(); // old
	Hook_generatePrimitives(); //new
	Hook_updateSkybox();
	Hook_MouseInputEnabled();
	Hook_drawAggregateSceneObject();
	Hook_ProcessSmoke();
	Hook_CalcViewModel();
	Hook_frameStage();
	Hook_tonemapUpdate();
	Hook_onAddEntity();
	Hook_onRemoveEntity();

	Init_ChatMessage();

}