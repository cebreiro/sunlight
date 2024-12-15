#include "entity_damage_system.h"

#include "sl/data/abf/ability_value.h"
#include "sl/generator/game/game_constant.h"
#include "sl/generator/game/component/entity_state_component.h"
#include "sl/generator/game/component/item_ownership_component.h"
#include "sl/generator/game/component/monster_aggro_component.h"
#include "sl/generator/game/component/monster_stat_component.h"
#include "sl/generator/game/component/player_party_component.h"
#include "sl/generator/game/component/player_stat_component.h"
#include "sl/generator/game/component/scene_object_component.h"
#include "sl/generator/game/contents/damage/damage_result.h"
#include "sl/generator/game/contents/damage/monster_attack_damage_calculator.h"
#include "sl/generator/game/contents/damage/player_attack_damage_calculator.h"
#include "sl/generator/game/contents/skill/player_skill.h"
#include "sl/generator/game/contents/stat/stat_value.h"
#include "sl/generator/game/contents/state/game_entity_state.h"
#include "sl/generator/game/data/sox/monster_base.h"
#include "sl/generator/game/data/sox/party_add_exp.h"
#include "sl/generator/game/entity/game_item.h"
#include "sl/generator/game/entity/game_monster.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/creator/scene_object_message_creator.h"
#include "sl/generator/game/message/creator/status_message_creator.h"
#include "sl/generator/game/system/entity_movement_system.h"
#include "sl/generator/game/system/entity_view_range_system.h"
#include "sl/generator/game/system/monster_drop_item_table_system.h"
#include "sl/generator/game/system/path_finding_system.h"
#include "sl/generator/game/system/player_index_system.h"
#include "sl/generator/game/system/player_job_system.h"
#include "sl/generator/game/system/player_quest_system.h"
#include "sl/generator/game/system/player_stat_system.h"
#include "sl/generator/game/system/scene_object_system.h"
#include "sl/generator/game/time/game_time_service.h"
#include "sl/generator/game/zone/stage.h"
#include "sl/generator/game/zone/service/game_entity_id_publisher.h"
#include "sl/generator/game/zone/service/zone_execution_service.h"
#include "sl/generator/server/packet/creator/zone_packet_s2c_creator.h"
#include "sl/generator/service/config/config_provide_service.h"
#include "sl/generator/service/gamedata/gamedata_provide_service.h"
#include "sl/generator/service/gamedata/monster/monster_data.h"
#include "sl/generator/service/gamedata/skill/monster_skill_data.h"

namespace sunlight
{
    EntityDamageSystem::EntityDamageSystem(const ServiceLocator& serviceLocator, int32_t stageId)
        : _serviceLocator(serviceLocator)
        , _stageId(stageId)
        , _gameConfig(_serviceLocator.Get<ConfigProvideService>().GetGameConfig())
        , _playerAttackDamageCalculator(std::make_unique<PlayerAttackDamageCalculator>())
        , _monsterAttackDamageCalculator(std::make_unique<MonsterAttackDamageCalculator>())
        , _mt(std::random_device{}())
    {
        for (const sox::PartyAddExp& data : _serviceLocator.Get<GameDataProvideService>().Get<sox::PartyAddExpTable>().Get())
        {
			if (data.addExpFactor <= 0.f)
			{
			    continue;
			}

            _partyExpFactors[data.index] = data.addExpFactor;
        }
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
        Add(stage.Get<PlayerStatSystem>());
        Add(stage.Get<PlayerJobSystem>());
        Add(stage.Get<PlayerQuestSystem>());
        Add(stage.Get<EntityMovementSystem>());

        if (PathFindingSystem* pathFindSystem = stage.Find<PathFindingSystem>();
            pathFindSystem)
        {
            Add(*pathFindSystem);
        }
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

    void EntityDamageSystem::KillMonster(GamePlayer& player, GameMonster& target)
    {
        target.GetStatComponent().SetHP(0);

        ProcessMonsterDead(player, target, nullptr);
    }

    void EntityDamageSystem::KillPlayer(GamePlayer& target)
    {
        if (target.IsDead())
        {
            return;
        }

        Get<EntityMovementSystem>().Remove(target.GetId());

        target.GetComponent<EntityStateComponent>().SetState(GameEntityState{
                .type = GameEntityStateType::Dying
            });

        const int32_t maxHP = target.GetStatComponent().GetFinalStat(PlayerStatType::MaxHP).As<int32_t>();

        int32_t _1 = 0;
        int32_t _2 = 0;
        Get<PlayerStatSystem>().ApplyDamage(target, maxHP, _1, _2);

        SceneObjectComponent& sceneObjectComponent = target.GetComponent<SceneObjectComponent>();
        sceneObjectComponent.SetDestPosition(sceneObjectComponent.GetPosition());
        sceneObjectComponent.SetMoving(false);

        Get<EntityViewRangeSystem>().VisitPlayer(target, [&](GamePlayer& player)
            {
                player.Defer(ZonePacketS2CCreator::CreateObjectMove(target));
                player.Defer(StatusMessageCreator::CreateHPChange(target, maxHP, 0, HPChangeFloaterType::None));
                player.Defer(SceneObjectPacketCreator::CreateState(target));

                player.FlushDeferred();
            });
    }

    void EntityDamageSystem::ProcessPlayerNormalAttack(GamePlayer& player, GameMonster& target, int32_t attackId, WeaponClassType weaponClass, const sox::MotionData& motionData)
    {
        if (target.IsDead())
        {
            return;
        }

        const PlayerNormalAttackDamageCalculateParam damageCalculateParam{
            .player = player,
            .target = target,
            .motionData = motionData,
        };
        PlayerNormalAttackDamageCalculateResult damageCalculateResult;
        _playerAttackDamageCalculator->Calculate(damageCalculateResult, damageCalculateParam);

        const DamageResult result{
            .attackerId = player.GetId(),
            .attackerType = player.GetType(),
            .damageType = damageCalculateResult.damageType,
            .id = attackId,
            .weaponClass = weaponClass,
            .damage = damageCalculateResult.damage,
            .blowType = DamageBlowType::BlowSmall,
            .attackedResultType = DamageResultType::Damage_A,
        };

        ProcessPlayerDamageResult(player, target, result.damage, &result);
    }

    void EntityDamageSystem::ProcessPlayerSkillEffect(GamePlayer& player, GameMonster& target, const PlayerSkill& skill,
        const SkillEffectData& effect, int32_t attackId, int32_t chargeCount, WeaponClassType weaponClass, const AbilityValue* abilityValue)
    {
        if (target.IsDead())
        {
            return;
        }

        const PlayerSkillDamageCalculateParam damageCalculateParam{
            .player = player,
            .target = target,
            .skill = skill,
            .skillEffectData = effect,
            .chargingCount = chargeCount,
            .attackSequence = 0
        };

        PlayerSkillDamageCalculateResult damageCalculateResult;
        _playerAttackDamageCalculator->Calculate(damageCalculateResult, damageCalculateParam);

        const DamageResult result{
            .attackerId = player.GetId(),
            .attackerType = player.GetType(),
            .damageType = damageCalculateResult.damageType,
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

        const int32_t firstDamage = result.damageCount > 1
            ? result.damage / result.damageCount + result.damage % result.damageCount
            : result.damage;

        ProcessPlayerDamageResult(player, target, firstDamage, &result);

		if (result.damageCount > 1)
		{
			const int32_t tickDamage = std::max(1, result.damage / result.damageCount);
			const int32_t tickCount = result.damageCount - 1;

            ZoneExecutionService& timerService = _serviceLocator.Get<ZoneExecutionService>();

			for (int32_t i = 0; i < tickCount; ++i)
			{
                timerService.AddTimer(std::chrono::milliseconds((1 + i) * result.damageInterval), player.GetCId(), _stageId,
					[this, tickDamage, targetId = target.GetId()](GamePlayer& player)
					{
                        GameEntity* entity = Get<SceneObjectSystem>().FindEntity(GameMonster::TYPE, targetId);
                        if (!entity || entity->GetId().GetRecycleSequence() != targetId.GetRecycleSequence())
                        {
                            return;
                        }

                        ProcessPlayerDamageResult(player, *entity->Cast<GameMonster>(), tickDamage, nullptr);
					});
			}
		}
    }

    void EntityDamageSystem::ProcessMonsterNormalAttack(GameMonster& monster, GameEntity& target)
    {
        const MonsterAttackData& attackData = monster.GetData().GetAttack();

        const MonsterNormalAttackDamageCalculateParam damageCalculateParam{
            .monster = monster,
            .target = target,
            .attackData = attackData,
        };
        MonsterNormalAttackDamageCalculateResult damageCalculateResult;
        _monsterAttackDamageCalculator->Calculate(damageCalculateResult, damageCalculateParam);

        bool blow = false;

        const DamageResult result{
            .attackerId = monster.GetId(),
            .attackerType = monster.GetType(),
            .damageType = damageCalculateResult.damageType,
            .damage = damageCalculateResult.damage,
            .damageCount = attackData.divDamage,
            .damageInterval = attackData.divDamageDelay,
            .blowType = DamageBlowType::BlowSmall,
            .attackedResultType = blow ? DamageResultType::Damage_B : DamageResultType::Damage_A,
        };

        ProcessMonsterDamageResult(target, result.damage, &result);
    }

    void EntityDamageSystem::ProcessMonsterSkillEffect(GameMonster& monster, GameEntity& target,
        const MonsterSkillData& skillData, const SkillEffectData& effect, const AbilityValue* abilityValue)
    {
        const MonsterSkillDamageCalculateParam damageCalculateParam{
            .monster = monster,
            .target = target,
            .skillData = skillData,
            .skillEffectData = effect,
        };
        MonsterSkillDamageCalculateResult damageCalculateResult;
        _monsterAttackDamageCalculator->Calculate(damageCalculateResult, damageCalculateParam);

        bool blow = false;

        const DamageResult result{
            .attackerId = monster.GetId(),
            .attackerType = monster.GetType(),
            .damageType = damageCalculateResult.damageType,
            .motionId = 3,
            .skillId = skillData.index,
            .weaponClass = WeaponClassType::Monster,
            .damage = damageCalculateResult.damage,
            .damageCount = abilityValue ? std::max(1, abilityValue->damageCount) : 1,
            .damageInterval = (abilityValue && abilityValue->damageCount > 1) ? (abilityValue->end - abilityValue->begin) / abilityValue->damageCount : 0,
            .blowType = DamageBlowType::BlowSmall,
            .attackedResultType = blow ? DamageResultType::Damage_B : DamageResultType::Damage_A,
        };

        const int32_t firstDamage = result.damageCount > 1
            ? result.damage / result.damageCount + result.damage % result.damageCount
            : result.damage;

        ProcessMonsterDamageResult(target, firstDamage, &result);

        if (result.damageCount > 1)
        {
            const int32_t tickDamage = std::max(1, result.damage / result.damageCount);
            const int32_t tickCount = result.damageCount - 1;

            ZoneExecutionService& timerService = _serviceLocator.Get<ZoneExecutionService>();

            for (int32_t i = 0; i < tickCount; ++i)
            {
                timerService.AddTimer(std::chrono::milliseconds((1 + i) * result.damageInterval),
                    [this, tickDamage, targetId = target.GetId()]()
                    {
                        GameEntity* entity = Get<SceneObjectSystem>().FindEntity(targetId);
                        if (!entity || entity->GetId().GetRecycleSequence() != targetId.GetRecycleSequence())
                        {
                            return;
                        }

                        this->ProcessMonsterDamageResult(*entity, tickDamage, nullptr);
                    });
            }
        }
    }

    void EntityDamageSystem::ProcessPlayerDamageResult(GamePlayer& player, GameMonster& target, int32_t damage, const DamageResult* result)
    {
        if (target.IsDead())
        {
            return;
        }

        if (result)
        {
            if (result->damageType == DamageType::DodgeMonster || result->damage <= 0)
            {
                Get<EntityViewRangeSystem>().Broadcast(target, StatusMessageCreator::CreateDamageResult(target, *result), false);

                return;
            }
        }

        MonsterStatComponent& monsterStatComponent = target.GetStatComponent();

        const int32_t currentHP = monsterStatComponent.GetHP().As<int32_t>();
        const int32_t newHP = std::max(0, currentHP - damage);

        monsterStatComponent.SetHP(newHP);
        target.GetAggroComponent().AddByDamage(player.GetId(), damage);

        if (newHP <= 0)
        {
            ProcessMonsterDead(player, target, result);
        }
        else
        {
            const int32_t maxHP = target.GetData().GetBase().hp;

            Get<EntityViewRangeSystem>().VisitPlayer(target, [&](GamePlayer& player)
                {
                    if (result)
                    {
                        player.Defer(StatusMessageCreator::CreateDamageResult(target, *result));
                    }

                    player.Defer(StatusMessageCreator::CreateHPChange(target, maxHP, newHP, HPChangeFloaterType::None));

                    player.FlushDeferred();
                });
        }
    }

    void EntityDamageSystem::ProcessMonsterDamageResult(GameEntity& target, int32_t damage, const DamageResult* result)
    {
        const bool blow = result ? result->attackedResultType == DamageResultType::Damage_B : false;

        int32_t maxHP = 0;
        int32_t hp = 0;

        if (result)
        {
            if (result->damageType == DamageType::DodgePlayer || result->damage <= 0)
            {
                Get<EntityViewRangeSystem>().Broadcast(target, StatusMessageCreator::CreateDamageResult(target, *result), true);

                return;
            }
        }

        if (target.GetType() == GamePlayer::TYPE)
        {
            GamePlayer& player = *target.Cast<GamePlayer>();
            if (player.IsDead())
            {
                return;
            }

            Get<PlayerStatSystem>().ApplyDamage(player, damage, maxHP, hp);

            if (hp <= 0)
            {
                ProcessPlayerDead(player);

                return;
            }
        }
        else
        {
            // TODO: pet

            assert(false);
        }

        if (blow)
        {
            Get<EntityMovementSystem>().Remove(target.GetId());

            SceneObjectComponent& sceneObjectComponent = target.GetComponent<SceneObjectComponent>();
            sceneObjectComponent.SetDestPosition(sceneObjectComponent.GetPosition());
            sceneObjectComponent.SetMoving(false);
        }

        Get<EntityViewRangeSystem>().VisitPlayer(target, [&](GamePlayer& player)
            {
                if (blow)
                {
                    player.Defer(ZonePacketS2CCreator::CreateObjectMove(target));
                }

                if (result)
                {
                    player.Defer(StatusMessageCreator::CreateDamageResult(target, *result));
                }

                player.Defer(StatusMessageCreator::CreateHPChange(target, maxHP, hp, HPChangeFloaterType::None));

                player.FlushDeferred();
            });
    }

    void EntityDamageSystem::ProcessPlayerDead(GamePlayer& player)
    {
        Get<EntityMovementSystem>().Remove(player.GetId());

        const auto newState = GameEntityState{
            .type = GameEntityStateType::Dying
        };

        player.GetComponent<EntityStateComponent>().SetState(newState);

        const int32_t maxHP = player.GetStatComponent().GetFinalStat(PlayerStatType::MaxHP).As<int32_t>();

        int32_t _1 = 0;
        int32_t _2 = 0;
        Get<PlayerStatSystem>().ApplyDamage(player, maxHP, _1, _2);

        SceneObjectComponent& sceneObjectComponent = player.GetComponent<SceneObjectComponent>();
        sceneObjectComponent.SetDestPosition(sceneObjectComponent.GetPosition());
        sceneObjectComponent.SetMoving(false);

        Get<EntityViewRangeSystem>().VisitPlayer(player, [&](GamePlayer& visited)
            {
                visited.Defer(ZonePacketS2CCreator::CreateObjectMove(player));
                visited.Defer(StatusMessageCreator::CreateHPChange(player, maxHP, 0, HPChangeFloaterType::None));
                visited.Defer(SceneObjectPacketCreator::CreateState(player, newState));

                visited.FlushDeferred();
            });
    }

    void EntityDamageSystem::ProcessMonsterDead(GamePlayer& player, GameMonster& monster, const DamageResult* damageResult)
    {
        const auto newState = GameEntityState{
            .type = GameEntityStateType::Dead,
        };

        monster.SetDead(true);
        monster.GetStateComponent().SetState(newState);

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

        _serviceLocator.Get<ZoneExecutionService>().AddTimer(std::chrono::milliseconds(1500),
            monster, _stageId, [this](const GameEntity& target)
            {
                assert(target.GetType() == GameMonster::TYPE);

                Get<SceneObjectSystem>().RemoveMonster(target.GetId());
            });

        const int32_t monsterExp = [&monster]() -> int32_t
            {
                const int32_t base = monster.GetData().GetBase().exp;

                return base;
            }();

        if (monsterExp > 0)
        {
            PlayerStatSystem& playerStatSystem = Get<PlayerStatSystem>();
            PlayerJobSystem& playerJobSystem = Get<PlayerJobSystem>();

            if (const PlayerPartyComponent& partyComponent = player.GetPartyComponent();
				partyComponent.HasParty())
            {
				_partyMemberBuffer.clear();

                PlayerIndexSystem& playerIndexSystem = Get<PlayerIndexSystem>();
                const EntityViewRangeSystem& entityViewRangeSystem = Get<EntityViewRangeSystem>();

                for (int64_t partyMemberId : partyComponent.GetMemberIds())
                {
                    GamePlayer* partyMember = playerIndexSystem.FindByCId(partyMemberId);
                    if (!partyMember)
                    {
                        continue;
                    }

					if (!entityViewRangeSystem.IsAdjacent(player, *partyMember))
                    {
                        continue;
                    }

					_partyMemberBuffer.emplace_back(partyMember);
                }

				const float partyExpFactor = GetPartyExpFactor(std::ssize(_partyMemberBuffer)).value_or(1.f);
				const int32_t expPerPartyMember = std::max(1, static_cast<int32_t>(std::round(monsterExp * partyExpFactor) / static_cast<int32_t>(std::ssize(_partyMemberBuffer))));

				for (GamePlayer& partyMember : _partyMemberBuffer | notnull::reference)
				{
					if (partyMember.IsDead())
					{
						continue;
					}

					playerStatSystem.GainCharacterExp(partyMember, expPerPartyMember);
					playerJobSystem.GainJobExp(partyMember, expPerPartyMember);
				}
            }
            else
            {
				playerStatSystem.GainCharacterExp(player, monsterExp);
				playerJobSystem.GainJobExp(player, monsterExp);
            }
        }

        Get<PlayerQuestSystem>().OnMonsterKill(player, monster.GetDataId());
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
				ownershipComponent->SetEndTimePoint(GameTimeService::Now() + _gameConfig.dropItemOwnershipSeconds);

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

            Eigen::Vector2f spawnPos;
            if (PathFindingSystem* pathFindingSystem = Find<PathFindingSystem>();
                !pathFindingSystem || !pathFindingSystem->GetRandPositionInCircle(spawnPos, monsterPos, GameConstant::MONSTER_DROP_ITEM_RADIUS))
            {
                spawnPos = monsterPos;

                spawnPos.x() += dist(_mt);
                spawnPos.x() += dist(_mt);
            }

			sceneObjectSystem.SpawnItem(std::move(item), monsterPos, spawnPos, _gameConfig.dropItemKeepSeconds);
		}
    }

    auto EntityDamageSystem::GetPartyExpFactor(int64_t partyMemberCount) const -> std::optional<float>
    {
        const auto iter = _partyExpFactors.find(partyMemberCount);

		return iter != _partyExpFactors.end() ? iter->second : std::optional<float>();
    }
}
