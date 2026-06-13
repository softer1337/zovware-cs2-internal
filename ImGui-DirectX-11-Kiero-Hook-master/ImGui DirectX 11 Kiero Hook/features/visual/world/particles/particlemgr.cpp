// ash - particles/rain_fx/ash_burning.vpcf_c
#include "../../../features.h"
#include "../../../../sdk/globalvars.hpp"
#include "../../../../sdk/particlesystem.hpp"
#include "../../../../sdk/c_csplayerpawn.hpp"

struct BeamObject_t {
	float m_time_added;
	float m_duration;
	unsigned int m_effect_index;
};

std::vector<BeamObject_t> m_active_beams;
std::vector<unsigned int> m_vec_effect_indexes;
std::vector<BeamObject_t> m_circle_beams;



static unsigned int ash_idx = 0;
static bool ash_loaded = false;

void render_ash()
{
	if (!ash_loaded)
	{
		CBufferString path("particles/explosions_fx/explosion_basic.vpcf", 'fcpv');
		Interface::GetCResourceSystem()->BlockingLoadResourceByName(&path);
		ash_loaded = Interface::GetGameParticleManager()->create_particle_effect(&ash_idx, "particles/explosions_fx/explosion_basic.vpcf");
	}

	if (!ash_loaded || !ash_idx)
		return;

	auto* local = (C_CSPlayerPawn*)MEM::GetLocalPawn();
	if (!local)
		return;

	Vec3 pos = local->m_pGameSceneNode()->m_vecAbsOrigin();
	Vec3 des = Vec3(500.f, 0.f, 0.f);
	Vec3 col = Vec3(255.f, 0.f, 255.f);



	Interface::GetGameParticleManager()->set_particle_data(ash_idx, e_particle_setting::particle_setting_density, &des);
	Interface::GetGameParticleManager()->set_particle_data(ash_idx, e_particle_setting::particle_setting_color, &col);

	Interface::GetGameParticleManager()->set_particle_data(ash_idx, e_particle_setting::particle_setting_position, &pos);
}


void FEATURES::VISUAL::WORLD::PARTICLES::onFrameStage(int stage)
{
	render_ash();

	if (stage != 8)
		return;

	if (!Interface::GetGlobalVars())
		return;

	auto it = m_active_beams.begin();
	while (it != m_active_beams.end()) {
		auto& beam = *it;
		float current_time = Interface::GetGlobalVars()->m_curtime;
		float delta = fabsf(current_time - beam.m_time_added);

		if (delta > beam.m_duration) {
			Interface::GetGameParticleManager()->destroy_particle(beam.m_effect_index, true, true);
			Interface::GetGameParticleManager()->release_particle_index(beam.m_effect_index);
			it = m_active_beams.erase(it);
		}
		else {
			++it;
		}
	}

}