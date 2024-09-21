#pragma once
#include "sl/emulator/game/system/game_system.h"

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
        bool IsOnMovableArea(const Eigen::Vector2f& position) const;
        bool IsOnMovableArea(float x, float y) const;

        auto GetWidth() const -> int32_t;
        auto GetHeight() const -> int32_t;
        auto GetMovableArea1() const -> const std::vector<std::vector<char>>&;
        auto GetMovableArea2() const -> const std::vector<std::vector<char>>&;

    private:
        bool IsOutOfRange(float x, float y) const;

        static auto CalculateIndex(float x, float y) -> std::pair<int32_t, int32_t>;

    private:
        const ServiceLocator& _serviceLocator;
        const int32_t _width = 0;
        const int32_t _height = 0;


        std::vector<std::vector<char>> _movableBlocks1;
        std::vector<std::vector<char>> _movableBlocks2;

        static constexpr int32_t movable_block_per_stage_block = 4;
    };
}
