#include "game_player_message_creator.h"

#include "sl/generator/game/component/player_appearance_component.h"
#include "sl/generator/game/component/player_profile_component.h"
#include "sl/generator/game/component/player_quest_component.h"
#include "sl/generator/game/contents/group/game_group_state.h"
#include "sl/generator/game/contents/quest/quest.h"
#include "sl/generator/game/entity/game_entity_network_id.h"
#include "sl/generator/game/entity/game_item.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/zone_message_type.h"
#include "sl/generator/game/message/zone_message_deliver_type.h"
#include "sl/generator/game/message/creator/zone_data_transfer_object_creator.h"
#include "sl/generator/server/packet/zone_packet_s2c.h"
#include "sl/generator/server/packet/io/sl_packet_writer.h"
#include "sl/generator/service/gamedata/item/item_data.h"

namespace sunlight
{
    auto GamePlayerMessageCreator::CreateAllState(const GamePlayer& player) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::CONTAIN_ALL_STATE);
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerGroupState(player));
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerAppearance(player));
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerInformation(player));
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerPet(player));
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerSkill(player));
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerStatusEffect(player));

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateRemotePlayerState(const GamePlayer& player) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::CONTAIN_ACTIVATE_STATE);
        writer.WriteString(""); // chatting
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(0);

        // mini-game
        {
            PacketWriter objectWriter;
            objectWriter.Write<int32_t>(0);
            objectWriter.Write<int32_t>(0); // 1 -> dice
            objectWriter.Write<int32_t>(0);
            objectWriter.Write<int32_t>(0);

            writer.WriteObject(objectWriter);
            writer.WriteInt64(0, 0);
        }

        writer.Write<int32_t>(player.GetGmLevel());
        writer.Write<int8_t>(0);
        writer.Write<int8_t>(0);
        writer.Write<int8_t>(0);

        const PlayerAppearanceComponent& appearanceComponent = player.GetAppearanceComponent();
        const int32_t hatModelId = appearanceComponent.GetHatModelId();

        writer.Write<int32_t>(hatModelId != 0 ? appearanceComponent.GetHair() : 0);
        writer.Write<int32_t>(hatModelId != 0 ? appearanceComponent.GetHatModelColor() : appearanceComponent.GetHairColor());
        writer.Write<int32_t>(appearanceComponent.GetSkinColor());

        // color
        {
            PacketWriter objectWriter;
            objectWriter.Write<int32_t>(appearanceComponent.GetHairColor());
            objectWriter.Write<int32_t>(hatModelId != 0 ? appearanceComponent.GetHairColor() : appearanceComponent.GetHatModelColor());
            objectWriter.Write<int32_t>(appearanceComponent.GetJacketModelColor());
            objectWriter.Write<int32_t>(appearanceComponent.GetGlovesModelColor());
            objectWriter.Write<int32_t>(appearanceComponent.GetPantsModelColor());
            objectWriter.Write<int32_t>(appearanceComponent.GetShoesModelColor());
            objectWriter.Write<int32_t>(appearanceComponent.GetWeaponModelColor());

            writer.WriteObject(objectWriter);
        }

        // model
        {
            PacketWriter objectWriter;
            objectWriter.Write<int32_t>(appearanceComponent.GetFace());
            objectWriter.Write<int32_t>(hatModelId != 0 ? hatModelId : appearanceComponent.GetHair());
            objectWriter.Write<int32_t>(appearanceComponent.GetJacketModelId());
            objectWriter.Write<int32_t>(appearanceComponent.GetGlovesModelId());
            objectWriter.Write<int32_t>(appearanceComponent.GetPantsModelId());
            objectWriter.Write<int32_t>(appearanceComponent.GetShoesModelId());
            objectWriter.Write<int32_t>(appearanceComponent.GetWeaponModelId());

            writer.WriteObject(objectWriter);
        }

        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerWeaponMotion(player));
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerStat(player));
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerStatusEffect(player));

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateCharacterExpGain(const GamePlayer& player, int32_t value) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::SLV2_MSG);
        writer.Write(ZoneMessageType::SLV2_GAIN_CHARACTER_EXP);
        writer.Write<int32_t>(value);

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateCharacterLevelUp(const GamePlayer& player) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::SLV2_MSG);
        writer.Write(ZoneMessageType::SLV2_SHOW_CHR_LEVELUP_FX);

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateJobExpGain(const GamePlayer& player, JobId jobId, int32_t exp) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::SLV2_MSG);
        writer.Write(ZoneMessageType::SLV2_GAIN_JOB_EXP);
        writer.Write<int32_t>(exp);
        writer.Write<int32_t>(static_cast<int32_t>(jobId));

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateJobExpLevelUp(const GamePlayer& player) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::SLV2_MSG);
        writer.Write(ZoneMessageType::SLV2_SHOW_JOB_LEVELUP_FX);
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerStat(player));

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateJobSkillAdd(const GamePlayer& player, JobId jobId, int32_t skillId, int32_t unk) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::SLV2_MSG);
        writer.Write(ZoneMessageType::ADD_ABILITY);
        writer.Write<int32_t>(skillId);
        writer.Write<int32_t>(static_cast<int32_t>(jobId));
        writer.Write<int32_t>(unk);

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateJobSkillLevelChange(const GamePlayer& player, int32_t skillId, int32_t skillLevel) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::SLV2_MSG);
        writer.Write(ZoneMessageType::SLV2_SKILL_LEVEL_CHANGE);
        writer.Write<int32_t>(skillId);
        writer.Write<int32_t>(skillLevel);

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateJobSkillPointChange(const GamePlayer& player, JobId jobId, int32_t newSkillPoint, bool unk) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::SLV2_MSG);
        writer.Write(ZoneMessageType::SLV2_CHANGE_SKILL_POINT);
        writer.Write<int32_t>(newSkillPoint);
        writer.Write<int32_t>(static_cast<int32_t>(jobId) / 1000 - 1);
        writer.Write<int8_t>(unk ? 1 : 0);

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateJobPromotion(const GamePlayer& player, JobId jobId) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::SLV2_MSG);
        writer.Write(ZoneMessageType::SLV2_PROMOTE_JOB);
        writer.Write<int32_t>(static_cast<int32_t>(jobId));

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateMixSkillExpChange(const GamePlayer& player, int32_t skillId, int32_t exp) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::SLV2_MSG);
        writer.Write(ZoneMessageType::SLV2_SET_MIX_SKILL_EXP);
        writer.Write<int32_t>(skillId);
        writer.Write<int32_t>(exp);

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreatePlayerGainGroupItem(const GamePlayer& player, int32_t x, int32_t y) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::PLAYER_PICK_ITEM_FX);
        writer.Write<int32_t>(x);
        writer.Write<int32_t>(y);

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreatePlayerWeaponSwap(const GamePlayer& player, const GameItem* weapon) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::SLV2_MSG);
        writer.Write(ZoneMessageType::SLV2_WEAPON_SWAP);
        writer.Write<int32_t>(weapon ? weapon->GetData().GetModelId() : 0);
        writer.Write<int32_t>(weapon ? weapon->GetData().GetModelColor() : 0);

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateRemovePlayerWeaponChange(const GamePlayer& player) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::SLV2_MSG);
        writer.Write(ZoneMessageType::SLV2_CHANGE_REMOTE_PLAYER_MOTION);
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerWeaponMotion(player));

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreatePlayerEquipmentChange(const GamePlayer& player, EquipmentPosition position,
        int32_t modelId, int32_t modelColor) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::MULTIPLAYER_SYNC_MSG);
        writer.Write(ZoneMessageType::MULTIPLAYER_SYNC_CHANGE_EQUIPMENT);
        writer.Write<int32_t>(static_cast<int32_t>(position));
        writer.Write<int32_t>(modelId);
        writer.Write<int32_t>(modelColor);

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreatePlayerHairColorChange(const GamePlayer& player, int32_t newColor) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::MULTIPLAYER_SYNC_MSG);
        writer.Write(ZoneMessageType::MULTIPLAYER_SYNC_CHANGE_HAIR_COLOR);
        writer.Write<int32_t>(newColor);

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreatePlayerHairChange(const GamePlayer& player, int32_t newHair, bool hasHat) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::MULTIPLAYER_SYNC_MSG);
        writer.Write(ZoneMessageType::MULTIPLAYER_SYNC_CHANGE_HAIR);
        writer.Write<int32_t>(newHair);
        writer.Write<int8_t>(hasHat ? 1 : 0);

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreatePlayerSkinColorChange(const GamePlayer& player, int32_t newColor) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::MULTIPLAYER_SYNC_MSG);
        writer.Write(ZoneMessageType::MULTIPLAYER_SYNC_CHANGE_SKIN_COLOR);
        writer.Write<int32_t>(newColor);

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateQuestAllState(const GamePlayer& player) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::QUESTARCHIVEMSG);
        writer.Write(ZoneMessageType::QUESTARCHIVE_CONTAINALL);

        {
            const auto& quests = player.GetQuestComponent().GetQuests();

            PacketWriter objectWriter;
            objectWriter.Write<int32_t>(static_cast<int32_t>(std::ssize(quests)));

            for (const Quest& quest : quests | std::views::values)
            {
                objectWriter.Write<int32_t>(quest.GetId());
                objectWriter.Write<int32_t>(quest.GetState());

                const auto& flags = quest.GetFlags();
                if (flags.empty())
                {
                    continue;
                }

                const int32_t lastIndex = flags.rbegin()->first;
                const int32_t count = lastIndex + 1;

                objectWriter.Write<int32_t>(count);

                for (int32_t i = 0; i < count; ++i)
                {
                    const auto iter = flags.find(i);
                    if (iter == flags.end())
                    {
                        objectWriter.Write<int32_t>(-1);
                    }
                    else
                    {
                        objectWriter.Write<int32_t>(iter->second);
                    }
                }
            }

            writer.Write<int32_t>(static_cast<int32_t>(objectWriter.GetWriteSize()));
            writer.WriteObject(objectWriter);
        }

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateQuestAdd(const GamePlayer& player, const Quest& quest) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::QUESTARCHIVEMSG);
        writer.Write(ZoneMessageType::QUESTARCHIVE_NEWQUEST);

        {
            PacketWriter objectWriter;
            objectWriter.Write<int32_t>(quest.GetId());
            objectWriter.Write<int32_t>(quest.GetState());

            const auto& flags = quest.GetFlags();
            if (!flags.empty())
            {
                const int32_t lastIndex = flags.rbegin()->first;
                const int32_t count = lastIndex + 1;

                objectWriter.Write<int32_t>(count);

                for (int32_t i = 0; i < count; ++i)
                {
                    const auto iter = flags.find(i);
                    if (iter == flags.end())
                    {
                        objectWriter.Write<int32_t>(-1);
                    }
                    else
                    {
                        objectWriter.Write<int32_t>(iter->second);
                    }
                }
            }

            writer.Write<int32_t>(static_cast<int32_t>(objectWriter.GetWriteSize()));
            writer.WriteObject(objectWriter);
        }

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateQuestStateChange(const GamePlayer& player, int32_t questId, int32_t oldState, int32_t newState) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::QUESTARCHIVEMSG);
        writer.Write(ZoneMessageType::QUESTARCHIVE_SETSTATE);
        writer.Write<int32_t>(questId);
        writer.Write<int32_t>(oldState);
        writer.Write<int32_t>(newState);

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateQuestFlagSet(const GamePlayer& player, int32_t questId, int32_t state, int32_t index, int32_t flag) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::QUESTARCHIVEMSG);
        writer.Write(ZoneMessageType::QUESTARCHIVE_SETFLAGDATA);
        writer.Write<int32_t>(questId);
        writer.Write<int32_t>(state);
        writer.Write<int32_t>(index);
        writer.Write<int32_t>(flag);

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateQuestGiveUpNotify(const GamePlayer& player, bool success) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::QUESTARCHIVEMSG);
        writer.Write(ZoneMessageType::QUESTARCHIVE_GIVEUP_QUEST);
        writer.Write<int32_t>(success ? 1 : 0);

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateEventScriptAddString(const GamePlayer& player, int32_t index) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::EVENTSCRIPT_MESSAGE);
        writer.Write(ZoneMessageType::EVENTSCRIPTMSG_ADDSTR);
        writer.Write<int32_t>(index);

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateEventScriptAddStringWithInt(const GamePlayer& player, int32_t index, int32_t value) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::EVENTSCRIPT_MESSAGE);
        writer.Write(ZoneMessageType::EVENTSCRIPTMSG_ADDINT);
        writer.Write<int32_t>(index);
        writer.Write<int32_t>(value);

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateEventScriptAddStringWithItem(const GamePlayer& player, int32_t index, int32_t itemId) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::EVENTSCRIPT_MESSAGE);
        writer.Write(ZoneMessageType::EVENTSCRIPTMSG_ADDITEM);
        writer.Write<int32_t>(index);
        writer.Write<int32_t>(itemId);

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateEventScriptClear(const GamePlayer& player) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::EVENTSCRIPT_MESSAGE);
        writer.Write(ZoneMessageType::EVENTSCRIPTMSG_CLEAR);

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateEventScriptShow(const GamePlayer& player) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::EVENTSCRIPT_MESSAGE);
        writer.Write(ZoneMessageType::EVENTSCRIPTMSG_SHOW);

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateProfileSettingChangeResult(const GamePlayer& player) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::PGROUP_MSG);
        writer.Write(ZoneMessageType::PROFILE_SETTING_CHANGE_RESULT);
        writer.Write<int8_t>(player.GetProfileComponent().GetBitmask());

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateProfileIntroduction(const GamePlayer& player, const PlayerProfileIntroduction& introduction) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::PROFILE_ARCHIVE_MSG);
        writer.Write(ZoneMessageType::PROFILE_ARCHIVE_REQUEST_USER_DATA);
        writer.Write<int8_t>(1);

        {
            PacketWriter objectWriter;
            objectWriter.WriteFixeSizeString("", 128); // is not displayed
            objectWriter.WriteFixeSizeString(introduction.age, 128);
            objectWriter.WriteFixeSizeString(introduction.sex, 128);
            objectWriter.WriteFixeSizeString("", 128); // is not displayed
            objectWriter.WriteFixeSizeString(introduction.mail, 128);
            objectWriter.WriteFixeSizeString(introduction.message, 1024);

            writer.WriteObject(objectWriter);
        }

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateProfileIntroductionFail(const GamePlayer& player) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::PROFILE_ARCHIVE_MSG);
        writer.Write(ZoneMessageType::PROFILE_ARCHIVE_REQUEST_USER_DATA);
        writer.Write<int8_t>(0);

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateProfileIntroductionSaveResult(const GamePlayer& player) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::PROFILE_ARCHIVE_MSG);
        writer.Write(ZoneMessageType::PROFILE_ARCHIVE_SAVE_USER_DATA);
        writer.Write<int8_t>(0); // no usage

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreatePlayerStateProposition(const GamePlayer& player, const std::string& title, int32_t groupId, GameGroupType groupType, int32_t unk2) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::STATE_PROPOSITION);
        writer.WriteString(title);
        writer.Write<int32_t>(groupId);
        writer.Write(groupType);
        writer.Write<int32_t>(unk2);

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreatePlayerStateProposition(const GamePlayer& player, const GameGroupState& groupState) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer());
        writer.Write(ZoneMessageType::STATE_PROPOSITION);
        writer.WriteString(groupState.title);
        writer.Write<int32_t>(groupState.groupId);
        writer.Write(groupState.groupType);
        writer.Write<int32_t>(groupState.type);

        return writer.Flush();
    }
}
