#pragma once
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/entity/game_monster.h"
#include "sl/generator/game/system/game_system.h"

namespace sunlight::collision
{
    class Circle;
}

namespace sunlight
{
    class GameSpatialSector;
    class GameSpatialCell;
}

namespace sunlight
{
    class EntityScanSystem final : public GameSystem
    {
    public:
        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        bool ShouldUpdate() const override;
        void Update() override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        auto ScanPlayerAsync(std::vector<std::pair<game_entity_id_type, float>>& result, const GameMonster& monster, float range) -> Future<void>;

        void ScanMonsterAttackTarget(std::vector<game_entity_id_type>& result, const Eigen::Vector2f& position, float range);

    private:
        void ProcessScan();

    private:
        static auto CalculateCellMinDistanceSq(const GameSpatialSector& sector, const GameSpatialCell& cellInSector, const Eigen::Vector2f& posInSector) -> float;
        static bool IsIntersection(const GameEntity& target, const collision::Circle& circle);

    private:
        struct ScanRequest
        {
            GameEntityType type = {};
            Eigen::Vector2f center = {};
            float range = 0.f;

            PtrNotNull<std::vector<std::pair<game_entity_id_type, float>>> result = nullptr;
            std::optional<Promise<void>> promise = std::nullopt;

            GameSpatialSector* sector = nullptr;
        };

        std::vector<ScanRequest> _scanRequests;
        std::vector<std::ranges::subrange<std::vector<ScanRequest>::iterator>> _scanRequestSubRanges;
    };
}
