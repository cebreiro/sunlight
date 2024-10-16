#include "entity_status_effect_system.h"

#include "sl/emulator/game/contents/status_effect/status_effect.h"
#include "sl/emulator/game/entity/game_entity.h"
#include "sl/emulator/game/message/creator/status_message_creator.h"
#include "sl/emulator/game/system/entity_view_range_system.h"
#include "sl/emulator/game/time/game_time_service.h"
#include "sl/emulator/game/zone/stage.h"

namespace sunlight
{
    EntityStatusEffectSystem::EntityStatusEffectSystem(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    void EntityStatusEffectSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<EntityViewRangeSystem>());
    }

    bool EntityStatusEffectSystem::Subscribe(Stage& stage)
    {
        return GameSystem::Subscribe(stage);
    }

    bool EntityStatusEffectSystem::ShouldUpdate() const
    {
        return GameSystem::ShouldUpdate();
    }

    void EntityStatusEffectSystem::Update()
    {
        GameSystem::Update();
    }

    auto EntityStatusEffectSystem::GetName() const -> std::string_view
    {
        return "entity_status_effect_system";
    }

    auto EntityStatusEffectSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<EntityStatusEffectSystem>();
    }

    void EntityStatusEffectSystem::AddStatusEffect(int32_t skillId, int32_t skillLevel,
        std::span<PtrNotNull<GameEntity>> targets, std::span<const SkillEffectStatusEffect> statusEffectDataList)
    {
        const game_time_point_type now = GameTimeService::Now();

        EntityViewRangeSystem& viewRangeSystem = Get<EntityViewRangeSystem>();

        for (const SkillEffectStatusEffect& statusEffectData : statusEffectDataList)
        {
            const std::chrono::milliseconds duration(statusEffectData.GetBaseDuration() + statusEffectData.GetDurationPerSkillLevel() * skillLevel);
            const StatusEffect statusEffect(skillId, statusEffectData.GetType(), statusEffectData.GetId(), now + duration, statusEffectData.GetData());

            // TODO: add status effect to StatusEffectComponent
            

            for (GameEntity& entity : targets | notnull::reference)
            {
                viewRangeSystem.Broadcast(entity, StatusMessageCreator::CreateStatusEffectAdd(entity, statusEffect), true);
            }
        }
    }
}
