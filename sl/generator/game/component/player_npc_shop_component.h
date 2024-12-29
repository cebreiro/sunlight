#pragma once
#include "sl/generator/game/component/game_component.h"
#include "sl/generator/game/entity/game_entity_id_type.h"

namespace sunlight
{
    class GameNPC;
}

namespace sunlight
{
    class PlayerNPCShopComponent final : public GameComponent
    {
    public:
        bool HasShoppingNPC() const;

        auto GetShoppingNPC() const -> GameNPC&;
        void SetShoppingNPC(GameNPC* npc);

    private:
        GameNPC* _shoppingNPC = nullptr;
    };
}
