#pragma once
#include <cstdint>
#include "../imgui/imgui.h"
#include "../sdk/math.hpp"

enum class EspType {
	BOX,
	CORNER,
	BOX_3D
};

enum class ChamsMat {
	FLAT,
	SOLID,
	BLOOM,
	GLOW,
	GHOST,
	LATEX
};

struct knife_changer_t {
	bool m_enabled = false;
	int m_knife = 0;
	int m_paint_kit = 0;
	float m_wear = 0.0001f;
	int m_seed = 0;
	char m_custom_name[161] = {};
};

struct glove_changer_t {
	bool m_enabled = false;
	int m_glove = 0;
	int m_paint_kit = 0;
	float m_wear = 0.0001f;
	int m_seed = 0;
};

struct skin_changer_t {
	bool m_enabled = false;
	int m_selected_weapon = 0;

	struct weapon_skin_t {
		int paint_kit = 0;
		float wear = 0.0001f;
		int seed = 0;
		char custom_name[161] = {};
	};

	weapon_skin_t weapon_skins[100];

	static int GetConfigIndex(uint16_t def_index) {
		if (def_index >= 1 && def_index <= 70) return def_index;
		return 0;
	}
};

namespace CFG {
	namespace MOVEMENT {
		inline bool isBhopEnabled = false;
	}
	namespace SKINS {
		inline bool isSkinsEnabled = true;
		inline knife_changer_t knifeChanger;
		inline skin_changer_t skinChanger;
	}
	namespace VISUAL {
		namespace ESP {
			inline bool isEspEnabled = false;
			inline ImColor espColor = ImColor(1.f, 0.f, 0.f, 1.f);
			inline EspType espType = EspType::BOX;
		}
		namespace MODELCHANGER {
			inline bool isModelChangerEnabled = false;
		}
		namespace VIEWMODEL {
			inline bool isViewModelEnabled = false;
			inline float viewModelFOV = 90.f;
			inline float viewModelX = 0.f;
			inline float viewModelY = 0.f;
			inline float viewModelZ = 0.f;
		}
		namespace CHAMS {
			inline bool isChamsEnabled = false;
			inline ImColor chamsVisColor = ImColor(1.f, 1.f, 1.f, 1.f);
			inline ImColor chamsInvisColor = ImColor(1.f, 1.f, 1.f, 1.f);
			inline ChamsMat curMat = ChamsMat::GHOST;

			inline bool isArmsEnabled = false;
			inline ImColor chamsArmsColor = ImColor(1.f, 1.f, 1.f, 1.f);
			inline ChamsMat curArmsMat = ChamsMat::BLOOM;

			inline bool isInvisEnabled = true;
			inline bool isVisEnabled = true;
		}
		namespace WORLD {
			inline bool isWorldEnabled = true;

			inline bool isSkyboxEnabled = false;
			inline Vec3 skyboxColor = Vec3(1.f, 0.f, 1.f);

			inline bool isCustomSkyboxEnabled = false;
			inline int selectedSkybox = 0;

			inline bool isSmokeEnabled = true;
			inline Vec3 smokeColor = Vec3(1.f, 0.f, 1.f);

			inline bool isNightmodeEnabled = true;
			inline int nightmodeValue = 100;

			inline bool isWorldModulationEnabled = false;
			inline Vec3 worldModulationColor = Vec3(1.f, 0.f, 1.f);
		}
	}
	namespace AIM {
		namespace LEGITBOT {
			inline bool isAimEnabled = false;
			inline float fov = 5.f;
			inline float smooth = 30.f;
		}
		namespace RAGEBOT {
			inline bool isAimEnabled = false;
			inline bool isSilentEnabled = false;
			inline bool isPenetrationEnabled = false;
			inline bool isForceShootEnabled = false;
			inline int minDamage = 1;
			inline float fov = 5.f;
			inline int hitChance = 71;

			inline bool internal_wantsStop = false;
		}
	}
}