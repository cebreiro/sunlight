#pragma once
#include "sl/emulator/game/entity/game_entity.h"

namespace sunlight
{
    class GameEntityIdPublisher;
}

namespace sunlight
{
    class GameNPC final : public GameEntity
    {
    public:
        static constexpr GameEntityType TYPE = GameEntityType::NPC;

    public:
        GameNPC(game_entity_id_type id, int32_t pnx);

        auto GetPNX() const -> int32_t;
        auto GetName() const -> const std::string&;

        void SetName(std::string name);

    private:
        int32_t _pnx = 0;
        std::string _name;
    };
}
