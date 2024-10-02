#include "zone_message_hooker.h"

#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/zone_message.h"
#include "sl/emulator/game/system/item_archive_system.h"
#include "sl/emulator/game/system/npc_shop_system.h"
#include "sl/emulator/game/system/player_job_system.h"
#include "sl/emulator/game/system/player_profile_system.h"
#include "sl/emulator/game/system/player_stat_system.h"
#include "sl/emulator/game/zone/stage.h"

namespace sunlight
{
    ZoneMessageHooker::ZoneMessageHooker(Stage& stage)
        : _stage(stage)
    {
    }

    ZoneMessageHooker::~ZoneMessageHooker()
    {
    }

    bool ZoneMessageHooker::ProcessEvent(const ZoneMessage& message)
    {
        SlPacketReader& reader = message.reader;

        switch (message.type)
        {
        case ZoneMessageType::ITEMARCHIVEMSG:
        {
            const ZoneMessageType subType = reader.Peek<ZoneMessageType>();

            switch (subType)
            {
            case ZoneMessageType::NPCITEMARCHIVE_REQUESTSYNCHRO:
            {
                reader.Skip();

                _stage.Get<NPCShopSystem>().OnShopSynchronizeRequest(message);

                return true;
            }
            case ZoneMessageType::NPCITEMARCHIVE_STOPSYNCHRO:
            {
                reader.Skip();

                _stage.Get<NPCShopSystem>().OnShopSynchronizeStop(message);

                return true;
            }
            case ZoneMessageType::NPCITEMARCHIVE_BUYITEM:
            {
                reader.Skip();

                _stage.Get<NPCShopSystem>().OnPlayerBuyShopItem(message);

                return true;
            }
            case ZoneMessageType::NPCITEMARCHIVE_SELLITEM:
            {
                reader.Skip();

                _stage.Get<NPCShopSystem>().OnPlayerSellOwnItem(message);

                return true;
            }
            case ZoneMessageType::BARBER_PAYMENT:
            {
                reader.Skip();

                _stage.Get<NPCShopSystem>().OnBarberPayment(message);

                return true;
            }
            default:;
            }

        }
        break;
        case ZoneMessageType::PGROUP_MSG:
        {
            const ZoneMessageType subType = reader.Peek<ZoneMessageType>();

            switch (subType)
            {
            case ZoneMessageType::PROFILE_SETTING_CHANGE:
            {
                reader.Skip();

                _stage.Get<PlayerProfileSystem>().OnProfileSettingChange(message);

                return true;
            }
            break;
            }
        }
        break;
        case ZoneMessageType::SLV2_MSG:
        {
            const ZoneMessageType subType = reader.Peek<ZoneMessageType>();

            switch (subType)
            {
            case ZoneMessageType::SLV2_CHAR_STAT:
            {
                reader.Skip();

                _stage.Get<PlayerStatSystem>().OnStatPointUse(message);

                return true;
            }
            case ZoneMessageType::SLV2_SKILL_LEVEL_CHANGE:
            {
                reader.Skip();

                _stage.Get<PlayerJobSystem>().OnSkillLevelSet(message);

                return true;
            }
            case ZoneMessageType::SLV2_WEAPON_SWAP:
            {
                reader.Skip();

                _stage.Get<ItemArchiveSystem>().OnWeaponSwap(message);

                return true;
            }
            default:
            SUNLIGHT_LOG_WARN(_stage.GetServiceLocator(),
                fmt::format("[{}] unhanlded slv2 message. player: {}, type: {}, buffer: {}",
                    _stage.GetName(), message.player.GetCId(), ToString(subType), message.reader.GetBuffer().ToString()));
            }
        }
        break;
        }

        return false;
    }
}
