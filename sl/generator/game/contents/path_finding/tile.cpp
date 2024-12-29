#include "tile.h"

#include "sl/generator/game/game_constant.h"

namespace sunlight
{
    auto Tile::GetPosition() const -> Eigen::Vector2f
    {
        constexpr float median = GameConstant::TILE_SIZE / 2.f;

        const float x = static_cast<float>(index.x) * GameConstant::TILE_SIZE + median;
        const float y = static_cast<float>(index.y) * GameConstant::TILE_SIZE + median;

        return { x, y };
    }
}
