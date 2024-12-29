#include "path_finding_system.h"

#include "sl/data/map/map_stage_terrain.h"
#include "sl/generator/game/game_constant.h"

namespace sunlight
{
    PathFindingSystem::PathFindingSystem(const ServiceLocator& serviceLocator, const MapStageTerrain& terrain)
        : _serviceLocator(serviceLocator)
        , _width(terrain.width)
        , _height(terrain.height)
        , _xSize(_width * 16)
        , _ySize(_height * 16)
        , _mt(std::random_device{}())
    {
        _tiles.resize(_xSize * _ySize);

        constexpr double scale = GameConstant::TILE_SCALE;

        const auto isMovable = [&](double x, double y)
            {
                const int64_t l = *(terrain.movableData.data() + (static_cast<int64_t>(x * scale) / 8 + (_width * 2) * static_cast<int64_t>(y * scale))) & 0xFF;
                const int64_t r = (1 << (7 - static_cast<int64_t>(x * scale) % 8)) & 0xFF;

                return (l & r) == 0;
            };

        for (int32_t y = 0; y < _ySize; ++y)
        {
            for (int32_t x = 0; x < _xSize; ++x)
            {
                const bool result = isMovable(x * 16.0, y * 16.0);

                Tile& tile = _tiles[x + y * _xSize];

                tile.index = { x, y };
                tile.blocked = !result;
            }
        }

        for (int32_t y = 0; y < _ySize; ++y)
        {
            for (int32_t x = 0; x < _xSize; ++x)
            {
                Tile& current = GetTile(TileIndex(x, y));
                if (current.blocked)
                {
                    continue;
                }

                const std::initializer_list<std::pair<int32_t, int32_t>> initializerList{ std::pair{ 1, 0 }, std::pair{ -1, 0 }, std::pair{ 0, 1 }, std::pair{ 0, -1 } };

                for (const auto& [dx, dy] : initializerList)
                {
                    const int32_t otherX = x + dx;
                    const int32_t otherY = y + dy;

                    if (otherX < 0 || otherX >= _xSize || otherY < 0 || otherY >= _ySize)
                    {
                        continue;
                    }

                    Tile& other = GetTile(TileIndex(otherX, otherY));
                    if (!other.blocked)
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

    bool PathFindingSystem::IsBlocked(Eigen::Vector2f src, Eigen::Vector2f dest) const
    {
        const TileIndex start = CalculateXYIndex(src.x(), src.y());
        const TileIndex last = CalculateXYIndex(dest.x(), dest.y());

        return IsBlocked(start, last);
    }

    bool PathFindingSystem::GetRandPositionOnCircleOutLine(Eigen::Vector2f& result, Eigen::Vector2f position, float radius, bool rayTest)
    {
        constexpr std::array randAngles = { 0.f, 30.f, 60.f, 90.f, 120.f, 150.f, 180.f, 210.f, 240.f, 270.f, 300.f, 330.f };

        _positionBuffer.clear();

        for (const float angle : randAngles)
        {
            const float radian = angle * (static_cast<float>(std::numbers::pi) / 180.f);

            Eigen::Vector2f checkPosition = position;
            checkPosition.x() += std::cos(radian) * radius;
            checkPosition.y() += std::sin(radian) * radius;

            if (IsOutOfRange(checkPosition.x(), checkPosition.y()))
            {
                continue;
            }

            if (GetTile(CalculateXYIndex(checkPosition.x(), checkPosition.y())).blocked)
            {
                continue;
            }

            _positionBuffer.emplace_back(checkPosition);
        }

        if (_positionBuffer.empty())
        {
            return false;
        }

        if (rayTest)
        {
            if (!GetTile(CalculateXYIndex(position.x(), position.y())).blocked)
            {
                std::ranges::shuffle(_positionBuffer, _mt);

                while (!_positionBuffer.empty())
                {
                    if (IsBlocked(position, _positionBuffer.back()))
                    {
                        _positionBuffer.pop_back();

                        continue;
                    }

                    result = _positionBuffer.back();

                    return true;
                }

                return false;
            }
        }

        result = _positionBuffer[std::uniform_int_distribution<int64_t>(0, std::ssize(_positionBuffer) - 1)(_mt)];

        return true;
    }

    bool PathFindingSystem::GetRandPositionInCircle(Eigen::Vector2f& result, Eigen::Vector2f position, float radius, bool rayTest)
    {
        _tileBuffer.clear();

        const int32_t startX = std::max(0, static_cast<int32_t>(std::ceil((position.x() - radius) / GameConstant::TILE_SIZE)));
        const int32_t startY = std::max(0, static_cast<int32_t>(std::ceil((position.y() - radius) / GameConstant::TILE_SIZE)));
        const int32_t lastX = std::min(_xSize - 1, static_cast<int32_t>(std::floor((position.x() + radius) / GameConstant::TILE_SIZE)));
        const int32_t lastY = std::min(_ySize - 1, static_cast<int32_t>(std::floor((position.y() + radius) / GameConstant::TILE_SIZE)));

        for (int32_t y = startY; y <= lastY; ++y)
        {
            for (int32_t x = startX; x <= lastX; ++x)
            {
                assert(x >= 0 && x < _xSize && y >= 0 && y < _ySize);

                if (const Tile& tile = GetTile(TileIndex(x, y));
                    !tile.blocked)
                {
                    _tileBuffer.emplace_back(&tile);
                }
            }
        }

        if (_tileBuffer.empty())
        {
            return false;
        }

        std::optional<TileIndex> index = std::nullopt;
        const TileIndex src = CalculateXYIndex(position.x(), position.y());

        if (rayTest && !GetTile(src).blocked)
        {
            std::ranges::shuffle(_tileBuffer, _mt);

            while (!_tileBuffer.empty())
            {
                const TileIndex current = _tileBuffer.back()->index;

                if (IsBlocked(src, current))
                {
                    _tileBuffer.pop_back();

                    continue;
                }

                index = current;

                break;
            }
        }
        else
        {
            index = _tileBuffer[std::uniform_int_distribution<int64_t>(0, std::ssize(_tileBuffer) - 1)(_mt)]->index;
        }

        if (!index.has_value())
        {
            return false;
        }

        const Tile& selected = GetTile(*index);

        const float posX = static_cast<float>(selected.index.x) * GameConstant::TILE_SIZE;
        const float posY = static_cast<float>(selected.index.y) * GameConstant::TILE_SIZE;

        result.x() = std::uniform_real_distribution<float>(posX, posX + GameConstant::TILE_SIZE)(_mt);
        result.y() = std::uniform_real_distribution<float>(posY, posY + GameConstant::TILE_SIZE)(_mt);

        return true;
    }

    bool PathFindingSystem::GetRandPositionInBox(const Eigen::AlignedBox2f& box, Eigen::Vector2f& result)
    {
        _tileBuffer.clear();

        const int32_t startX = std::max(0, static_cast<int32_t>(std::ceil(box.min().x() / GameConstant::TILE_SIZE)));
        const int32_t startY = std::max(0, static_cast<int32_t>(std::ceil(box.min().y() / GameConstant::TILE_SIZE)));
        const int32_t lastX = std::min(_xSize - 1, static_cast<int32_t>(std::floor(box.max().x() / GameConstant::TILE_SIZE)));
        const int32_t lastY = std::min(_ySize - 1, static_cast<int32_t>(std::floor(box.max().y() / GameConstant::TILE_SIZE)));

        for (int32_t y = startY; y <= lastY; ++y)
        {
            for (int32_t x = startX; x <= lastX; ++x)
            {
                assert(x >= 0 && x < _xSize && y >= 0 && y < _ySize);

                if (const Tile& tile = GetTile(TileIndex(x, y));
                    !tile.blocked)
                {
                    _tileBuffer.emplace_back(&tile);
                }
            }
        }

        if (_tileBuffer.empty())
        {
            return false;
        }

        const int64_t index = std::uniform_int_distribution<int64_t>(0, std::ssize(_tileBuffer) - 1)(_mt);
        const Tile& selected = *_tileBuffer[index];

        const float posX = static_cast<float>(selected.index.x) * GameConstant::TILE_SIZE;
        const float posY = static_cast<float>(selected.index.y) * GameConstant::TILE_SIZE;

        result.x() = std::uniform_real_distribution<float>(posX, posX + GameConstant::TILE_SIZE)(_mt);
        result.y() = std::uniform_real_distribution<float>(posY, posY + GameConstant::TILE_SIZE)(_mt);

        return true;
    }

    bool PathFindingSystem::FindRawPath(std::vector<Eigen::Vector2f>& result, Eigen::Vector2f src, Eigen::Vector2f dest)
    {
        if (IsOutOfRange(src.x(), src.y()) || IsOutOfRange(dest.x(), dest.y()))
        {
            return false;
        }

        return FindPathRawReverse(result, CalculateXYIndex(dest.x(), dest.y()), CalculateXYIndex(src.x(), src.y()));
    }

    bool PathFindingSystem::FindPath(std::vector<Eigen::Vector2f>& result, Eigen::Vector2f src, Eigen::Vector2f dest)
    {
        _positionBuffer.clear();
        if (FindRawPath(_positionBuffer, src, dest))
        {
            SmoothPath(result, _positionBuffer);

            return true;
        }

        return false;
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

    bool PathFindingSystem::IsBlocked(TileIndex src, TileIndex dest) const
    {
        const int32_t dx = dest.x - src.x;
        const int32_t dy = dest.y - src.y;

        const int32_t steps = std::max(std::abs(dx), std::abs(dy));

        float x = static_cast<float>(src.x);
        float y = static_cast<float>(src.y);

        const float incrementX = static_cast<float>(dx) / static_cast<float>(steps);
        const float incrementY = static_cast<float>(dy) / static_cast<float>(steps);

        for (int32_t i = 0; i < steps; ++i)
        {
            const int32_t tileX = static_cast<int32_t>(std::roundf(x));
            const int32_t tileY = static_cast<int32_t>(std::roundf(y));

            if (tileX < 0 || tileX >= _xSize || tileY < 0 || tileY >= _ySize)
            {
                return true;
            }

            if (GetTile(TileIndex(tileX, tileY)).blocked)
            {
                return true;
            }

            x += incrementX;
            y += incrementY;
        }

        return false;
    }

    bool PathFindingSystem::FindPathRawReverse(std::vector<Eigen::Vector2f>& result, TileIndex start, TileIndex end)
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

    void PathFindingSystem::SmoothPath(std::vector<Eigen::Vector2f>& result, const std::vector<Eigen::Vector2f>& paths) const
    {
        if (std::ssize(paths) < 2)
        {
            std::ranges::copy(paths, std::back_inserter(result));

            return;
        }

        result.emplace_back(paths[0]);

        for (int64_t i = 2; i < std::ssize(paths) - 1; ++i)
        {
            const Eigen::Vector2f& from = result.back();
            const Eigen::Vector2f& to = paths[i];

            if (IsBlocked(from, to))
            {
                result.emplace_back(to);
            }
        }

        result.emplace_back(paths.back());
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
