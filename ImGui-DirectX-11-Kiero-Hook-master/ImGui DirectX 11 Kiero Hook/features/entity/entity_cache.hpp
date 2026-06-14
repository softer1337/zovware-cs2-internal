#pragma once
#include "../../sdk/c_csplayerpawn.hpp"


class CachedPlayer_t {
public:
	C_CSPlayerPawn* pawn;
	C_CSPlayerController* controller;
	int index;
	CachedPlayer_t() = default;
	CachedPlayer_t(C_CSPlayerPawn* ent, C_CSPlayerController* cont, int idx)
		: controller(cont), index(idx), pawn(ent)
	{
	}
};

inline std::vector<CachedPlayer_t> CachedPlayers;

