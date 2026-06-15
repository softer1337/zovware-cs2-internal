#pragma once
#include "../../sdk/c_csplayerpawn.hpp"
#include "../../sdk/tight_array.h"
#include "../../sdk/entitysystem.hpp"
#include "../aim/rage/penetration.h"
#include "lag comp/lag_comp.h"


class CachedPlayer_t {
public:
    CachedPlayer_t() = default;
    CachedPlayer_t(C_CSPlayerController* ent, int idx)
        : controller(ent), index(idx)
    {
    }

    C_CSPlayerController* controller = nullptr;
    C_CSPlayerPawn* pawn = nullptr;
    int                     index = 0;

    bool check_and_update_pawn() {
        if (!pawn) { printf("pawn is null"); return false; }
        //pawn = Interface::GetEntitySystem()
        //    ->getBaseBntity<C_CSPlayerPawn>(controller->m_hPawn().getEntryIndex());
        return (pawn != nullptr);
    }

    struct lag_compensation_data_t {
        tight_array<lag_record_t, 24> m_lag_records;
        __forceinline bool is_valid() { return m_valid; }
        __forceinline void validate() { m_valid = true; }
        __forceinline void invalidate() { 
            m_lag_records.clear(); m_valid = false;         printf(
                "[invalidate] size=%zu\n",
                m_lag_records.size()
            );
        }
        __forceinline void add_record(lag_record_t record) {
            printf(
                "[add_record] before=%zu sim=%.6f\n",
                m_lag_records.size(),
                record.m_simulation_time
            );

            m_lag_records.push_back(record);

            printf(
                "[add_record] after=%zu\n",
                m_lag_records.size()
            );

            validate();
        }
        __forceinline void remove_oldest() { m_lag_records.pop_front(); }

    private:
        bool m_valid;
    };
    lag_compensation_data_t                        m_lagcomp_data = {};
    c_penetration::player_context_t         m_penetration_context = {};
};

inline tight_array<CachedPlayer_t, 64> CachedPlayers;

