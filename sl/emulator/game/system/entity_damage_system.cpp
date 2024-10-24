#include "entity_damage_system.h"

#include "sl/data/abf/ability_value.h"
#include "sl/emulator/game/game_constant.h"
#include "sl/emulator/game/component/entity_state_component.h"
#include "sl/emulator/game/component/item_ownership_component.h"
#include "sl/emulator/game/component/monster_stat_component.h"
#include "sl/emulator/game/component/player_party_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/contents/damage/damage_result.h"
#include "sl/emulator/game/contents/damage/player_attack_damage_calculator.h"
#include "sl/emulator/game/contents/skill/player_skill.h"
#include "sl/emulator/game/contents/stat/stat_value.h"
#include "sl/emulator/game/contents/state/game_entity_state.h"
#include "sl/emulator/game/data/sox/monster_base.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/entity/game_monster.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/creator/scene_object_message_creator.h"
#include "sl/emulator/game/message/creator/status_message_creator.h"
#include "sl/emulator/game/system/entity_view_range_system.h"
#include "sl/emulator/game/system/monster_drop_item_table_system.h"
#include "sl/emulator/game/system/player_index_system.h"
#include "sl/emulator/game/system/scene_object_system.h"
#include "sl/emulator/game/time/game_time_service.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/game/zone/service/game_entity_id_publisher.h"
#include "sl/emulator/game/zone/service/zone_timer_service.h"
#include "sl/emulator/service/gamedata/skill/skill_effect_data.h"

namespace sunlight
{
    EntityDamageSystem::EntityDamageSystem(const ServiceLocator& serviceLocator, int32_t stageId)
        : _serviceLocator(serviceLocator)
        , _stageId(stageId)
        , _damageCalculator(std::make_unique<PlayerAttackDamageCalculator>())
        , _mt(std::random_device{}())
    {
    }

    EntityDamageSystem::~EntityDamageSystem()
    {
    }

    void EntityDamageSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<EntityViewRangeSystem>());
        Add(stage.Get<PlayerIndexSystem>());
        Add(stage.Get<SceneObjectSystem>());
        Add(stage.Get<MonsterDropItemTableSystem>());
    }

    bool EntityDamageSystem::Subscribe(Stage& stage)
    {
        return GameSystem::Subscribe(stage);
    }

    auto EntityDamageSystem::GetName() const -> std::string_view
    {
        return "entity_damage_system";
    }

    auto EntityDamageSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<EntityDamageSystem>();
    }

    void EntityDamageSystem::KillMonster(GamePlayer& player, game_entity_id_type mobId)
    {
        const auto& entity = Get<SceneObjectSystem>().FindEntity(GameMonster::TYPE, mobId);
        if (!entity)
        {
            return;
        }

        GameMonster& monster = *entity->Cast<GameMonster>();
        monster.GetStatComponent().SetHP(0);

        ProcessMonsterDead(player, monster, nullptr);
    }

    void EntityDamageSystem::ProcessPlayerSkillEffect(GamePlayer& player, GameMonster& target, const PlayerSkill& skill,
        const SkillEffectData& effect, int32_t attackId, int32_t chargeCount, WeaponClassType weaponClass, const AbilityValue* abilityValue)
    {
        MonsterStatComponent& monsterStatComponent = target.GetStatComponent();
        if (monsterStatComponent.IsDead())
        {
            return;
        }

        const PlayerSkillDamageCalculateParam damageCalculateParam{
            .player = player,
            .target = target,
            .skill = skill,
            .skillEffectData = effect,
            .chargingCount = chargeCount,
            .attackSequence = 0,
            .abilityValue = abilityValue
        };

        PlayerSkillDamageCalculateResult damageCalculateResult;
        _damageCalculator->Calculate(damageCalculateResult, damageCalculateParam);

        const DamageResult result{
            .attackerId = player.GetId(),
            .attackerType = player.GetType(),
            .damageType = damageCalculateResult.isDodged ? DamageType::DodgeMonster : DamageType::DamageMonster,
            .id = attackId,
            .motionId = 3,
            .skillId = skill.GetId(),
            .weaponClass = weaponClass,
            .damage = damageCalculateResult.damage,
            .damageCount = abilityValue ? std::max(1, abilityValue->damageCount) : 1,
            .damageInterval = (abilityValue && abilityValue->damageCount > 1) ? (abilityValue->end - abilityValue->begin) / abilityValue->damageCount : 0,
            .blowType = DamageBlowType::BlowSmall,
            .attackedResultType = DamageResultType::Damage_A,
        };

        if (damageCalculateResult.isDodged)
        {
            Get<EntityViewRangeSystem>().VisitPlayer(target, [&target, &result](GamePlayer& player)
                {
                    player.Send(StatusMessageCreator::CreateDamageResult(target, result));
                });

            return;
        }

        const int32_t firstDamage = result.damageCount > 1
            ? result.damage / result.damageCount + result.damage % result.damageCount
            : result.damage;

        const StatValue currentHP(monsterStatComponent.GetHP());

        const int32_t newHP = std::max(0, currentHP.As<int32_t>() - firstDamage);
        const int32_t maxHP = monsterStatComponent.GetData().hp;

        monsterStatComponent.SetHP(newHP);
		const bool dead = newHP <= 0;

        if (dead)
        {
            ProcessMonsterDead(player, target, &result);

            return;
        }

        Get<EntityViewRangeSystem>().VisitPlayer(target, [&](GamePlayer& player)
            {
                player.Defer(StatusMessageCreator::CreateDamageResult(target, result));
                player.Defer(StatusMessageCreator::CreateHPChange(target, maxHP, newHP, HPChangeFloaterType::None));

                player.FlushDeferred();
            });

		if (result.damageCount > 1)
		{
			const int32_t tickDamage = result.damage / result.damageCount;
			const int32_t tickCount = result.damageCount - 1;

            ZoneTimerService& timerService = _serviceLocator.Get<ZoneTimerService>();

			for (int32_t i = 0; i < tickCount; ++i)
			{
                timerService.AddTimer(std::chrono::milliseconds((1 + i) * result.damageInterval),
					[this, tickDamage, playerId = player.GetCId(), targetId = target.GetId()]()
					{
						this->OnDelayDamage(playerId, targetId, tickDamage);
					});
			}
		}
    }

    void EntityDamageSystem::OnDelayDamage(int64_t playerId, game_entity_id_type targetMonsterId, int32_t damage)
    {
        const GamePlayer* player = Get<PlayerIndexSystem>().FindByCId(playerId);
        if (!player)
        {
            return;
        }

        const auto& targetEntity = Get<SceneObjectSystem>().FindEntity(GameMonster::TYPE, targetMonsterId);
        if (!targetEntity || targetEntity->GetId().GetRecycleSequence() != targetMonsterId.GetRecycleSequence())
        {
            return;
        }

        if (damage <= 0)
        {
            return;
        }

        GameMonster& target = *targetEntity->Cast<GameMonster>();

        MonsterStatComponent& monsterStatComponent = target.GetStatComponent();
        if (monsterStatComponent.IsDead())
        {
            return;
        }

		const int32_t maxHP = monsterStatComponent.GetData().hp;
        const StatValue currentHP(monsterStatComponent.GetHP());

        const int32_t newHP = std::max(0, currentHP.As<int32_t>() - damage);
		monsterStatComponent.SetHP(newHP);

        if (newHP <= 0)
        {
            ProcessMonsterDead(*player, target, nullptr);
        }
        else
        {
            Get<EntityViewRangeSystem>().VisitPlayer(target, [&](GamePlayer& player)
                {
                    player.Send(StatusMessageCreator::CreateHPChange(target, maxHP, newHP, HPChangeFloaterType::None));
                });
        }
    }

    void EntityDamageSystem::ProcessMonsterDead(const GamePlayer& player, GameMonster& monster, const DamageResult* damageResult)
    {
        const auto newState = GameEntityState{
            .type = GameEntityStateType::Dead,
        };

        monster.GetStateComponent().SetState(newState);
        monster.GetStatComponent().SetDead(true);

        DropMonsterItem(monster, &player);

        Get<EntityViewRangeSystem>().VisitPlayer(monster, [&](GamePlayer& player)
            {
                if (damageResult)
                {
                    player.Defer(StatusMessageCreator::CreateDamageResult(monster, *damageResult));
                }

                player.Defer(SceneObjectPacketCreator::CreateState(monster, newState));
                player.FlushDeferred();
            });

        _serviceLocator.Get<ZoneTimerService>().AddTimer(std::chrono::milliseconds(1500),
            monster, _stageId, [this](const GameEntity& target)
            {
                assert(target.GetType() == GameMonster::TYPE);

                Get<SceneObjectSystem>().RemoveMonster(target.GetId());
            });
    }

    void EntityDamageSystem::DropMonsterItem(const GameMonster& monster, const GamePlayer* player)
    {
		_dropItemQueryResult.clear();
		Get<MonsterDropItemTableSystem>().GetMonsterDropItem(_dropItemQueryResult, monster);

		if (_dropItemQueryResult.empty())
		{
			return;
		}

        GameEntityIdPublisher& entityIdPublisher = _serviceLocator.Get<GameEntityIdPublisher>();
        SceneObjectSystem& sceneObjectSystem = Get<SceneObjectSystem>();

		const Eigen::Vector2f monsterPos = monster.GetSceneObjectComponent().GetPosition();

		std::uniform_real_distribution<float> dist(-15.f, 15.f);

		for (const auto& [itemPtr, quantity] : _dropItemQueryResult)
		{
			auto item = std::make_shared<GameItem>(entityIdPublisher, *itemPtr, quantity);

			if (player)
			{
				auto ownershipComponent = std::make_unique<ItemOwnershipComponent>();
				ownershipComponent->SetEndTimePoint(GameTimeService::Now() + GameConstant::DROP_ITEM_OWNERSHIP_DURATION);

				if (player->GetPartyComponent().HasParty())
				{
				    for (int64_t memberId : player->GetPartyComponent().GetMemberIds())
				    {
						ownershipComponent->Add(memberId);
				    }
				}
				else
				{
					ownershipComponent->Add(player->GetCId());
				}

				item->AddComponent(std::move(ownershipComponent));
			}

			// TODO: query movable area, spawn on that
			Eigen::Vector2f spawnPos = monsterPos;
			spawnPos.x() += dist(_mt);
			spawnPos.x() += dist(_mt);

			sceneObjectSystem.SpawnItem(std::move(item), monsterPos, spawnPos);
		}
    }
}
