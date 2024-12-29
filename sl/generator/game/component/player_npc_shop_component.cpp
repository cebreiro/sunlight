#include "player_npc_shop_component.h"

namespace sunlight
{
    bool PlayerNPCShopComponent::HasShoppingNPC() const
    {
        return _shoppingNPC != nullptr;
    }

    auto PlayerNPCShopComponent::GetShoppingNPC() const -> GameNPC&
    {
        assert(HasShoppingNPC());

        return *_shoppingNPC;
    }

    void PlayerNPCShopComponent::SetShoppingNPC(GameNPC* npc)
    {
        _shoppingNPC = npc;
    }
}
