#include "tile.h"

#include "sl/emulator/game/game_constant.h"

namespace sunlight
{
    bool Tile::IsBlocked() const
    {
        return value < 0 || value >= 31;
    }

    auto Tile::GetPosition() const -> Eigen::Vector2f
    {
        return { static_cast<float>(index.x) * GameConstant::TILE_SIZE, static_cast<float>(index.y) * GameConstant::TILE_SIZE };
    }
}
