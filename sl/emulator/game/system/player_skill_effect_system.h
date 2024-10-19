#pragma once
#include "sl/emulator/game/system/game_system.h"
#include "sl/emulator/game/zone/stage_enter_type.h"

namespace sunlight
{
    struct GameEntityState;
    class IPassiveEffect;
    class PlayerSkill;
    class PlayerSkillTargetSelector;

    class GamePlayer;
}

namespace sunlight
{
    class PlayerSkillEffectSystem final : public GameSystem
    {
    public:
        explicit PlayerSkillEffectSystem(const ServiceLocator& serviceLocator);
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

    private:
        void Apply(GamePlayer& player, IPassiveEffect& passiveEffect, int32_t skillLevel) const;
        void Revert(GamePlayer& player, IPassiveEffect& passiveEffect, int32_t skillLevel) const;

    private:
        const ServiceLocator& _serviceLocator;

        UniquePtrNotNull<PlayerSkillTargetSelector> _skillTargetSelector;
    };
}
