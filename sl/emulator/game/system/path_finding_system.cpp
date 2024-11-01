#include "path_finding_system.h"

#include "sl/data/map/map_stage_terrain.h"
#include "sl/emulator/game/game_constant.h"

namespace sunlight
{
    PathFindingSystem::PathFindingSystem(const ServiceLocator& serviceLocator, const MapStageTerrain& terrain)
        : _serviceLocator(serviceLocator)
        , _width(terrain.width)
        , _height(terrain.height)
        , _xSize(_width * GameConstant::TILE_PER_STAGE_BLOCK)
        , _ySize(_height * GameConstant::TILE_PER_STAGE_BLOCK)
    {
        _tiles.resize(_xSize * _ySize);

        int32_t index = 0;

        for (int32_t h = 0; h < _height * 4; ++h)
        {
            for (int32_t k = 0; k < 2; ++k)
            {
                for (int32_t i = 0; i <2; ++i)
                {
                    for (int32_t w = 0; w < _width * 2; ++w)
                    {
                        const TileIndex tileIndex(w * 2 + i, h);
                        const int32_t flatIndex = CalculateFlatIndex(tileIndex);

                        if (k == 0)
                        {
                            Tile& tile = _tiles[flatIndex];
                            tile.index = tileIndex;
                            tile.value = terrain.movableData[index];
                        }
                        else
                        {
                            // unk.. data2
                        }

                        ++index;
                    }
                }
            }
        }

        constexpr std::array<std::pair<int32_t, int32_t>, 4> moveDirections = {

        };

        for (int32_t y = 0; y < _ySize; ++y)
        {
            for (int32_t x = 0; x < _xSize; ++x)
            {
                Tile& current = GetTile(TileIndex(x, y));
                if (current.IsBlocked())
                {
                    continue;
                }

                for (const auto& [dx, dy] : std::initializer_list{ std::pair{ 1, 0 }, std::pair{ -1, 0 }, std::pair{ 0, 1 }, std::pair{ 0, -1 } })
                {
                    const int32_t otherX = x + dx;
                    const int32_t otherY = y + dy;

                    if (otherX < 0 || otherX >= _xSize || otherY < 0 || otherY >= _ySize)
                    {
                        continue;
                    }

                    Tile& other = GetTile(TileIndex(otherX, otherY));
                    if (!other.IsBlocked())
                    {
                        current.connections.emplace_back(&other);
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

    bool PathFindingSystem::FindPath(std::vector<Eigen::Vector2f>& result, Eigen::Vector2f src, Eigen::Vector2f dest)
    {
        if (IsOutOfRange(src.x(), src.y()) || IsOutOfRange(dest.x(), dest.y()))
        {
            return false;
        }

        return FindPathReverse(result, CalculateXYIndex(dest.x(), dest.y()), CalculateXYIndex(src.x(), src.y()));
    }

    auto PathFindingSystem::GetWidth() const -> int32_t
    {
        return _width;
    }

    auto PathFindingSystem::GetHeight() const -> int32_t
    {
        return _height;
    }

    auto PathFindingSystem::GetXSize() const -> int32_t
    {
        return _xSize;
    }

    auto PathFindingSystem::GetYSize() const -> int32_t
    {
        return _ySize;
    }

    auto PathFindingSystem::GetTile(TileIndex index) -> Tile&
    {
        const int32_t flatIndex = CalculateFlatIndex(index);
        assert(flatIndex >= 0 && flatIndex <= std::ssize(_tiles));

        return _tiles[flatIndex];
    }

    auto PathFindingSystem::GetTile(TileIndex index) const -> const Tile&
    {
        const int32_t flatIndex = CalculateFlatIndex(index);
        assert(flatIndex >= 0 && flatIndex <= std::ssize(_tiles));

        return _tiles[flatIndex];
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

    bool PathFindingSystem::FindPathReverse(std::vector<Eigen::Vector2f>& result, TileIndex start, TileIndex end)
    {
        if (start == end)
        {
            return false;
        }

        struct Context
        {
            PtrNotNull<Tile> tile = nullptr;
            int32_t g = 0;
            int32_t h = 0;

            auto f() const -> int32_t
            {
                return g + h;
            }

            bool operator<(const Context& other) const
            {
                return f() > other.f();
            }
        };

        std::map<TileIndex, Context> paths;
        std::priority_queue<Context> openList;

        openList.push(Context{
            .tile = &GetTile(start),
            .h = CalculateHeuristic(start, end),
            });

        while (!openList.empty())
        {
            const Context current = openList.top();
            openList.pop();

            if (current.tile->index == end)
            {
                TileIndex index = current.tile->index;

                while (true)
                {
                    result.emplace_back(GetTile(index).GetPosition());

                    if (index == start)
                    {
                        break;
                    }

                    index = paths[index].tile->index;
                }

                return true;
            }

            for (Tile& next : current.tile->connections | notnull::reference)
            {
                const int32_t g = current.g + 1;
                const auto iter = paths.find(next.index);

                if (iter == paths.end() || g < iter->second.g)
                {
                    Context context;
                    context.tile = &next;
                    context.g = g;
                    context.h = CalculateHeuristic(next.index, end);

                    paths[next.index] = current;
                    openList.emplace(context);
                }
            }
        }

        return false;
    }

    auto PathFindingSystem::CalculateFlatIndex(TileIndex pair) const -> int32_t
    {
        assert(pair.x >= 0 && pair.x < _xSize);
        assert(pair.y >= 0 && pair.y < _ySize);

        return pair.x + (pair.y * _xSize);
    }

    auto PathFindingSystem::CalculateHeuristic(TileIndex lhs, TileIndex rhs) -> int32_t
    {
        return std::abs(lhs.x - rhs.x) + std::abs(lhs.y - rhs.y);
    }

    auto PathFindingSystem::CalculateXYIndex(float x, float y) -> TileIndex
    {
        const int32_t w = static_cast<int32_t>(x / GameConstant::TILE_SIZE);
        const int32_t h = static_cast<int32_t>(y / GameConstant::TILE_SIZE);

        return { w, h };
    }
}
