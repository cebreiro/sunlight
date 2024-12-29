#pragma once

namespace sunlight::db
{
    enum class ItemPosType
    {
        Inventory,
        Equipment,
        QuickSlot,
        Pick,
        Vendor,
        Mix,
    };

    inline auto ToString(ItemPosType type) -> const char*
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
        case ItemPosType::Vendor:
            return "vendor";
        case ItemPosType::Mix:
            return "mix";
        }

        assert(false);

        return nullptr;
    }

    inline auto CreateItemPosTypeFrom(const std::string& str) -> ItemPosType
    {
        if (str == "inventory")
        {
            return ItemPosType::Inventory;
        }
        else if (str == "equipment")
        {
            return ItemPosType::Equipment;
        }
        else if (str == "quick_slot")
        {
            return ItemPosType::QuickSlot;
        }
        else if (str == "pick")
        {
            return ItemPosType::Pick;
        }
        else if (str == "vendor")
        {
            return ItemPosType::Vendor;
        }
        else if (str == "mix")
        {
            return ItemPosType::Mix;
        }

        throw std::runtime_error(__FUNCTION__);
    }
}
