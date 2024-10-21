#pragma once

namespace sunlight::sox
{
    struct MotionData;
}

namespace sunlight
{
    class PlayerSkill;
    struct SkillEffectData;
    struct AbilityValue;

    class GamePlayer;
    class GameMonster;
}

namespace sunlight
{
    struct PlayerNormalAttackDamageCalculateParam
    {
        const GamePlayer& player;
        const GameMonster& target;

        const sox::MotionData& motionData;
    };

    struct PlayerNormalAttackDamageCalculateResult
    {
        int32_t damage = 0;
    };

    struct PlayerSkillDamageCalculateParam
    {
        const GamePlayer& player;
        const GameMonster& target;

        const PlayerSkill& skill;
        const SkillEffectData& skillEffectData;

        int32_t chargingCount = 0;

        int32_t attackSequence = 0;
        const AbilityValue* abilityValue = nullptr;
    };

    struct PlayerSkillDamageCalculateResult
    {
        bool isDodged = false;
        int32_t damage = 0;

        int32_t damageCount = 1;
        int32_t damageInterval = 0;
    };

    class IPlayerAttackDamageCalculator
    {
    public:
        virtual ~IPlayerAttackDamageCalculator() = default;

        virtual void Calculate(PlayerNormalAttackDamageCalculateResult& result, const PlayerNormalAttackDamageCalculateParam& param) = 0;
        virtual void Calculate(PlayerSkillDamageCalculateResult& result, const PlayerSkillDamageCalculateParam& param) = 0;
    };
}
