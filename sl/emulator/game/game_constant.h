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

        static constexpr int32_t STAGE_MAIN = 10000;
        static constexpr float STAGE_TERRAIN_BLOCK_SIZE = 256.f;

        // client 0x486748 0x48674F
        static constexpr int32_t PLAYER_CHARACTER_LEVEL_MAX = 70;
        static constexpr int32_t STAT_POINT_PER_CHARACTER_LEVEL_UP = 4;

        static constexpr int32_t INVENTORY_WIDTH = 6;
        static constexpr int32_t INVENTORY_HEIGHT = 9;
        static constexpr int32_t MAX_INVENTORY_PAGE_SIZE = 5;

        static constexpr int32_t MAX_ACCOUNT_STORAGE_PAGE_SIZE = 5;

        static constexpr int32_t MAX_NPC_ITEM_SHOP_PAGE_SIZE = 6;

        static constexpr int32_t TRADE_STORAGE_WIDTH = 6;
        static constexpr int32_t TRADE_STORAGE_HEIGHT = 5;

        static constexpr int32_t MAX_STREET_VENDOR_PAGE_SIZE = 20;
        static constexpr int32_t MAX_STREET_VENDOR_STORED_ITEM_SIZE = 3;

        static constexpr int32_t QUICK_SLOT_WIDTH = 4;
        static constexpr int32_t QUICK_SLOT_HEIGHT = 2;
        static constexpr int32_t MAX_QUICK_SLOT_PAGE_SIZE = 10;

        static constexpr int32_t MAX_MIX_SKILL_DEGREE = 3;
        static constexpr int32_t MAX_MIX_SKILL_LEVEL = 30;

        // client 0x460C50
        static const std::array<int32_t, MAX_MIX_SKILL_LEVEL> MIX_SKILL_LEVEL_UP_REQUIRED_EXP_D1;
        static const std::array<int32_t, MAX_MIX_SKILL_LEVEL> MIX_SKILL_LEVEL_UP_REQUIRED_EXP_D2;
        static const std::array<int32_t, MAX_MIX_SKILL_LEVEL> MIX_SKILL_LEVEL_UP_REQUIRED_EXP_D3;
    };
}
