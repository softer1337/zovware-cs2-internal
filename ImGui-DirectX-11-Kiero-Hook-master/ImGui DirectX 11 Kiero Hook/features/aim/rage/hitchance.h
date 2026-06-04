#pragma once
#include "../../../sdk/math.hpp"
#include "../../../sdk/c_csplayerpawn.hpp"
#include "../../../sdk/econsdk.hpp"
#include "../../../sdk/qangle.h"
#include "../../cfg.hpp"
#include "../../../sdk/trace/trace.h"


bool IsAccurate(QAngle targetAngle, C_CSPlayerPawn* target, Vec3 start);