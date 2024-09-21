#include "path_finding_system.h"

#include "sl/data/map/map_stage_terrain.h"
#include "sl/emulator/game/game_constant.h"

namespace sunlight
{
    PathFindingSystem::PathFindingSystem(const ServiceLocator& serviceLocator, const MapStageTerrain& terrain)
        : _serviceLocator(serviceLocator)
        , _width(terrain.width)
        , _height(terrain.height)
    {
        _movableBlocks1.resize(_height * movable_block_per_stage_block, std::vector<char>(_width * movable_block_per_stage_block));
        _movableBlocks2.resize(_height * movable_block_per_stage_block, std::vector<char>(_width * movable_block_per_stage_block));

        int32_t index = 0;

        for (int32_t h = 0; h < _height * 4; ++h)
        {
            for (int32_t k = 0; k < 2; ++k)
            {
                for (int32_t i = 0; i <2; ++i)
                {
                    for (int32_t w = 0; w < _width * 2; ++w)
                    {
                        if (k == 0)
                        {
                            _movableBlocks1[h][w * 2 + i] = terrain.movableData[index];
                        }
                        else
                        {
                            _movableBlocks2[h][w * 2 + i] = terrain.movableData[index];
                        }

                        ++index;
                    }
                }
            }
        }
    }

    void PathFindingSystem::InitializeSubSystem(Stage& stage)
    {
        (void)stage;
    }

    bool PathFindingSystem::Subscribe(Stage& stage)
    {
        (void)stage;

        return true;
    }

    auto PathFindingSystem::GetName() const -> std::string_view
    {
        return "path_finding_system";
    }

    auto PathFindingSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<PathFindingSystem>();
    }

    bool PathFindingSystem::IsOnMovableArea(const Eigen::Vector2f& position) const
    {
        return IsOnMovableArea(position.x(), position.y());
    }

    bool PathFindingSystem::IsOnMovableArea(float x, float y) const
    {
        if (IsOutOfRange(x, y))
        {
            return false;
        }

        const auto [h, w] = CalculateIndex(x, y);

        return _movableBlocks1[h][w] == 0;
    }

    auto PathFindingSystem::GetWidth() const -> int32_t
    {
        return _width;
    }

    auto PathFindingSystem::GetHeight() const -> int32_t
    {
        return _height;
    }

    auto PathFindingSystem::GetMovableArea1() const -> const std::vector<std::vector<char>>&
    {
        return _movableBlocks1;
    }

    auto PathFindingSystem::GetMovableArea2() const -> const std::vector<std::vector<char>>&
    {
        return _movableBlocks2;
    }

    bool PathFindingSystem::IsOutOfRange(float x, float y) const
    {
        if (x < 0.0 || y < 0.0)
        {
            return true;
        }

        if (x >= static_cast<float>(_width) * GameConstant::STAGE_TERRAIN_BLOCK_SIZE ||
            y >= static_cast<float>(_height) * GameConstant::STAGE_TERRAIN_BLOCK_SIZE)
        {
            return true;
        }

        return false;
    }

    auto PathFindingSystem::CalculateIndex(float x, float y) -> std::pair<int32_t, int32_t>
    {
        constexpr float movableAreaBlockSize =
            GameConstant::STAGE_TERRAIN_BLOCK_SIZE / static_cast<float>(movable_block_per_stage_block);

        const int32_t h = static_cast<int32_t>(y / movableAreaBlockSize);
        const int32_t w = static_cast<int32_t>(x / movableAreaBlockSize);

        return { h, w };
    }
}
