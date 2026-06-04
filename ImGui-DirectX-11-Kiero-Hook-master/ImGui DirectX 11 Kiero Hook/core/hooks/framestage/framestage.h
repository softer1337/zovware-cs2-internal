#pragma once

enum FrameStage_t
{
    FRAME_UNDEFINED = -1,

    FRAME_START = 3,  // case 0
    FRAME_NET_UPDATE_START = 4,  // case 1

    FRAME_NET_UPDATE_END = 5,  // case 2
    // "FrameNetUpdate"

    FRAME_NET_POST_DATA_UPDATE = 6,  // case 3
    // "FramePostDataUpdate"

    FRAME_RENDER_START = 7,  // case 4

    FRAME_RENDER_END = 8,  // case 5

    FRAME_POST_RENDER = 9,  // case 6

    FRAME_RESTORE = 10, // case 7
    // "Client Restore Server State"

    FRAME_END = 11  // case 8
};

void Hook_frameStage();