#include "player_appearance_component.h"

namespace sunlight
{
    auto PlayerAppearanceComponent::GetHair() const -> int32_t
    {
        return _hair;
    }

    auto PlayerAppearanceComponent::GetHairColor() const -> int32_t
    {
        return _hairColor;
    }

    auto PlayerAppearanceComponent::GetFace() const -> int32_t
    {
        return _face;
    }

    auto PlayerAppearanceComponent::GetSkinColor() const -> int32_t
    {
        return _skinColor;
    }

    auto PlayerAppearanceComponent::GetHatModelId() const -> int32_t
    {
        return _hatModelId;
    }

    auto PlayerAppearanceComponent::GetHatModelColor() const -> int32_t
    {
        return _hatModelColor;
    }

    auto PlayerAppearanceComponent::GetJacketModelId() const -> int32_t
    {
        return _jacketModelId;
    }

    auto PlayerAppearanceComponent::GetJacketModelColor() const -> int32_t
    {
        return _jacketModelColor;
    }

    auto PlayerAppearanceComponent::GetGlovesModelId() const -> int32_t
    {
        return _glovesModelId;
    }

    auto PlayerAppearanceComponent::GetGlovesModelColor() const -> int32_t
    {
        return _glovesModelColor;
    }

    auto PlayerAppearanceComponent::GetPantsModelId() const -> int32_t
    {
        return _pantsModelId;
    }

    auto PlayerAppearanceComponent::GetPantsModelColor() const -> int32_t
    {
        return _pantsModelColor;
    }

    auto PlayerAppearanceComponent::GetShoesModelId() const -> int32_t
    {
        return _shoesModelId;
    }

    auto PlayerAppearanceComponent::GetShoesModelColor() const -> int32_t
    {
        return _shoesModelColor;
    }

    auto PlayerAppearanceComponent::GetWeaponModelId() const -> int32_t
    {
        return _weaponModelId;
    }

    auto PlayerAppearanceComponent::GetWeaponModelColor() const -> int32_t
    {
        return _weaponModelColor;
    }

    auto PlayerAppearanceComponent::GetWeaponMotionCategory() const -> int32_t
    {
        return _weaponMotionCategory;
    }

    void PlayerAppearanceComponent::SetHair(int32_t hair)
    {
        _hair = hair;
    }

    void PlayerAppearanceComponent::SetHairColor(int32_t hairColor)
    {
        _hairColor = hairColor;
    }

    void PlayerAppearanceComponent::SetFace(int32_t face)
    {
        _face = face;
    }

    void PlayerAppearanceComponent::SetSkinColor(int32_t skinColor)
    {
        _skinColor = skinColor;
    }

    void PlayerAppearanceComponent::SetHatModelId(int32_t hatModelId)
    {
        _hatModelId = hatModelId;
    }

    void PlayerAppearanceComponent::SetHatModelColor(int32_t hatModelColor)
    {
        _hatModelColor = hatModelColor;
    }

    void PlayerAppearanceComponent::SetJacketModelId(int32_t jacketModelId)
    {
        _jacketModelId = jacketModelId;
    }

    void PlayerAppearanceComponent::SetJacketModelColor(int32_t jacketModelColor)
    {
        _jacketModelColor = jacketModelColor;
    }

    void PlayerAppearanceComponent::SetGlovesModelId(int32_t glovesModelId)
    {
        _glovesModelId = glovesModelId;
    }

    void PlayerAppearanceComponent::SetGlovesModelColor(int32_t glovesModelColor)
    {
        _glovesModelColor = glovesModelColor;
    }

    void PlayerAppearanceComponent::SetPantsModelId(int32_t pantsModelId)
    {
        _pantsModelId = pantsModelId;
    }

    void PlayerAppearanceComponent::SetPantsModelColor(int32_t pantsModelColor)
    {
        _pantsModelColor = pantsModelColor;
    }

    void PlayerAppearanceComponent::SetShoesModelId(int32_t shoesModelId)
    {
        _shoesModelId = shoesModelId;
    }

    void PlayerAppearanceComponent::SetShoesModelColor(int32_t shoesModelColor)
    {
        _shoesModelColor = shoesModelColor;
    }

    void PlayerAppearanceComponent::SetWeaponModelId(int32_t weaponModelId)
    {
        _weaponModelId = weaponModelId;
    }

    void PlayerAppearanceComponent::SetWeaponModelColor(int32_t weaponModelColor)
    {
        _weaponModelColor = weaponModelColor;
    }

    void PlayerAppearanceComponent::SetWeaponMotionCategory(int32_t weaponMotionCategory)
    {
        _weaponMotionCategory = weaponMotionCategory;
    }
}
