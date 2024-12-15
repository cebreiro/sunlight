#pragma once
#include "sl/generator/service/gamedata/item/weapon_class_type.h"

namespace sunlight::sox
{
    struct MotionData;
}

namespace sunlight
{
    class SoxTableSet;
}

namespace sunlight
{
    class PlayerMotionDataProvider
    {
    public:
        PlayerMotionDataProvider(const SoxTableSet& tableSet);

        auto FindNormalAttackMotion(int32_t weaponClass, int32_t combo) const -> const sox::MotionData*;

    private:
        std::unordered_map<int32_t, std::vector<PtrNotNull<const sox::MotionData>>> _normalAttackMotions;
    };
}
