#pragma once

namespace sunlight::db
{
    enum class ItemPosType
    {
        Inventory,
        Equipment,
        QuickSlot,
        Pick,
    };

    static inline auto ToString(ItemPosType type) -> const char*
    {
        switch (type)
        {
        case ItemPosType::Inventory:
            return "inventory";
        case ItemPosType::Equipment:
            return "equipment";
        case ItemPosType::QuickSlot:
            return "quick_slot";
        case ItemPosType::Pick:
            return "pick";
        }

        assert(false);

        return nullptr;
    }
}
