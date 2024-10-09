#pragma once
#include "sl/emulator/game/contents/group/game_group_type.h"

namespace sunlight
{
    struct GameGroupState
    {
        std::string title;
        int32_t groupId = 0;
        GameGroupType groupType = {};
        int32_t type = 0;
    };
}
