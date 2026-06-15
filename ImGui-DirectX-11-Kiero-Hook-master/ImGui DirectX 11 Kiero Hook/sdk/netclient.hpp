#pragma once
#include "globalvars.hpp"



class c_network_game_client
{
public:
    char pad_0x0000[0x90];

    float m_realtime;
    int m_frame_count;
    float m_frame_time;
    float m_frame_time2;
    int m_max_clients;

    char pad_0x009C[0x8];

    float m_player_interp;
    double m_some_timer_db;

    char pad_0x00B0[0x8];

    float m_interval_per_subtick;
    float m_current_time;
    float m_current_time2;

    char pad_0x00C4[0x8];

    bool m_in_prediction;

    char pad_0x00CD[0x3];

    int m_tick_count;
    int m_tick_count2;
    float m_some_timer;

    char pad_0x00DC[0x4];

    void* m_net_channel_info;

    char pad_0x00E8[0x8];

    bool m_should_predict;

    char pad_0x00F1[0x14B];

    int m_delta_tick;

    char pad_0x023C[0x124];

    int m_server_tick;

    char pad_0x0360[2642556];

    int m_client_tick;

    c_network_channel* get_net_channel()
    {
        return *(c_network_channel**)(reinterpret_cast<uintptr_t>(this) + 0xE0);
    }
};

class CNetworkGameClientServices
{
public:
	c_network_game_client* get_network_game_client() {
		return MEM::CallVFunc<c_network_game_client*, 23>(this);
	}
};