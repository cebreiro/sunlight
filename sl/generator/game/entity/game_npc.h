#pragma once
#include "sl/generator/game/entity/game_entity.h"

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
        auto GetUnk1() const -> int32_t;
        auto GetUnk2() const -> int32_t;

        void SetName(std::string name);
        void SetUnk1(int32_t value);
        void SetUnk2(int32_t value);

    private:
        int32_t _pnx = 0;
        std::string _name;

        int32_t _unk1 = 0;
        int32_t _unk2 = 0;
    };
}
