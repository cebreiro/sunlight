#include "zone_message_router.h"

#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/zone_message.h"
#include "sl/emulator/game/system/item_archive_system.h"
#include "sl/emulator/game/system/player_job_system.h"
#include "sl/emulator/game/system/player_stat_system.h"
#include "sl/emulator/game/zone/stage.h"

namespace sunlight
{
    ZoneMessageRouter::ZoneMessageRouter(Stage& stage)
        : _stage(stage)
    {
    }

    ZoneMessageRouter::~ZoneMessageRouter()
    {
    }

    void ZoneMessageRouter::Subscribe()
    {
        [[maybe_unused]]
        bool success = _stage.AddSubscriber(ZoneMessageType::SLV2_MSG,
            std::bind_front(&ZoneMessageRouter::RouteSlV2Message, this));
        assert(success);
    }

    void ZoneMessageRouter::RouteSlV2Message(const ZoneMessage& message)
    {
        const ZoneMessageType subType = message.reader.Read<ZoneMessageType>();

        switch (subType)
        {
        case ZoneMessageType::SLV2_CHAR_STAT:
        {
            _stage.Get<PlayerStatSystem>().OnStatPointUse(message);
        }
        break;
        case ZoneMessageType::SLV2_SKILL_LEVEL_CHANGE:
        {
            _stage.Get<PlayerJobSystem>().OnSkillLevelSet(message);
        }
        break;
        case ZoneMessageType::SLV2_WEAPON_SWAP:
        {
            _stage.Get<ItemArchiveSystem>().OnWeaponSwap(message);
        }
        break;
        default:
            SUNLIGHT_LOG_WARN(_stage.GetServiceLocator(),
                fmt::format("[{}] unhanlded zone request. player: {}, type: {}, buffer: {}",
                    _stage.GetName(), message.player.GetCId(), ToString(subType), message.reader.GetBuffer().ToString()));
        }
    }
}
