#pragma once
#include "sl/generator/game/component/game_component.h"

namespace sunlight
{
    class PlayerAppearanceComponent final : public GameComponent
    {
    public:
        auto GetHair() const -> int32_t;
        auto GetHairColor() const -> int32_t;
        auto GetFace() const -> int32_t;
        auto GetSkinColor() const -> int32_t;
        auto GetHatModelId() const -> int32_t;
        auto GetHatModelColor() const -> int32_t;
        auto GetJacketModelId() const -> int32_t;
        auto GetJacketModelColor() const -> int32_t;
        auto GetGlovesModelId() const -> int32_t;
        auto GetGlovesModelColor() const -> int32_t;
        auto GetPantsModelId() const -> int32_t;
        auto GetPantsModelColor() const -> int32_t;
        auto GetShoesModelId() const -> int32_t;
        auto GetShoesModelColor() const -> int32_t;
        auto GetWeaponModelId() const -> int32_t;
        auto GetWeaponModelColor() const -> int32_t;

        auto GetWeaponMotionCategory() const -> int32_t;

        void SetHair(int32_t hair);
        void SetHairColor(int32_t hairColor);
        void SetFace(int32_t face);
        void SetSkinColor(int32_t skinColor);
        void SetHatModelId(int32_t hatModelId);
        void SetHatModelColor(int32_t hatModelColor);
        void SetJacketModelId(int32_t jacketModelId);
        void SetJacketModelColor(int32_t jacketModelColor);
        void SetGlovesModelId(int32_t glovesModelId);
        void SetGlovesModelColor(int32_t glovesModelColor);
        void SetPantsModelId(int32_t pantsModelId);
        void SetPantsModelColor(int32_t pantsModelColor);
        void SetShoesModelId(int32_t shoesModelId);
        void SetShoesModelColor(int32_t shoesModelColor);
        void SetWeaponModelId(int32_t weaponModelId);
        void SetWeaponModelColor(int32_t weaponModelColor);

        void SetWeaponMotionCategory(int32_t weaponMotionCategory);

    private:
        int32_t _hair = 0;
        int32_t _hairColor = 0;
        int32_t _face = 0;
        int32_t _skinColor = 0;

        int32_t _hatModelId = 0;
        int32_t _hatModelColor = 0;
        int32_t _jacketModelId = 0;
        int32_t _jacketModelColor = 0;
        int32_t _glovesModelId = 0;
        int32_t _glovesModelColor = 0;
        int32_t _pantsModelId = 0;
        int32_t _pantsModelColor = 0;
        int32_t _shoesModelId = 0;
        int32_t _shoesModelColor = 0;
        int32_t _weaponModelId = 0;
        int32_t _weaponModelColor = 0;

        int32_t _weaponMotionCategory = 0;
    };
}
