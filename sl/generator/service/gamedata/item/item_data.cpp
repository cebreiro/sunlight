#include "item_data.h"

#include "sl/generator/game/data/sox/item_weapon.h"
#include "sl/generator/game/data/sox/item_clothes.h"
#include "sl/generator/game/data/sox/item_edible.h"
#include "sl/generator/game/data/sox/item_etc.h"
#include "sl/generator/game/data/sox/item_pet_food.h"

namespace sunlight
{
    ItemData::ItemData(const sox::ItemWeapon& data)
    {
        Initialize(data);
    }

    ItemData::ItemData(const sox::ItemClothes& data)
    {
        Initialize(data);
    }

    ItemData::ItemData(const sox::ItemEdible& data)
    {
        Initialize(data);
    }

    ItemData::ItemData(const sox::ItemEtc& data)
    {
        Initialize(data);
    }

    ItemData::ItemData(const sox::ItemPetFood& data)
    {
        Initialize(data);
    }

    template <typename T>
    void ItemData::Initialize(const T& data)
    {
        _id = data.index;
        _width = data.invenWidth;
        _height = data.invenHeight;
        _maxOverlapCount = data.maxOverlapCount;

        _rarity = data.rarity;
        _price = data.price;
        _modelId = data.modelID;
        _modelColor = data.modelColor;

        _ableToSell = data.ableToSell;
        _ableToTrade = data.ableToTrade;
        _ableToDrop = data.ableToDrop;
        _ableToDestroy = data.ableToDestroy;
        _ableToStorage = data.ableToStorage;
        _ableToUseQuickSlot = data.useQuickSlot;
        _isOneMoreItem = data.oneMoreItem;

        if constexpr (!std::is_same_v<T, sox::ItemPetFood>)
        {
            _equipmentType = static_cast<sox::EquipmentType>(data.equipPos);
        }

        if constexpr (std::is_same_v<T, sox::ItemWeapon>)
        {
            _weaponData = &data;
        }
        else if constexpr (std::is_same_v<T, sox::ItemClothes>)
        {
            _clothesData = &data;
        }
        else if constexpr (std::is_same_v<T, sox::ItemEdible>)
        {
            _edibleData = &data;
        }
        else if constexpr (std::is_same_v<T, sox::ItemEtc>)
        {
            _etcData = &data;
        }
        else if constexpr (std::is_same_v<T, sox::ItemPetFood>)
        {
            _petFoodData = &data;
        }
    }

    auto ItemData::GetId() const -> int32_t
    {
        return _id;
    }

    auto ItemData::GetWidth() const -> int32_t
    {
        return _width;
    }

    auto ItemData::GetHeight() const -> int32_t
    {
        return _height;
    }

    auto ItemData::GetMaxOverlapCount() const -> int32_t
    {
        return _maxOverlapCount;
    }

    auto ItemData::GetRarity() const -> int32_t
    {
        return _rarity;
    }

    auto ItemData::GetPrice() const -> int32_t
    {
        return _price;
    }

    auto ItemData::GetModelId() const -> int32_t
    {
        return _modelId;
    }

    auto ItemData::GetModelColor() const -> int32_t
    {
        return _modelColor;
    }

    bool ItemData::IsAbleToSell() const
    {
        return _ableToSell;
    }

    bool ItemData::IsAbleToTrade() const
    {
        return _ableToTrade;
    }

    bool ItemData::IsAbleToDrop() const
    {
        return _ableToDrop;
    }

    bool ItemData::IsAbleToDestroy() const
    {
        return _ableToDestroy;
    }

    bool ItemData::IsAbleToStorage() const
    {
        return _ableToStorage;
    }

    bool ItemData::IsAbleToUseQuickSlot() const
    {
        return _ableToUseQuickSlot;
    }

    bool ItemData::IsIsOneMoreItem() const
    {
        return _isOneMoreItem;
    }

    auto ItemData::GetEquipmentType() const -> sox::EquipmentType
    {
        return _equipmentType;
    }

    auto ItemData::GetWeaponData() const -> const sox::ItemWeapon*
    {
        return _weaponData;
    }

    auto ItemData::GetClothesData() const -> const sox::ItemClothes*
    {
        return _clothesData;
    }

    auto ItemData::GetEdibleData() const -> const sox::ItemEdible*
    {
        return _edibleData;
    }

    auto ItemData::GetEtcData() const -> const sox::ItemEtc*
    {
        return _etcData;
    }

    auto ItemData::GetPetFoodData() const -> const sox::ItemPetFood*
    {
        return _petFoodData;
    }
}
