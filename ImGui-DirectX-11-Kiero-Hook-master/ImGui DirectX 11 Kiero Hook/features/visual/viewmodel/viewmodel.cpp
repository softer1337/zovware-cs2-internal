#include "../../features.h"
#include "../../../core/hooks/viewmodel/calcviewmodel.h"


void FEATURES::VISUAL::VIEWMODEL::onCalcViewModel(float* flUnk, float* flOffsets, float* flFov)
{
	if (!CFG::VISUAL::VIEWMODEL::isViewModelEnabled)
		return;

    float flXOffset = CFG::VISUAL::VIEWMODEL::viewModelX;
    float flYOffset = CFG::VISUAL::VIEWMODEL::viewModelY;
    float flZOffset = CFG::VISUAL::VIEWMODEL::viewModelZ;
    float flViewmodelFov = CFG::VISUAL::VIEWMODEL::viewModelFOV;

    flOffsets[0] += flXOffset;
    flOffsets[1] += flYOffset;
    flOffsets[2] += flZOffset;

    *flFov = flViewmodelFov;
}