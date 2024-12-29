#pragma once
#include <boost/container/small_vector.hpp>
#include "sl/generator/service/gamedata/item/weapon_class_type.h"

namespace sunlight
{
    class IPassiveEffect;
}

namespace sunlight
{
    class Passive
    {
    public:
        Passive(const Passive& other) = delete;
        Passive& operator=(const Passive& other) = delete;

    public:
        Passive() = default;
        Passive(Passive&& other) noexcept = default;
        Passive& operator=(Passive&& other) noexcept = default;

    public:
        Passive(int32_t skillId, int32_t skillLevel);
        ~Passive();

        void AddEffect(UniquePtrNotNull<IPassiveEffect> effect);
        inline auto GetEffectRange() const;

    public:
        bool IsActive() const;
        bool CanBeActivatedBy(WeaponClassType weaponClass) const;

        auto GetSkillId() const -> int32_t;
        auto GetSkillLevel() const -> int32_t;
        auto GetAttackEffectProbability() const -> std::optional<int32_t>;

        void SetActive(bool value);
        void SetSkillLevel(int32_t level);
        void SetRangeWeaponCondition(bool value);
        void SetWeaponClassCondition(WeaponClassType weaponClass);
        void SetAttackEffectProbability(int32_t baseProbability, int32_t levelPerProbability);

    private:
        int32_t _skillId = 0;
        int32_t _skillLevel = 0;

        bool _active = false;
        bool _checkRangeWeapon = false;
        std::optional<WeaponClassType> _weaponClassCondition = std::nullopt;
        std::optional<std::pair<int32_t, int32_t>> _attackProbabilityCondition = std::nullopt;

        boost::container::small_vector<UniquePtrNotNull<IPassiveEffect>, 4> _effects;
    };

    auto Passive::GetEffectRange() const
    {
        return _effects | notnull::reference;
    }
}
