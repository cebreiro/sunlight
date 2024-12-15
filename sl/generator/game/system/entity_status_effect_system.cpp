#include "entity_status_effect_system.h"

#include "sl/generator/game/component/entity_status_effect_component.h"
#include "sl/generator/game/contents/status_effect/status_effect.h"
#include "sl/generator/game/contents/status_effect/status_effect_handler_register.h"
#include "sl/generator/game/entity/game_entity.h"
#include "sl/generator/game/entity/game_monster.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/creator/status_message_creator.h"
#include "sl/generator/game/system/entity_view_range_system.h"
#include "sl/generator/game/system/event_bubbling_system.h"
#include "sl/generator/game/system/player_stat_system.h"
#include "sl/generator/game/system/scene_object_system.h"
#include "sl/generator/game/system/event_bubbling/monster_event_bubbling.h"
#include "sl/generator/game/time/game_time_service.h"
#include "sl/generator/game/zone/stage.h"
#include "sl/generator/game/zone/service/zone_execution_service.h"
#include "sl/generator/service/gamedata/skill/skill_effect_data.h"

namespace sunlight
{
    EntityStatusEffectSystem::EntityStatusEffectSystem(const ServiceLocator& serviceLocator, int32_t stageId)
        : _serviceLocator(serviceLocator)
        , _stageId(stageId)
    {
        PlayerSkillEffectStatusEffectHandlerRegister::Configure(*this);
    }

    void EntityStatusEffectSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<EntityViewRangeSystem>());
        Add(stage.Get<PlayerStatSystem>());
        Add(stage.Get<SceneObjectSystem>());
    }

    bool EntityStatusEffectSystem::Subscribe(Stage& stage)
    {
        stage.Get<EventBubblingSystem>().AddSubscriber<EventBubblingMonsterDespawn>(
            [this](const EventBubblingMonsterDespawn& event)
            {
                OnStageExit(*event.monster);
            });

        return true;
    }

    bool EntityStatusEffectSystem::ShouldUpdate() const
    {
        return true;
    }

    void EntityStatusEffectSystem::Update()
    {
        if (!_tickStatusEffects.empty())
        {
            const game_time_point_type now = GameTimeService::Now();

            for (const auto& [entity, statusEffects] : _tickStatusEffects)
            {
                for (StatusEffect& statusEffect : statusEffects | notnull::reference)
                {
                    if (now < statusEffect.GetNextTickTimePoint())
                    {
                        continue;
                    }

                    const IStatusEffectTickHandler* tickHandler = GetTickHandler(statusEffect.GetType());
                    assert(tickHandler);

                    tickHandler->Tick(*this, *entity, statusEffect);
                }
            }
        }
    }

    auto EntityStatusEffectSystem::GetName() const -> std::string_view
    {
        return "entity_status_effect_system";
    }

    auto EntityStatusEffectSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<EntityStatusEffectSystem>();
    }

    void EntityStatusEffectSystem::OnStageEnter(GamePlayer& player, StageEnterType enterType)
    {
        EntityStatusEffectComponent& statusEffectComponent = player.GetStatusEffectComponent();

        const game_time_point_type now = GameTimeService::Now();

        for (StatusEffect& statusEffect : statusEffectComponent.GetRange())
        {
            if (enterType == StageEnterType::Login)
            {
                if (const IStatusEffectApplyHandler* applyHandler = GetApplyHandler(statusEffect.GetType());
                    applyHandler)
                {
                    applyHandler->Apply(*this, player, statusEffect);
                }
            }

            AddStatusEffectRemoveTimer(player, statusEffect);

            if (GetTickHandler(statusEffect.GetType()))
            {
                statusEffect.SetLastTickTimePoint(now);
                statusEffect.SetNextTickTimePoint(now);

                _tickStatusEffects[&player].push_back(&statusEffect);
            }
        }
    }

    void EntityStatusEffectSystem::OnStageExit(GamePlayer& player, StageExitType exitType)
    {
        (void)player;
        (void)exitType;

        switch (exitType)
        {
        case StageExitType::Logout:
        case StageExitType::ZoneChange:
        {
            // TODO: save to db
        }
        break;
        case StageExitType::StageChange:
        default:;
        }

        _tickStatusEffects.erase(&player);
    }

    void EntityStatusEffectSystem::OnStageExit(GameMonster& monster)
    {
        _tickStatusEffects.erase(&monster);
    }

    void EntityStatusEffectSystem::AddStatusEffectBySkill(int32_t skillId, int32_t skillLevel,
        std::span<game_entity_id_type> targets, const SkillEffectData& skillEffectData)
    {
        if (skillEffectData.category != SkillEffectCategory::StatusEffect)
        {
            assert(false);

            return;
        }

        const game_time_point_type now = GameTimeService::Now();

        EntityViewRangeSystem& viewRangeSystem = Get<EntityViewRangeSystem>();

        const int32_t durationMilli = skillEffectData.value4 + skillEffectData.value3 * skillLevel;
        StatusEffect statusEffect(skillId, skillLevel, skillEffectData, now, now + std::chrono::milliseconds(durationMilli));

        const IStatusEffectApplyHandler* applyHandler = GetApplyHandler(statusEffect.GetType());
        const IStatusEffectTickHandler* tickHandler = GetTickHandler(statusEffect.GetType());

        SceneObjectSystem& sceneObjectSystem = Get<SceneObjectSystem>();

        for (const game_entity_id_type targetId : targets)
        {
            GameEntity* entity = sceneObjectSystem.FindEntity(targetId);
            if (!entity)
            {
                continue;
            }

            EntityStatusEffectComponent& statusEffectComponent = entity->GetComponent<EntityStatusEffectComponent>();
            std::optional<StatusEffect> prevStatusEffect = std::nullopt;

            if (StatusEffect* prevStatusEffectPtr = statusEffectComponent.Find(statusEffect.GetId());
                prevStatusEffectPtr)
            {
                ClearStatusEffect(*entity, *prevStatusEffectPtr);

                prevStatusEffect = statusEffectComponent.Release(statusEffect.GetId());
            }

            if (applyHandler)
            {
                applyHandler->Apply(*this, *entity, statusEffect);
            }

            viewRangeSystem.VisitPlayer(*entity, [&](GamePlayer& player)
            {
                if (prevStatusEffect.has_value())
                {
                    player.Defer(StatusMessageCreator::CreateStatusEffectRemove(*entity, *prevStatusEffect));
                }

                player.Defer(StatusMessageCreator::CreateStatusEffectAdd(*entity, statusEffect));
                player.FlushDeferred();
            });

            if (durationMilli > 0)
            {
                StatusEffect* addStatusEffectPtr = statusEffectComponent.Add(statusEffect);
                assert(addStatusEffectPtr);

                if (tickHandler)
                {
                    _tickStatusEffects[entity].push_back(addStatusEffectPtr);
                }

                AddStatusEffectRemoveTimer(*entity, *addStatusEffectPtr);
            }
        }
    }

    void EntityStatusEffectSystem::AddStatusEffectByPassive(int32_t skillId, int32_t skillLevel, GameEntity& target, const SkillEffectData& skillEffectData)
    {
        const game_time_point_type now = GameTimeService::Now();
        StatusEffect statusEffect(skillId, skillLevel, skillEffectData, now, game_time_point_type::max());

        EntityStatusEffectComponent& statusEffectComponent = target.GetComponent<EntityStatusEffectComponent>();
        std::optional<StatusEffect> prevStatusEffect = std::nullopt;

        if (StatusEffect* prevStatusEffectPtr = statusEffectComponent.Find(statusEffect.GetId());
            prevStatusEffectPtr)
        {
            ClearStatusEffect(target, *prevStatusEffectPtr);

            prevStatusEffect = statusEffectComponent.Release(statusEffect.GetId());
        }

        StatusEffect* addStatusEffectPtr = statusEffectComponent.Add(statusEffect);
        assert(addStatusEffectPtr);

        if (const IStatusEffectApplyHandler* applyHandler = GetApplyHandler(statusEffect.GetType());
            applyHandler)
        {
            applyHandler->Apply(*this, target, statusEffect);
        }

        if (const IStatusEffectTickHandler* tickHandler = GetTickHandler(statusEffect.GetType());
            tickHandler)
        {
            _tickStatusEffects[&target].push_back(addStatusEffectPtr);
        }

        Get<EntityViewRangeSystem>().VisitPlayer(target, [&](GamePlayer& player)
            {
                if (prevStatusEffect.has_value())
                {
                    player.Defer(StatusMessageCreator::CreateStatusEffectRemove(target, *prevStatusEffect));
                }

                player.Defer(StatusMessageCreator::CreateStatusEffectAdd(target, statusEffect));
                player.FlushDeferred();
            });
    }

    bool EntityStatusEffectSystem::RemoveStatusEffect(GameEntity& entity, int32_t statusEffectId)
    {
        EntityStatusEffectComponent& statusEffectComponent = entity.GetComponent<EntityStatusEffectComponent>();
        StatusEffect* statusEffect = statusEffectComponent.Find(statusEffectId);

        if (!statusEffect)
        {
            return false;
        }

        ClearStatusEffect(entity, *statusEffect);

        Get<EntityViewRangeSystem>().Broadcast(entity, StatusMessageCreator::CreateStatusEffectRemove(entity, *statusEffect), true);

        statusEffectComponent.Remove(statusEffectId);

        return true;
    }

    void EntityStatusEffectSystem::ClearStatusEffect(GameEntity& entity, StatusEffect& statusEffect)
    {
        if (const IStatusEffectTickHandler* tickHandler = GetTickHandler(statusEffect.GetType());
            tickHandler)
        {
            const auto iter1 = _tickStatusEffects.find(&entity);
            assert(iter1 != _tickStatusEffects.end());

            std::vector<StatusEffect*>& statusEffects = iter1->second;
            const auto iter2 = std::ranges::find_if(statusEffects, [id = statusEffect.GetId()](const StatusEffect* statusEffect)
                {
                    return statusEffect->GetId() == id;
                });
            assert(iter2 != statusEffects.end());

            statusEffects.erase(iter2);

            if (statusEffects.empty())
            {
                _tickStatusEffects.erase(iter1);
            }
        }

        if (const IStatusEffectRevertHandler* revertHandler = GetRevertHandler(statusEffect.GetType());
            revertHandler)
        {
            revertHandler->Revert(*this, entity, statusEffect);
        }
    }

    void EntityStatusEffectSystem::AddStatusEffectRemoveTimer(const GameEntity& entity, const StatusEffect& statusEffect)
    {
        const game_time_point_type now = GameTimeService::Now();

        const std::chrono::milliseconds delay = statusEffect.GetEndTimePoint() > now ?
            std::chrono::duration_cast<std::chrono::milliseconds>(statusEffect.GetEndTimePoint() - now) :
            std::chrono::milliseconds(0);

        _serviceLocator.Get<ZoneExecutionService>().AddTimer(delay, entity, _stageId,
            [this, id = statusEffect.GetId(), endTimePoint = statusEffect.GetEndTimePoint()](GameEntity& entity)
            {
                const EntityStatusEffectComponent& statusEffectComponent = entity.GetComponent<EntityStatusEffectComponent>();
                const StatusEffect* statusEffect = statusEffectComponent.Find(id);

                if (!statusEffect || statusEffect->GetEndTimePoint() != endTimePoint)
                {
                    return;
                }

                this->RemoveStatusEffect(entity, statusEffect->GetId());
            });
    }

    auto EntityStatusEffectSystem::GetApplyHandler(StatusEffectType type) const -> const IStatusEffectApplyHandler*
    {
        const auto iter = _applyHandlers.find(type);

        return iter != _applyHandlers.end() ? iter->second : nullptr;
    }

    auto EntityStatusEffectSystem::GetRevertHandler(StatusEffectType type) const -> const IStatusEffectRevertHandler*
    {
        const auto iter = _revertHandlers.find(type);

        return iter != _revertHandlers.end() ? iter->second : nullptr;
    }

    auto EntityStatusEffectSystem::GetTickHandler(StatusEffectType type) const -> const IStatusEffectTickHandler*
    {
        const auto iter = _tickHandlers.find(type);

        return iter != _tickHandlers.end() ? iter->second : nullptr;
    }
}
