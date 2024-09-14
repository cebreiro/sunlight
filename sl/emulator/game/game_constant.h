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

        static constexpr int32_t INVENTORY_WIDTH = 6;
        static constexpr int32_t INVENTORY_HEIGHT = 9;
        static constexpr int32_t MAX_INVENTORY_PAGE_SIZE = 5;
    };
}
