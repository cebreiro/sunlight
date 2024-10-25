#pragma once
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
    class GameEntity;
    class GameMonster;
}

namespace sunlight
{
    class MonsterSkillEffectSystem final : public GameSystem
    {
    public:
        MonsterSkillEffectSystem(const ServiceLocator& serviceLocator, int32_t stageId);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        void ProcessNormalAttack(GameMonster& monster, GameEntity& target, int32_t attackIndex = 0);

        static auto CalculateYaw(const Eigen::Vector2f& monsterPosition, const Eigen::Vector2f& mainTargetPosition) -> float;

    private:
        const ServiceLocator& _serviceLocator;
        int32_t _stageId = 0;

        std::vector<std::vector<game_entity_id_type>> _attackTargetRecycleBuffer;
    };
}
