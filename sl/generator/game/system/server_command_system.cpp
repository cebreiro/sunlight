#include "server_command_system.h"

#include <boost/algorithm/string.hpp>

#include "sl/generator/game/contents/server_command/server_command_interface.h"
#include "sl/generator/game/contents/server_command/server_command_register.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/zone_community_message.h"
#include "sl/generator/game/system/entity_ai_control_system.h"
#include "sl/generator/game/system/entity_damage_system.h"
#include "sl/generator/game/system/entity_movement_system.h"
#include "sl/generator/game/system/entity_status_effect_system.h"
#include "sl/generator/game/system/entity_view_range_system.h"
#include "sl/generator/game/system/item_archive_system.h"
#include "sl/generator/game/system/npc_shop_system.h"
#include "sl/generator/game/system/path_finding_system.h"
#include "sl/generator/game/system/player_stat_system.h"
#include "sl/generator/game/system/player_job_system.h"
#include "sl/generator/game/system/player_quest_system.h"
#include "sl/generator/game/system/game_script_system.h"
#include "sl/generator/game/system/scene_object_system.h"
#include "sl/generator/game/zone/stage.h"

namespace sunlight
{
    ServerCommandSystem::ServerCommandSystem(const ServiceLocator& serviceLocator, int32_t zoneId, int32_t stageId)
        : _serviceLocator(serviceLocator)
        , _zoneId(zoneId)
        , _stageId(stageId)
        , _mt(std::random_device{}())
    {
    }

    void ServerCommandSystem::InitializeSubSystem(Stage& stage)
    {
        Add(stage.Get<ItemArchiveSystem>());
        Add(stage.Get<SceneObjectSystem>());
        Add(stage.Get<PlayerStatSystem>());
        Add(stage.Get<PlayerJobSystem>());
        Add(stage.Get<EntityViewRangeSystem>());
        Add(stage.Get<EntityMovementSystem>());
        Add(stage.Get<NPCShopSystem>());
        Add(stage.Get<GameScriptSystem>());
        Add(stage.Get<EntityDamageSystem>());
        Add(stage.Get<EntityAIControlSystem>());
        Add(stage.Get<PlayerQuestSystem>());
        Add(stage.Get<EntityStatusEffectSystem>());

        if (PathFindingSystem* pathFindSystem = stage.Find<PathFindingSystem>();
            pathFindSystem)
        {
            Add(*pathFindSystem);
        }

        ServerCommandRegister::Register(*this);
    }

    bool ServerCommandSystem::Subscribe(Stage& stage)
    {
        if (!stage.AddSubscriber(ZoneMessageType::SERVERCOMMAND_STRING,
            std::bind_front(&ServerCommandSystem::HandleCommand, this)))
        {
            return false;
        }

        return true;
    }

    auto ServerCommandSystem::GetName() const -> std::string_view
    {
        return "gm_command_system";
    }

    auto ServerCommandSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<ServerCommandSystem>();
    }

    void ServerCommandSystem::AddCommand(SharedPtrNotNull<IServerCommand> command)
    {
        std::string name(command->GetName());

        [[maybe_unused]]
        const bool inserted = _commands.try_emplace(std::move(name), std::move(command)).second;
        assert(inserted);
    }

    auto ServerCommandSystem::GetServiceLocator() const -> const ServiceLocator&
    {
        return _serviceLocator;
    }

    auto ServerCommandSystem::GetZoneId() const -> int32_t
    {
        return _zoneId;
    }

    auto ServerCommandSystem::GetStageId() const -> int32_t
    {
        return _stageId;
    }

    auto ServerCommandSystem::GetRandomEngine() -> std::mt19937&
    {
        return _mt;
    }

    void ServerCommandSystem::HandleCommand(const ZoneCommunityMessage& message)
    {
        const std::string& request = message.reader.ReadString();

        LogRequest(message.player, request);

        _splitStringBuffer.clear();
        boost::algorithm::split(_splitStringBuffer, request, boost::is_any_of(" "));

        if (_splitStringBuffer.empty())
        {
            return;
        }

        std::string command(_splitStringBuffer[0]);
        std::ranges::transform(command, command.begin(), [](char c) -> char
            {
                return static_cast<char>(tolower(c));
            });

        const auto iter = _commands.find(command);
        if (iter == _commands.end())
        {
            return;
        }

        ServerCommandParamReader reader(std::span(_splitStringBuffer.begin() + 1, _splitStringBuffer.end()));

        const bool result = iter->second->HandleCommand(message.player, reader);

        LogResult(message.player, request, result);
    }

    void ServerCommandSystem::LogRequest(const GamePlayer& player, const std::string& command)
    {
        SUNLIGHT_LOG_INFO(_serviceLocator,
            fmt::format("[{}] gm_command request. player: {}, command: {}",
                GetName(), player.GetCId(), command));
    }

    void ServerCommandSystem::LogResult(const GamePlayer& player, const std::string& command, bool result)
    {
        SUNLIGHT_LOG_INFO(_serviceLocator,
            fmt::format("[{}] gm_command result. player: {}, result: {}, command: {}",
                GetName(), player.GetCId(), result, command));
    }
}
