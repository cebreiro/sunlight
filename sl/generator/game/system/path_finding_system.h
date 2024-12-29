#pragma once
#include "sl/generator/game/contents/path_finding/tile.h"
#include "sl/generator/game/system/game_system.h"

namespace sunlight
{
    struct MapStageTerrain;
}

namespace sunlight
{
    class PathFindingSystem final : public GameSystem
    {
    public:
        PathFindingSystem(const ServiceLocator& serviceLocator, const MapStageTerrain& terrain);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        bool IsBlocked(Eigen::Vector2f src, Eigen::Vector2f dest) const;

        bool GetRandPositionOnCircleOutLine(Eigen::Vector2f& result, Eigen::Vector2f position, float radius, bool rayTest = true);
        bool GetRandPositionInCircle(Eigen::Vector2f& result, Eigen::Vector2f position, float radius, bool rayTest = true);
        bool GetRandPositionInBox(const Eigen::AlignedBox2f& box, Eigen::Vector2f& result);

        bool FindRawPath(std::vector<Eigen::Vector2f>& result, Eigen::Vector2f src, Eigen::Vector2f dest);
        bool FindPath(std::vector<Eigen::Vector2f>& result, Eigen::Vector2f src, Eigen::Vector2f dest);

    public:
        auto GetWidth() const -> int32_t;
        auto GetHeight() const -> int32_t;
        auto GetXSize() const -> int32_t;
        auto GetYSize() const -> int32_t;
        auto GetTile(TileIndex index) -> Tile&;
        auto GetTile(TileIndex index) const -> const Tile&;

    private:
        bool IsOutOfRange(float x, float y) const;
        bool IsBlocked(TileIndex src, TileIndex dest) const;

        bool FindPathRawReverse(std::vector<Eigen::Vector2f>& result, TileIndex start, TileIndex end);
        void SmoothPath(std::vector<Eigen::Vector2f>& result, const std::vector<Eigen::Vector2f>& paths) const;

        auto CalculateFlatIndex(TileIndex pair) const -> int32_t;

        static auto CalculateHeuristic(TileIndex lhs, TileIndex rhs) -> int32_t;
        static auto CalculateXYIndex(float x, float y) -> TileIndex;

    private:
        const ServiceLocator& _serviceLocator;;
        const int32_t _width = 0;
        const int32_t _height = 0;
        const int32_t _xSize = 0;
        const int32_t _ySize = 0;

        std::vector<Tile> _tiles;

        std::mt19937 _mt;
        std::vector<PtrNotNull<const Tile>> _tileBuffer;
        std::vector<Eigen::Vector2f> _positionBuffer;
    };
}
