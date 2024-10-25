#pragma once
#include "sl/emulator/game/system/game_system.h"
#include "sl/emulator/game/zone/stage_enter_type.h"

namespace sunlight::sox
{
    struct MotionData;
}

namespace sunlight
{
    struct GameEntityState;
    class IPassiveEffect;
    class PlayerSkill;
    class SkillTargetSelector;

    class GamePlayer;
    class GameMonster;
}

namespace sunlight
{
    class PlayerSkillEffectSystem final : public GameSystem
    {
    public:
        PlayerSkillEffectSystem(const ServiceLocator& serviceLocator, int32_t stageId);
        ~PlayerSkillEffectSystem();

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        void OnStageEnter(GamePlayer& player, StageEnterType type);

    public:
        void OnSkillAdd(GamePlayer& player, PlayerSkill& skill);
        void OnSkillLevelChange(GamePlayer& player, const PlayerSkill& skill, int32_t oldLevel, int32_t newLevel);
        void OnMainWeaponChange(GamePlayer& player);

        void OnSkillUse(GamePlayer& player, const GameEntityState& state);
        void OnNormalAttackUse(GamePlayer& player, const GameEntityState& state);

    private:
        void ProcessNormalAttack(GamePlayer& player, GameMonster& monster, int32_t attackId, const sox::MotionData& motionData);

        void Apply(GamePlayer& player, IPassiveEffect& passiveEffect, int32_t skillLevel) const;
        void Revert(GamePlayer& player, IPassiveEffect& passiveEffect, int32_t skillLevel) const;

    private:
        const ServiceLocator& _serviceLocator;
        int32_t _stageId = 0;

        UniquePtrNotNull<SkillTargetSelector> _skillTargetSelector;
    };
}
