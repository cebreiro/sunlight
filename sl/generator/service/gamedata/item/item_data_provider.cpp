#include "item_data_provider.h"

#include "sl/generator/game/data/sox_table_set.h"
#include "sl/generator/game/data/sox/item_weapon.h"
#include "sl/generator/game/data/sox/item_clothes.h"
#include "sl/generator/game/data/sox/item_edible.h"
#include "sl/generator/game/data/sox/item_etc.h"
#include "sl/generator/game/data/sox/item_pet_food.h"

namespace sunlight
{
    ItemDataProvider::ItemDataProvider(const ServiceLocator& serviceLocator, const SoxTableSet& tableSet)
        : _weaponTable(tableSet.Get<sox::ItemWeaponTable>().shared_from_this())
        , _clothesTable(tableSet.Get<sox::ItemClothesTable>().shared_from_this())
        , _edibleTable(tableSet.Get<sox::ItemEdibleTable>().shared_from_this())
        , _etcTable(tableSet.Get<sox::ItemEtcTable>().shared_from_this())
        , _petFoodTable(tableSet.Get<sox::ItemPetFoodTable>().shared_from_this())
    {
        Initialize(serviceLocator, *_weaponTable);
        Initialize(serviceLocator, *_clothesTable);
        Initialize(serviceLocator, *_edibleTable);
        Initialize(serviceLocator, *_etcTable);
        Initialize(serviceLocator, *_petFoodTable);
    }

    auto ItemDataProvider::Find(int32_t itemId) const -> const ItemData*
    {
        const auto iter = _itemData.find(itemId);

        return iter != _itemData.end() ? &iter->second : nullptr;
    }

    auto ItemDataProvider::GetName() const -> std::string_view
    {
        return "item_data_provider";
    }

    template <typename T>
    void ItemDataProvider::Initialize(const ServiceLocator& serviceLocator, const T& table)
    {
        for (const auto& soxData : table.Get())
        {
            if (!_itemData.try_emplace(soxData.index, ItemData(soxData)).second)
            {
                SUNLIGHT_LOG_ERROR(serviceLocator,
                    fmt::format("[{}] fail to add item data. duplicate item id. id: {}",
                        GetName(), soxData.index));
            }
        }
    }
}
