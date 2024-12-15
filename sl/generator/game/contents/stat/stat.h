#pragma once
#include "sl/generator/game/contents/stat/stat_value.h"

namespace sunlight
{
    ENUM_CLASS(StatOriginType, int32_t,
        (Base, 0)
        (Item)
        (ItemPercentage)
        (SkillPassive)
        (SkillPassivePercentage)
        (StatusEffect)
        (StatusEffectPercentage)
        (JobReference)
    )

    class Stat
    {
    public:
        bool HasChanges() const;

        auto Get(StatOriginType type) const -> StatValue;
        auto GetStatSum() const -> StatValue;
        auto GetFinalValue() const -> StatValue;

        void Set(StatOriginType type, StatValue value);
        void SetStatSum(StatValue value);
        void SetFinalValue(StatValue value);
        void SetChanges(bool value);

    private:
        std::array<StatValue, StatOriginTypeMeta::GetSize()> _values = {};

        bool _changes = false;
        StatValue _statSum = {};
        StatValue _finalValue = {};
    };
}
