#pragma once

namespace sunlight
{
    struct TileIndex
    {
        TileIndex() = default;
        TileIndex(int32_t x, int32_t y);

        int32_t x = 0;
        int32_t y = 0;

        friend bool operator==(const TileIndex& lhs, const TileIndex& rhs);
        friend bool operator<(const TileIndex& lhs, const TileIndex& rhs);
    };
}
