#include "passive.h"

#include "sl/generator/game/contents/passive/effect/passive_effect_interface.h"
#include "sl/generator/service/gamedata/item/weapon_class_type.h"

namespace sunlight
{
    Passive::Passive(int32_t skillId, int32_t skillLevel)
        : _skillId(skillId)
        , _skillLevel(skillLevel)
    {
    }

    Passive::~Passive()
    {
    }

    void Passive::AddEffect(UniquePtrNotNull<IPassiveEffect> effect)
    {
        _effects.emplace_back(std::move(effect));
    }

    bool Passive::IsActive() const
    {
        return _active;
    }

    bool Passive::CanBeActivatedBy(WeaponClassType weaponClass) const
    {
        if (_checkRangeWeapon)
        {
            switch (weaponClass)
            {
            case WeaponClassType::Shuriken:
            case WeaponClassType::Pistol:
            case WeaponClassType::DoublePistol:
            case WeaponClassType::Shotgun:
            case WeaponClassType::Machinegun:
            case WeaponClassType::Bow:
            case WeaponClassType::Crossbow:
                return true;
            default:
                return false;
            }
        }

        return _weaponClassCondition ? *_weaponClassCondition == weaponClass : true;
    }

    auto Passive::GetSkillId() const -> int32_t
    {
        return _skillId;
    }

    auto Passive::GetSkillLevel() const -> int32_t
    {
        return _skillLevel;
    }

    auto Passive::GetAttackEffectProbability() const -> std::optional<int32_t>
    {
        if (_attackProbabilityCondition.has_value())
        {
            return _attackProbabilityCondition->first + _attackProbabilityCondition->second * _skillLevel;
        }

        return std::nullopt;
    }

    void Passive::SetActive(bool value)
    {
        _active = value;
    }

    void Passive::SetSkillLevel(int32_t level)
    {
        _skillLevel = level;
    }

    void Passive::SetRangeWeaponCondition(bool value)
    {
        _checkRangeWeapon = value;
    }

    void Passive::SetWeaponClassCondition(WeaponClassType weaponClass)
    {
        _weaponClassCondition = weaponClass;
    }

    void Passive::SetAttackEffectProbability(int32_t baseProbability, int32_t levelPerProbability)
    {
        _attackProbabilityCondition = { baseProbability, levelPerProbability };
    }
}
