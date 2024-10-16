#pragma once
#include "sl/emulator/game/contents/stat/player_stat_type.h"
#include "sl/emulator/service/gamedata/skill/skill_effect_data.h"
#include "sl/emulator/service/gamedata/skill/status_effect_type.h"

namespace sunlight::sox
{
    struct ItemWeapon;
}

namespace sunlight
{
    class PlayerSkillEffectDamage
    {
    public:
        PlayerSkillEffectDamage() = default;
        explicit PlayerSkillEffectDamage(const SkillEffectData& skillEffectData);

        bool IsOneShotKillEffect() const;

        auto GetData() const -> const SkillEffectData&;
        auto GetType() const -> int32_t;
        auto GetBaseDamage() const -> int32_t;
        auto GetDamageRandMin() const -> int32_t;
        auto GetDamageRandMax() const -> int32_t;

    private:
        PtrNotNull<const SkillEffectData> _data = nullptr;

        int32_t _type = 0;
        int32_t _baseDamage = 0; // value2
        int32_t _randMin = 0; // value3
        int32_t _randMax = 0; // value4
    };

    class SkillEffectStatusEffect
    {
    public:
        SkillEffectStatusEffect() = default;
        explicit SkillEffectStatusEffect(const SkillEffectData& skillEffectData);

        auto GetData() const -> const SkillEffectData&;
        auto GetType() const -> StatusEffectType;
        auto GetDurationPerSkillLevel() const -> int32_t;
        auto GetBaseDuration() const -> int32_t;
        auto GetId() const -> int32_t;

    private:
        PtrNotNull<const SkillEffectData> _data = nullptr;

        StatusEffectType _type = {}; // type
        int32_t _durationPerSkillLevel = 0; // value3
        int32_t _baseDuration = 0; // value4
        int32_t _id = 0; // value5
    };

    class PlayerSkillEffectPassiveStat
    {
    public:
        PlayerSkillEffectPassiveStat() = default;
        PlayerSkillEffectPassiveStat(PlayerStatType statType, int32_t value);

        auto GetStatType() const -> PlayerStatType;
        auto GetValue() const -> int32_t;

    private:
        PlayerStatType _statType = {};
        int32_t _value = 0;
    };

    class PlayerSkillEffectWeaponClassRestriction
    {
    public:
        explicit PlayerSkillEffectWeaponClassRestriction(int32_t value);

        bool IsAllowed(const sox::ItemWeapon& weaponData, int32_t weaponClass) const;

    private:
        int32_t _value;
    };

    class PlayerSkillEffectAttackProbability
    {
    public:
        explicit PlayerSkillEffectAttackProbability(int32_t value);

        // 0~100
        auto GetProbability() const -> int32_t;

    private:
        int32_t _value = 0;
    };
}
