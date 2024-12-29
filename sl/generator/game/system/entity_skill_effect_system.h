#pragma once
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/system/game_system.h"
#include "sl/generator/game/zone/stage_enter_type.h"
#include "sl/generator/service/gamedata/monster/monster_attack_data.h"

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

    class GameEntity;
    class GamePlayer;
    class GameMonster;
}

namespace sunlight
{
    class EntitySkillEffectSystem final : public GameSystem
    {
    public:
        EntitySkillEffectSystem(const ServiceLocator& serviceLocator, int32_t stageId);
        ~EntitySkillEffectSystem();

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        void OnStageEnter(GamePlayer& player, StageEnterType type);
        void OnStageEnter(GameMonster& monster);

    public:
        void OnSkillAdd(GamePlayer& player, PlayerSkill& skill);
        void OnSkillLevelChange(GamePlayer& player, const PlayerSkill& skill, int32_t oldLevel, int32_t newLevel);
        void OnMainWeaponChange(GamePlayer& player);

        void OnSkillUse(GamePlayer& player, const GameEntityState& state);
        void OnNormalAttackUse(GamePlayer& player, const GameEntityState& state);

    public:
        void ProcessMonsterNormalAttack(GameMonster& monster, GameEntity& target, int32_t attackIndex = 0);
        void ProcessMonsterSkill(GameMonster& monster, GameEntity& target, const MonsterAttackData::Skill& attackData, int32_t attackIndex);

    private:
        void Apply(GameEntity& entity, IPassiveEffect& passiveEffect, int32_t skillId, int32_t skillLevel, int32_t monsterPassivePercentage = -1);
        void Revert(GameEntity& entity, IPassiveEffect& passiveEffect, int32_t skillId, int32_t skillLevel);

    private:
        static auto CalculateYaw(const Eigen::Vector2f& src, const Eigen::Vector2f& dest) -> float;

    private:
        const ServiceLocator& _serviceLocator;
        int32_t _stageId = 0;

        UniquePtrNotNull<SkillTargetSelector> _skillTargetSelector;

        std::vector<std::vector<game_entity_id_type>> _attackTargetRecycleBuffer;
    };
}
