#pragma once
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/entity/game_monster.h"
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
    class GameSpatialSector;
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

    private:
        void ProcessScan();

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
