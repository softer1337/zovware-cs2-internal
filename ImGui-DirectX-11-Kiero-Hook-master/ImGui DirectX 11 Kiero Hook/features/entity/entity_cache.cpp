#include "entity_cache.hpp"
#include "../features.h"

#include "../../sdk/hash.hpp"
#include "../../sdk/utl.hpp"


void FEATURES::ENTITY::onAddEntity(c_base_handle handle, CEntityInstance* inst) {

	if (!handle.is_valid() || !(handle.get_entry_index() < 0x3FF))
		return;

	int idx = handle.get_entry_index();
	if (idx < 0)
		return;

	C_BaseEntity* ent = (C_BaseEntity*)inst;

	if (!ent)
		return;

	auto* info = ent->getSchemaClassInfo();

	uint64_t classname_hashed = fnv1a::hash_64(info->GetName());

	if (classname_hashed == fnv1a::hash_64("C_CSPlayerPawn")) {
		CachedPlayer_t cached_player;

		cached_player.index = idx;
		cached_player.controller = Interface::GetEntitySystem()->getBaseBntity<C_CSPlayerController>(((C_CSPlayerPawn*)ent)->m_hOriginalController().getEntryIndex());
		cached_player.pawn = ((C_CSPlayerPawn*)ent);


		// доделать под контроллер и закончить кэщ
		CachedPlayers.push_back(cached_player);
	}
}


void FEATURES::ENTITY::onRemoveEntity(c_base_handle handle, CEntityInstance* inst)
{
	int idx = handle.get_entry_index();
	if (idx < 0) return;

	for (auto it = CachedPlayers.begin(); it != CachedPlayers.end(); ++it) {
		if (it->index == idx) {
			CachedPlayers.erase(it);
			break;
		}
	}
}