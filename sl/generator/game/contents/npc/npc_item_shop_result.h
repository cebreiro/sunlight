#pragma once

namespace sunlight
{
    ENUM_CLASS(NPCItemShopResult, int32_t,
        (Success, 0)
        (RemovePickedItem, 1)
        (NotEnoughGold, 2)
        (NotEnoughInventory, 3)
        (AlreadySold, 4)
        (JustFail, 11)
    )
}
