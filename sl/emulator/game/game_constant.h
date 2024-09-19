#pragma once

namespace sunlight
{
    class GameConstant
    {
    public:
        GameConstant() = delete;

        static constexpr int32_t START_ZONE = 401;
        static constexpr float START_ZONE_X = 9685.f;
        static constexpr float START_ZONE_Y = 8652.f;

        static constexpr float STAGE_TERRAIN_BLOCK_SIZE = 256.f;

        // client 0x486748 0x48674F
        static constexpr int32_t PLAYER_CHARACTER_LEVEL_MAX = 70;
        static constexpr int32_t STAT_POINT_PER_CHARACTER_LEVEL_UP = 4;

        static constexpr int32_t INVENTORY_WIDTH = 6;
        static constexpr int32_t INVENTORY_HEIGHT = 9;
        static constexpr int32_t MAX_INVENTORY_PAGE_SIZE = 5;

        static constexpr int32_t QUICK_SLOT_WIDTH = 4;
        static constexpr int32_t QUICK_SLOT_HEIGHT = 2;
        static constexpr int32_t MAX_QUICK_SLOT_PAGE_SIZE = 10;
    };
}
