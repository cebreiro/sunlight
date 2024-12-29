#include "tile_index.h"

namespace sunlight
{
    TileIndex::TileIndex(int32_t x, int32_t y)
        : x(x)
        , y(y)
    {
    }

    bool operator==(const TileIndex& lhs, const TileIndex& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    bool operator<(const TileIndex& lhs, const TileIndex& rhs)
    {
        if (lhs.x == rhs.x)
        {
            return lhs.y < rhs.y;
        }

        return lhs.x < rhs.x;
    }
}
