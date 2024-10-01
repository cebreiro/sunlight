#pragma once
#include "sl/emulator/server/client/game_client_id.h"

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
        bool StartStageChange(GamePlayer& player, int32_t destStageId, int32_t destX, int32_t destY);
        auto StartZoneChange(game_client_id_type clientId, int32_t destZoneId, int32_t destX, int32_t destY) -> Future<void>;

    private:
        Zone& _zone;
    };
}
