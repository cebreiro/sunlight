#include "server_command_debug.h"

#include "shared/execution/executor/operation/schedule.h"
#include "sl/emulator/game/component/player_debug_component.h"
#include "sl/emulator/game/component/player_stat_component.h"
#include "sl/emulator/game/debug/game_debugger.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/system/server_command_system.h"
#include "sl/emulator/game/zone/service/zone_timer_service.h"

namespace sunlight
{
    ServerCommandDebugServer::ServerCommandDebugServer(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandDebugServer::GetName() const -> std::string_view
    {
        return "debug_server";
    }

    auto ServerCommandDebugServer::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandDebugServer::Execute(GamePlayer& player, int32_t type, int32_t value) const
    {
        (void)player;

        if (GameDebugger* debugger = _system.GetServiceLocator().Find<GameDebugger>(); debugger)
        {
            if (type < 0 || type >= static_cast<int32_t>(GameDebugType::Count))
            {
                return false;
            }

            debugger->SetState(static_cast<GameDebugType>(type), value);

            return true;
        }

        return false;
    }

    ServerCommandDebugServerOff::ServerCommandDebugServerOff(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandDebugServerOff::GetName() const -> std::string_view
    {
        return "debug_server_off";
    }

    auto ServerCommandDebugServerOff::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandDebugServerOff::Execute(GamePlayer& player) const
    {
        (void)player;

        if (GameDebugger* debugger = _system.GetServiceLocator().Find<GameDebugger>(); debugger)
        {
            debugger->Clear();

            return true;
        }

        return false;
    }

    ServerCommandDebugPlayerHP::ServerCommandDebugPlayerHP(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandDebugPlayerHP::GetName() const -> std::string_view
    {
        return "debug_player_hp";
    }

    auto ServerCommandDebugPlayerHP::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandDebugPlayerHP::Execute(GamePlayer& player) const
    {
        PlayerDebugComponent& playerDebugComponent = player.GetDebugComponent();
        playerDebugComponent.Toggle(PlayerDebugType::HP);

        if (playerDebugComponent.IsSet(PlayerDebugType::HP))
        {
            DebugNotify(player);
        }

        return true;
    }

    void ServerCommandDebugPlayerHP::DebugNotify(GamePlayer& player) const
    {
        if (!player.GetDebugComponent().IsSet(PlayerDebugType::HP))
        {
            return;
        }

        PlayerStatComponent& statComponent = player.GetStatComponent();
        const int32_t hp = statComponent.GetFinalStat(RecoveryStatType::HP).As<int32_t>();
        const int32_t maxHP = statComponent.GetFinalStat(PlayerStatType::MaxHP).As<int32_t>();

        player.Notice(fmt::format("HP: {} / {}", hp, maxHP));

        _system.GetServiceLocator().Get<ZoneTimerService>().AddTimer(std::chrono::milliseconds(1000), player.GetCId(), _system.GetStageId(),
            [this](GamePlayer& player)
            {
                DebugNotify(player);
            });
    }

    ServerCommandDebugPlayerSP::ServerCommandDebugPlayerSP(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandDebugPlayerSP::GetName() const -> std::string_view
    {
        return "debug_player_sp";
    }

    auto ServerCommandDebugPlayerSP::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandDebugPlayerSP::Execute(GamePlayer& player) const
    {
        PlayerDebugComponent& playerDebugComponent = player.GetDebugComponent();
        playerDebugComponent.Toggle(PlayerDebugType::SP);

        if (playerDebugComponent.IsSet(PlayerDebugType::SP))
        {
            DebugNotify(player);
        }

        return true;
    }

    void ServerCommandDebugPlayerSP::DebugNotify(GamePlayer& player) const
    {
        PlayerStatComponent& statComponent = player.GetStatComponent();
        const int32_t sp = statComponent.GetFinalStat(RecoveryStatType::SP).As<int32_t>();
        const int32_t maxSP = statComponent.GetFinalStat(PlayerStatType::MaxSP).As<int32_t>();

        player.Notice(fmt::format("SP: {} / {}", sp, maxSP));

        _system.GetServiceLocator().Get<ZoneTimerService>().AddTimer(std::chrono::milliseconds(1000), player.GetCId(), _system.GetStageId(),
            [this](GamePlayer& player)
            {
                DebugNotify(player);
            });
    }
}
