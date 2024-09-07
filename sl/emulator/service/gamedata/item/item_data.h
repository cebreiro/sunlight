#pragma once
#include "sl/data/sox/item/equipment_type.h"

namespace sunlight::sox
{
    struct ItemWeapon;
    struct ItemClothes;
    struct ItemEdible;
    struct ItemEtc;
    struct ItemPetFood;
}

namespace sunlight
{
    class ItemData
    {
    public:
        ItemData() = default;
        ItemData(const ItemData& other) = default;
        ItemData(ItemData&& other) noexcept = default;
        ItemData& operator=(const ItemData& other) = default;
        ItemData& operator=(ItemData&& other) noexcept = default;

    public:
        explicit ItemData(const sox::ItemWeapon& data);
        explicit ItemData(const sox::ItemClothes& data);
        explicit ItemData(const sox::ItemEdible& data);
        explicit ItemData(const sox::ItemEtc& data);
        explicit ItemData(const sox::ItemPetFood& data);

        auto GetId() const -> int32_t;
        auto GetWidth() const -> int32_t;
        auto GetHeight() const -> int32_t;
        auto GetMaxOverlapCount() const -> int32_t;
        auto GetRarity() const -> int32_t;
        auto GetPrice() const -> int32_t;
        auto GetModelId() const -> int32_t;
        auto GetModelColor() const -> int32_t;

        bool IsAbleToSell() const;
        bool IsAbleToTrade() const;
        bool IsAbleToDrop() const;
        bool IsAbleToDestroy() const;
        bool IsAbleToStorage() const;
        bool IsAbleToUseQuickSlot() const;
        bool IsIsOneMoreItem() const;

        auto GetEquipmentType() const -> sox::EquipmentType;
        auto GetWeaponData() const -> const sox::ItemWeapon*;
        auto GetClothesData() const -> const sox::ItemClothes*;
        auto GetEdibleData() const -> const sox::ItemEdible*;
        auto GetEtcData() const -> const sox::ItemEtc*;
        auto GetPetFoodData() const -> const sox::ItemPetFood*;

    private:
        template <typename T>
        void Initialize(const T& data);

    private:
        int32_t _id = 0;
        int32_t _width = 0;
        int32_t _height = 0;
        int32_t _maxOverlapCount = 0;
        int32_t _rarity = 0;
        int32_t _price = 0;
        int32_t _modelId = 0;
        int32_t _modelColor = 0;

        bool _ableToSell = false;
        bool _ableToTrade = false;
        bool _ableToDrop = false;
        bool _ableToDestroy = false;
        bool _ableToStorage = false;
        bool _ableToUseQuickSlot = false;
        bool _isOneMoreItem = false;

        sox::EquipmentType _equipmentType = sox::EquipmentType::None;

        const sox::ItemWeapon* _weaponData = nullptr;
        const sox::ItemClothes* _clothesData = nullptr;
        const sox::ItemEdible* _edibleData = nullptr;
        const sox::ItemEtc* _etcData = nullptr;
        const sox::ItemPetFood* _petFoodData = nullptr;
    };
}
