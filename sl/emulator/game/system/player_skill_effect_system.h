#pragma once
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/entity/game_entity_type.h"
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
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
        void OnSkillUse(GamePlayer& player, int32_t skillId, game_entity_id_type targetId, GameEntityType targetType, int32_t chargeTime);

    private:
        const ServiceLocator& _serviceLocator;

        UniquePtrNotNull<PlayerSkillTargetSelector> _skillTargetSelector;
    };
}
