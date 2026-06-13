#include "../../features.h"
#include "../../../core/mem.hpp"
#include "../../../sdk/ctx.hpp"



void FEATURES::MOVEMENT::BHOP::onMove(CUserCmd* pCmd)
{
    if (!CFG::MOVEMENT::isBhopEnabled)
        return;

    uintptr_t client = (uintptr_t)GetModuleHandleA("client.dll");
    if (!client)
        return;

    uintptr_t localPawn = (uintptr_t)context->localPawn;
    if (!localPawn)
        return;

    int flags = *(int*)(localPawn + offsets::m_fFlags);
 
    if (!(flags & FL_ONGROUND))
    {
        pCmd->nButtons.nValue &= ~IN_JUMP;
        pCmd->nButtons.nValueChanged &= ~IN_JUMP;
        pCmd->nButtons.nValueScroll &= ~IN_JUMP;
    }
}