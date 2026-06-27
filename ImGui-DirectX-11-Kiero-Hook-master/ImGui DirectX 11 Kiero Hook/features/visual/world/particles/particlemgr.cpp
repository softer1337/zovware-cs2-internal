// ash - particles/rain_fx/ash_burning.vpcf_c
#include "../../../features.h"
#include "../../../../sdk/globalvars.hpp"
#include "../../../../sdk/particlesystem.hpp"
#include "../../../../sdk/c_csplayerpawn.hpp"
#include "../../../../sdk/ctx.hpp"

constexpr int WEATHER_MAX_RANGE = 750;
constexpr int WEATHER_STEP = 75;
constexpr int WEATHER_LAYERS = WEATHER_MAX_RANGE / WEATHER_STEP;
constexpr int TOTAL_PARTICLES_NEEDED = WEATHER_LAYERS * 4;

struct BeamObject_t {
	float m_time_added;
	float m_duration;
	unsigned int m_effect_index;
};

std::vector<BeamObject_t> m_active_beams;
std::vector<unsigned int> m_vec_effect_indexes;
std::vector<BeamObject_t> m_circle_beams;


bool m_b_new_round_callback = false;
bool opt_enable_world_weather = true;
Vec3 m_weather_ash_color = Vec3(1.f, 0.f, 0.f);

int weather_ind = 0;

void drawWeather()
{	
	//remove effects

	auto remove_effects = []() {

		if (Interface::GetGameParticleManager()) {
			for (unsigned int& i : m_vec_effect_indexes) {
				if (i != 0) {
					Interface::GetGameParticleManager()->destroy_particle(i, true, true);
					Interface::GetGameParticleManager()->release_particle_index(i);
				}
			}
		}
		m_vec_effect_indexes.clear();
		};
	if (!MEM::IsInGame()) {
		if (!m_vec_effect_indexes.empty()) {
			remove_effects();
			m_b_new_round_callback = true;
		}
		return;
	}

	// create effects
	auto create_effects = []() {
		static const char* m_sz_particle_path_rain = "particles/rain_fx/rain_edge_sparse.vpcf";
		static const char* m_sz_particle_path_ash = "particles/rain_fx/ash_burning.vpcf";
		const char* target_path = (weather_ind == 0) ? m_sz_particle_path_rain : m_sz_particle_path_ash;

		if (Interface::GetCResourceSystem()) {
			CBufferString weather_resource(target_path, 'fcpv');
			Interface::GetCResourceSystem()->BlockingLoadResourceByName(&weather_resource);
		}

		std::vector<unsigned int> u_effect_index(TOTAL_PARTICLES_NEEDED, 0);
		for (int i = 0; i < TOTAL_PARTICLES_NEEDED; i++) {
			unsigned int effect_index = 0;

			if (Interface::GetGameParticleManager()->create_particle_effect(&effect_index, target_path) != nullptr) {
				if (effect_index != 0) {
					u_effect_index[i] = effect_index;
				}
			}
		}

		for (int i = 0; i < TOTAL_PARTICLES_NEEDED; i++) {
			if (u_effect_index[i] != 0)
				m_vec_effect_indexes.push_back(u_effect_index[i]);
		}
		};

	static bool m_b_last_world_particles = false;
	static int m_i_last_particles_type = -1;

	bool m_b_particles_changed = (m_b_last_world_particles != opt_enable_world_weather);
	bool m_b_type_changed = (m_i_last_particles_type != weather_ind);

	m_b_last_world_particles = opt_enable_world_weather;
	m_i_last_particles_type = weather_ind;

	if (m_b_new_round_callback || m_b_particles_changed || m_b_type_changed) {
		if (!m_vec_effect_indexes.empty())
			remove_effects();

		create_effects();
		m_b_new_round_callback = false;
	}


	if (!m_vec_effect_indexes.empty()) {

		Vec3 player_origin = context->localPawn->m_vOldOrigin();
		player_origin.z += 180.0f;

		Vec3 den = Vec3(1000.f, 0.f, 0.f);
		particle_color_t ash_color = {
			m_weather_ash_color.x * 255.f,
			m_weather_ash_color.y * 255.f,
			m_weather_ash_color.z * 255.f
		};

		int current_particle_slot = 0;

		for (int layer_idx = 0; layer_idx < WEATHER_LAYERS; layer_idx++) {
			float dist = (layer_idx + 1) * (float)WEATHER_STEP;

			float x_offsets[4] = { dist, -dist, dist, -dist };
			float y_offsets[4] = { dist, dist, -dist, -dist };

			for (int dir_idx = 0; dir_idx < 4; dir_idx++) {
				if (current_particle_slot < m_vec_effect_indexes.size()) {
					unsigned int effect_idx = m_vec_effect_indexes[current_particle_slot];

					if (effect_idx != 0) {
						Vec3 target_pos = player_origin;
						target_pos.x += x_offsets[dir_idx];
						target_pos.y += y_offsets[dir_idx];

						Interface::GetGameParticleManager()->set_particle_data(effect_idx, 0U, &target_pos);

						if (weather_ind == 1) {
							Interface::GetGameParticleManager()->set_particle_data(effect_idx, 1U, &ash_color);
						}
						Interface::GetGameParticleManager()->set_particle_data(effect_idx, 2U, &den);
					}
					current_particle_slot++;
				}
			}
		}
	}
}

void updateBeams(int stage) {
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

void FEATURES::VISUAL::WORLD::PARTICLES::onFrameStage(int stage)
{
	updateBeams(stage);
	drawWeather();

}