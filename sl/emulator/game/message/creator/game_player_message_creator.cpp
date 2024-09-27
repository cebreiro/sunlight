#include "game_player_message_creator.h"

#include "sl/emulator/game/component/player_appearance_component.h"
#include "sl/emulator/game/component/player_quest_component.h"
#include "sl/emulator/game/contants/quest/quest.h"
#include "sl/emulator/game/entity/game_entity_network_id.h"
#include "sl/emulator/game/entity/game_item.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/zone_message_type.h"
#include "sl/emulator/game/message/zone_message_deliver_type.h"
#include "sl/emulator/game/message/creator/zone_data_transfer_object_creator.h"
#include "sl/emulator/server/packet/zone_packet_s2c.h"
#include "sl/emulator/server/packet/io/sl_packet_writer.h"
#include "sl/emulator/service/gamedata/item/item_data.h"

namespace sunlight
{
    auto GamePlayerMessageCreator::CreateAllState(const GamePlayer& player) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_GOB_MESSAGE);
        writer.Write<int32_t>(0);
        writer.WriteObject(GameEntityNetworkId(player).ToBuffer()); // virtual function handler by entity type. client 0x4EC3F3
        writer.Write(ZoneMessageType::CONTAIN_ALL_STATE);
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerUnkState(player));
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerAppearance(player));
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerInformation(player));
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerPet(player));
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerSkill(player));
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerStatusEffect(player));

        return writer.Flush();
    }

    auto GamePlayerMessageCreator::CreateRemotePlayerState(const GamePlayer& player) -> Buffer
    {
        // client 0x48A4F0

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

        writer.Write<int32_t>(appearanceComponent.GetHair());
        writer.Write<int32_t>(appearanceComponent.GetHairColor());
        writer.Write<int32_t>(appearanceComponent.GetSkinColor());

        // color
        {
            PacketWriter objectWriter;
            objectWriter.Write<int32_t>(0);
            objectWriter.Write<int32_t>(appearanceComponent.GetHatModelColor());
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
            objectWriter.Write<int32_t>(appearanceComponent.GetHatModelId());
            objectWriter.Write<int32_t>(appearanceComponent.GetJacketModelId());
            objectWriter.Write<int32_t>(appearanceComponent.GetGlovesModelId());
            objectWriter.Write<int32_t>(appearanceComponent.GetPantsModelId());
            objectWriter.Write<int32_t>(appearanceComponent.GetShoesModelId());
            objectWriter.Write<int32_t>(appearanceComponent.GetWeaponModelId());

            writer.WriteObject(objectWriter);
        }

        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerWeaponMotion(player));
        writer.WriteObject(ZoneDataTransferObjectCreator::CreatePlayerStat(player));
        
        // unk maybe status_effect
        writer.WriteZeroBytes(24);

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
        writer.Write<int32_t>(static_cast<int32_t>(jobId) / 1000 - 1); // client 485C10h
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
}
