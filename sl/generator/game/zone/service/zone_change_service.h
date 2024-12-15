#pragma once
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/server/client/game_client_id.h"

namespace sunlight
{
    class Zone;

    class GamePlayer;
}

namespace sunlight
{
    class ZoneChangeService final : public IService
    {
    public:
        explicit ZoneChangeService(Zone& zone);

        auto GetName() const -> std::string_view override;

    public:
        void RegisterStageExitPortal(int32_t linkId, int32_t stageId, game_entity_id_type entityId);

    public:
        bool StartStageChange(GamePlayer& player, int32_t currentStageId, int32_t linkId);
        bool StartStageChange(GamePlayer& player, int32_t destStageId, int32_t destX, int32_t destY, std::optional<float> yaw = std::nullopt);
        auto StartZoneChange(game_client_id_type clientId, int32_t destZoneId, int32_t destX, int32_t destY,
            std::optional<float> yaw = std::nullopt) -> Future<void>;
        auto StartZoneChange(game_client_id_type clientId, int32_t destZoneId, int32_t destStage, int32_t destX, int32_t destY,
            std::optional<float> yaw = std::nullopt) -> Future<void>;

    private:
        Zone& _zone;
        std::unordered_multimap<int32_t, std::pair<int32_t, game_entity_id_type>> _stageExitPortalLinks;
    };
}
