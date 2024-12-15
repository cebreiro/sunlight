#include "game_script_system.h"

#include "sl/generator/game/component/player_npc_script_component.h"
#include "sl/generator/game/component/player_quest_component.h"
#include "sl/generator/game/entity/game_npc.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/creator/npc_message_creator.h"
#include "sl/generator/game/script/lua_script_engine.h"
#include "sl/generator/game/script/class/lua_npc.h"
#include "sl/generator/game/script/class/lua_player.h"
#include "sl/generator/game/script/class/lua_system.h"
#include "sl/generator/game/system/entity_view_range_system.h"
#include "sl/generator/game/system/event_bubbling_system.h"
#include "sl/generator/game/system/item_archive_system.h"
#include "sl/generator/game/system/npc_shop_system.h"
#include "sl/generator/game/system/player_job_system.h"
#include "sl/generator/game/system/player_quest_system.h"
#include "sl/generator/game/system/player_stat_system.h"
#include "sl/generator/game/system/scene_object_system.h"
#include "sl/generator/game/system/event_bubbling/player_event_bubbling.h"
#include "sl/generator/game/zone/stage.h"
#include "sl/generator/game/zone/service/zone_change_service.h"
#include "sl/generator/game/zone/service/zone_execution_service.h"

namespace sunlight
{
    GameScriptSystem::GameScriptSystem(const ServiceLocator& serviceLocator, int32_t stageId)
        : _serviceLocator(serviceLocator)
        , _stageId(stageId)
    {
    }

    void GameScriptSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<PlayerJobSystem>());
        Add(stage.Get<PlayerStatSystem>());
        Add(stage.Get<ItemArchiveSystem>());
        Add(stage.Get<NPCShopSystem>());
        Add(stage.Get<PlayerQuestSystem>());
        Add(stage.Get<EntityViewRangeSystem>());
        Add(stage.Get<SceneObjectSystem>());
    }

    bool GameScriptSystem::Subscribe(Stage& stage)
    {
        stage.Get<EventBubblingSystem>().AddSubscriber<EventBubblingPlayerQuestMonsterKill>(
            [this](const EventBubblingPlayerQuestMonsterKill& param)
            {
                _serviceLocator.Get<ZoneExecutionService>().Post(param.player->GetCId(), _stageId,
                    [this, questId = param.questId, monsterId = param.monsterId](GamePlayer& player)
                    {
                        this->StartQuestScriptMonsterKill(player, questId, monsterId);
                    });
            });

        return true;
    }

    auto GameScriptSystem::GetName() const -> std::string_view
    {
        return "game_script_system";
    }

    auto GameScriptSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<GameScriptSystem>();
    }

    auto GameScriptSystem::GetServiceLocator() const -> const ServiceLocator&
    {
        return _serviceLocator;
    }

    bool GameScriptSystem::StartNPCScript(GamePlayer& player, GameNPC& npc)
    {
        PlayerNPCScriptComponent& scriptComponent = player.GetNPCScriptComponent();
        if (scriptComponent.HasTargetNPC())
        {
            SUNLIGHT_LOG_WARN(_serviceLocator,
                fmt::format("[{}] player has already talk npc. player: {}, prev_npc: {}, new_npc: {}",
                    GetName(), player.GetCId(), scriptComponent.GetTargetNPCId(), npc.GetId()));

            return false;
        }

        scriptComponent.SetTargetNPCId(npc.GetId());

        LuaSystem luaSystem(*this);
        LuaNPC luaNPC(npc);
        LuaPlayer luaPlayer(*this, player);

        if (!_serviceLocator.Get<LuaScriptEngine>().ExecuteNPCScript(luaNPC.GetId(), luaSystem, luaNPC, luaPlayer, 0))
        {
            scriptComponent.Clear();

            return false;
        }

        return true;
    }

    void GameScriptSystem::ContinueNPCScript(GamePlayer& player, int32_t selection)
    {
        // selection == 0 -> exit state

        PlayerNPCScriptComponent& scriptComponent = player.GetNPCScriptComponent();
        if (!scriptComponent.HasTargetNPC())
        {
            // npc script is disposed
            return;
        }

        do
        {
            GameEntity* entity = Get<SceneObjectSystem>().FindEntity(GameEntityType::NPC, scriptComponent.GetTargetNPCId());
            if (!entity)
            {
                break;
            }

            GameNPC* npc = entity->Cast<GameNPC>();
            if (!npc)
            {
                assert(false);

                break;
            }

            scriptComponent.SetSequence(scriptComponent.GetSequence() + 1);
            scriptComponent.SetSelection(selection);

            LuaSystem luaSystem(*this);
            LuaNPC luaNPC(*npc);
            LuaPlayer luaPlayer(*this, player);

            if (!_serviceLocator.Get<LuaScriptEngine>().ExecuteNPCScript(luaNPC.GetId(), luaSystem, luaNPC, luaPlayer, scriptComponent.GetSequence()))
            {
                scriptComponent.Clear();

                break;
            }

            return;

        } while (false);

        SUNLIGHT_LOG_WARN(_serviceLocator,
            fmt::format("[{}] fail to execute script. player: {}",
                GetName(), player.GetCId()));

        DisposeTalk(player);
    }

    void GameScriptSystem::StartQuestScriptMonsterKill(GamePlayer& player, int32_t questId, int32_t monsterId)
    {
        Quest* quest = player.GetQuestComponent().FindQuest(questId);
        if (!quest || quest->GetState() != 0)
        {
            return;
        }

        LuaSystem luaSystem(*this);
        LuaPlayer luaPlayer(*this, player);

        _serviceLocator.Get<LuaScriptEngine>().ExecuteQuestScriptMonsterKill(luaSystem, luaPlayer, *quest, monsterId);
    }

    void GameScriptSystem::Talk(GamePlayer& player, GameNPC& npc, const NPCTalkBox& talkBox)
    {
        player.Talk(npc, talkBox);
    }

    void GameScriptSystem::DisposeTalk(GamePlayer& player)
    {
        PlayerNPCScriptComponent& scriptComponent = player.GetNPCScriptComponent();
        if (!scriptComponent.HasTargetNPC())
        {
            return;
        }

        game_entity_id_type targetId = scriptComponent.GetTargetNPCId();
        player.Send(NPCMessageCreator::CreateTalkBoxClose(targetId));

        scriptComponent.Clear();
    }

    void GameScriptSystem::ChangeStage(GamePlayer& player, int32_t stageId, int32_t destX, int32_t destY)
    {
        _serviceLocator.Get<ZoneChangeService>().StartStageChange(player, stageId, destX, destY);
    }

    void GameScriptSystem::ChangeZone(GamePlayer& player, int32_t zoneId, int32_t destX, int32_t destY)
    {
        _serviceLocator.Get<ZoneChangeService>().StartZoneChange(player.GetClientId(), zoneId, destX, destY);
    }

    void GameScriptSystem::ChangeZoneWithStage(GamePlayer& player, int32_t zoneId, int32_t stage, int32_t destX, int32_t destY)
    {
        _serviceLocator.Get<ZoneChangeService>().StartZoneChange(player.GetClientId(), zoneId, stage, destX, destY);
    }
}
