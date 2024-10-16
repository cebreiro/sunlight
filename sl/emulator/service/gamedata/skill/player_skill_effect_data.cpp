#include "player_skill_effect_data.h"

#include "sl/emulator/game/data/sox/item_weapon.h"

namespace sunlight
{
    PlayerSkillEffectDamage::PlayerSkillEffectDamage(const SkillEffectData& skillEffectData)
        : _data(&skillEffectData)
        , _type(skillEffectData.type)
        , _damagePerSkillLevel(skillEffectData.value1)
        , _baseDamage(skillEffectData.value2)
        , _randMin(skillEffectData.value3)
        , _randMax(skillEffectData.value4)
    {
    }

    bool PlayerSkillEffectDamage::IsOneShotKillEffect() const
    {
        return _baseDamage == 0;
    }

    auto PlayerSkillEffectDamage::GetData() const -> const SkillEffectData&
    {
        assert(_data);

        return *_data;
    }

    auto PlayerSkillEffectDamage::GetType() const -> int32_t
    {
        return _type;
    }

    auto PlayerSkillEffectDamage::GetDamagePerSkillLevel() const -> int32_t
    {
        return _damagePerSkillLevel;
    }

    auto PlayerSkillEffectDamage::GetBaseDamage() const -> int32_t
    {
        return _baseDamage;
    }

    auto PlayerSkillEffectDamage::GetDamageRandMin() const -> int32_t
    {
        return _randMin;
    }

    auto PlayerSkillEffectDamage::GetDamageRandMax() const -> int32_t
    {
        return _randMax;
    }

    SkillEffectStatusEffect::SkillEffectStatusEffect(const SkillEffectData& skillEffectData)
        : _data(&skillEffectData)
        , _type(static_cast<StatusEffectType>(skillEffectData.type))
        , _valuePerSkillLevel(skillEffectData.value1)
        , _baseValue(skillEffectData.value2)
        , _durationPerSkillLevel(skillEffectData.value3)
        , _baseDuration(skillEffectData.value4)
        , _id(skillEffectData.value5)
    {
    }

    auto SkillEffectStatusEffect::GetRawData() const -> const SkillEffectData&
    {
        assert(_data);

        return *_data;
    }

    auto SkillEffectStatusEffect::GetType() const -> StatusEffectType
    {
        return _type;
    }

    auto SkillEffectStatusEffect::GetValuePerSkillLevel() const -> int32_t
    {
        return _valuePerSkillLevel;
    }

    auto SkillEffectStatusEffect::GetBaseValue() const -> int32_t
    {
        return _baseValue;
    }

    auto SkillEffectStatusEffect::GetDurationPerSkillLevel() const -> int32_t
    {
        return _durationPerSkillLevel;
    }

    auto SkillEffectStatusEffect::GetBaseDuration() const -> int32_t
    {
        return _baseDuration;
    }

    auto SkillEffectStatusEffect::GetId() const -> int32_t
    {
        return _id;
    }

    PlayerSkillEffectPassiveStat::PlayerSkillEffectPassiveStat(const SkillEffectData& skillEffectData)
        : _statType(static_cast<PlayerStatType>(skillEffectData.type))
        , _valuePerSkillLevel(skillEffectData.value1)
        , _baseValue(skillEffectData.value2)
        , _percentageValuePerSkillLevel(skillEffectData.value3)
        , _percentageBaseValue(skillEffectData.value4)
    {
    }

    auto PlayerSkillEffectPassiveStat::GetStatType() const -> PlayerStatType
    {
        return _statType;
    }

    auto PlayerSkillEffectPassiveStat::GetValuePerSkillLevel() const -> int32_t
    {
        return _valuePerSkillLevel;
    }

    auto PlayerSkillEffectPassiveStat::GetBaseValue() const -> int32_t
    {
        return _baseValue;
    }

    auto PlayerSkillEffectPassiveStat::GetPercentageValuePerSkillLevel() const -> int32_t
    {
        return _percentageValuePerSkillLevel;
    }

    auto PlayerSkillEffectPassiveStat::GetPercentageBaseValue() const -> int32_t
    {
        return _percentageBaseValue;
    }

    PlayerSkillEffectWeaponClassRestriction::PlayerSkillEffectWeaponClassRestriction(int32_t value)
        : _value(value)
    {
    }

    bool PlayerSkillEffectWeaponClassRestriction::IsAllowed(const sox::ItemWeapon& weaponData, int32_t weaponClass) const
    {
        if (_value == 1)
        {
            return weaponData.dexBased != 0;
        }

        return _value == weaponClass;
    }

    PlayerSkillEffectAttackProbability::PlayerSkillEffectAttackProbability(int32_t value)
        : _value(value)
    {
    }

    auto PlayerSkillEffectAttackProbability::GetProbability() const -> int32_t
    {
        return _value;
    }
}
