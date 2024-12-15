#pragma once
#include "sl/generator/service/gamedata/item/item_data.h"

namespace sunlight::sox
{
    class ItemWeaponTable;
    class ItemClothesTable;
    class ItemEdibleTable;
    class ItemEtcTable;
    class ItemPetFoodTable;
}

namespace sunlight
{
    class SoxTableSet;
}

namespace sunlight
{
    class ItemDataProvider
    {
    public:
        ItemDataProvider(const ServiceLocator& serviceLocator, const SoxTableSet& tableSet);

        auto Find(int32_t itemId) const -> const ItemData*;

        auto GetName() const -> std::string_view;

    private:
        template <typename T>
        void Initialize(const ServiceLocator& serviceLocator, const T& table);

    private:
        SharedPtrNotNull<const sox::ItemWeaponTable> _weaponTable;
        SharedPtrNotNull<const sox::ItemClothesTable> _clothesTable;
        SharedPtrNotNull<const sox::ItemEdibleTable> _edibleTable;
        SharedPtrNotNull<const sox::ItemEtcTable> _etcTable;
        SharedPtrNotNull<const sox::ItemPetFoodTable> _petFoodTable;

        std::unordered_map<int64_t, ItemData> _itemData;
    };
}
