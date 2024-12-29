#include "server_command_query.h"

#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/system/server_command_system.h"
#include "sl/generator/service/gamedata/gamedata_provide_service.h"
#include "sl/generator/service/gamedata/map/map_data_provider.h"

namespace sunlight
{
    ServerCommandQueryNesCaller::ServerCommandQueryNesCaller(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandQueryNesCaller::GetName() const -> std::string_view
    {
        return "query_nes_caller";
    }

    auto ServerCommandQueryNesCaller::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandQueryNesCaller::Execute(GamePlayer& player, int32_t scriptId) const
    {
        const GameDataProvideService& gameDataProvider = _system.GetServiceLocator().Get<GameDataProvideService>();
        const MapDataProvider& mapDataProvider = gameDataProvider.GetMapDataProvider();

        const NesFile* nesFile = mapDataProvider.FindNesFile(_system.GetZoneId());
        if (!nesFile)
        {
            return false;
        }

        for (const NesScriptCall& scriptCall : nesFile->scriptCalls)
        {
            for (const std::pair<int32_t, int32_t>& pair : scriptCall.scriptCalls)
            {
                if (pair.second == scriptId)
                {
                    player.Notice(fmt::format("caller: {}, script: {}", static_cast<uint32_t>(scriptCall.callerId), scriptId));
                }
            }
        }

        return true;
    }
}
