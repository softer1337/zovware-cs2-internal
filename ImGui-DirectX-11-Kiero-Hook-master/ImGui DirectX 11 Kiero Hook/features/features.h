#pragma once
#include "../sdk/input/usercmd.h"
#include "cfg.hpp"
#include "../sdk/mesh_sdk.h"
#include "../core/hooks/drawaggregate/drawaggregate.h"

namespace FEATURES {
	namespace MOVEMENT {
		namespace BHOP {
			void onMove(CUserCmd* pCmd);
		}
		namespace AUTOSTOP {
			void onMove(CUserCmd* pCmd, bool want);
		}
		namespace STRAFE {
			void subtick_air_strafer(bool stop);
		}
	}
	
	namespace SKINS {
		void onFrameStage(int stage);
	}

	namespace VISUAL {
		namespace ESP {
			void onRender();
		}
		namespace VIEWMODEL {
			void onCalcViewModel(float* flUnk, float* flOffsets, float* flFov);
		}
		namespace MODELCHANGER {
			void onFrameStage(int stage);
		}
		namespace CHAMS {
			void onDrawArray(CSceneAnimatableObjectDesc* desc, void* a2, CMeshDrawPrimitive_t* mesh_draw, int a4, void* a5, void* a6, void* a7);
			bool onGeneratePrimitives(CSceneAnimatableObject* object, void* a3, CMeshPrimitiveOutputBuffer* render_buf, void* thisptr);
		}
		namespace WORLD {
			namespace SKYBOX {
				void onUpdateSkyBox(__int64 a1, __int64 a2, __int64 a3, int nCount, int render_flags, __int64 a6, __int64 a7);
				void init_on_level_load();
				void change_skybox_material();
				void restore_default_skybox();
			}
			namespace WORLDCOLOR {
				void onDrawAggregateSceneObject(__int64 a1, __int64 a2, CAggregateObjectArray* a3);
				void onTonemapUpdate(__int64 a1);
			}
			namespace SMOKE {
				void onDrawSmoke(__int64 a1);
			}
			namespace PARTICLES {
				void onFrameStage(int stage);
			}
		}
	}
	namespace ENTITY {
		void onAddEntity(c_base_handle handle, CEntityInstance* inst);
		void onRemoveEntity(c_base_handle handle, CEntityInstance* inst);
	}
	namespace AIM {
		namespace LEGITBOT {
			void onMove(CUserCmd* pCmd);
		}
		namespace RAGEBOT {
			void onMove(CUserCmd* pCmd);
		}
	}
}