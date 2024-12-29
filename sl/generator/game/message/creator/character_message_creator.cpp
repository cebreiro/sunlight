#include "character_message_creator.h"

#include "sl/generator/game/component/player_appearance_component.h"
#include "sl/generator/game/component/player_job_component.h"
#include "sl/generator/game/component/player_party_component.h"
#include "sl/generator/game/component/player_profile_component.h"
#include "sl/generator/game/component/player_stat_component.h"
#include "sl/generator/game/component/scene_object_component.h"
#include "sl/generator/game/contents/channel/channel_notify_type.h"
#include "sl/generator/game/contents/channel/game_channel_type.h"
#include "sl/generator/game/data/sox/zone.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/message/character_message_type.h"
#include "sl/generator/game/message/zone_message_deliver_type.h"
#include "sl/generator/game/message/zone_message_type.h"
#include "sl/generator/server/packet/zone_packet_s2c.h"
#include "sl/generator/server/packet/io/sl_packet_writer.h"
#include "sl/generator/service/community/party/party_information.h"
#include "sl/generator/service/community/party/party_player_information.h"

namespace sunlight
{
    auto CharacterMessageCreator::CreateProfile(const GamePlayer& targetPlayer, const sox::Zone& zone) -> Buffer
    {
        const bool isPrivateProfile = targetPlayer.GetProfileComponent().IsConfigured(PlayerProfileSetting::Private);

        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_CHR_MESSAGE);
        writer.WriteString(targetPlayer.GetName());
        writer.Write(CharacterMessageType::ShowYouAck);
        writer.Write<int8_t>(isPrivateProfile ? 2 : 1);

        {
            const PlayerAppearanceComponent& appearanceComponent = targetPlayer.GetAppearanceComponent();

            PacketWriter objectWriter;
            objectWriter.Write<int32_t>(targetPlayer.GetStatComponent().GetGender());

            objectWriter.Write<int32_t>(appearanceComponent.GetFace());
            objectWriter.Write<int32_t>(appearanceComponent.GetHatModelId() != 0 ? appearanceComponent.GetHatModelId() : appearanceComponent.GetHair());
            objectWriter.Write<int32_t>(appearanceComponent.GetJacketModelId());
            objectWriter.Write<int32_t>(appearanceComponent.GetGlovesModelId());
            objectWriter.Write<int32_t>(appearanceComponent.GetPantsModelId());
            objectWriter.Write<int32_t>(appearanceComponent.GetShoesModelId());
            objectWriter.Write<int32_t>(appearanceComponent.GetWeaponModelId());

            objectWriter.Write<int32_t>(0);
            objectWriter.Write<int32_t>(appearanceComponent.GetHatModelColor());
            objectWriter.Write<int32_t>(appearanceComponent.GetJacketModelColor());
            objectWriter.Write<int32_t>(appearanceComponent.GetGlovesModelColor());
            objectWriter.Write<int32_t>(appearanceComponent.GetPantsModelColor());
            objectWriter.Write<int32_t>(appearanceComponent.GetShoesModelColor());
            objectWriter.Write<int32_t>(appearanceComponent.GetWeaponModelColor());

            // unk
            objectWriter.Write<int32_t>(0);
            objectWriter.Write<int32_t>(0);
            objectWriter.Write<int32_t>(0);
            objectWriter.Write<int32_t>(0);
            objectWriter.Write<int32_t>(0);
            objectWriter.Write<int32_t>(0);
            objectWriter.Write<int32_t>(0);

            objectWriter.Write<int32_t>(appearanceComponent.GetHairColor());
            objectWriter.Write<int32_t>(appearanceComponent.GetSkinColor());

            writer.WriteObject(objectWriter);
        }

        if (!isPrivateProfile)
        {
            const Job& mainJob = targetPlayer.GetJobComponent().GetMainJob();
            const Eigen::Vector2f& position = targetPlayer.GetSceneObjectComponent().GetPosition();

            PacketWriter objectWriter;
            objectWriter.Write<int8_t>(static_cast<int8_t>(targetPlayer.GetStatComponent().GetLevel()));
            objectWriter.Write<int8_t>(static_cast<int8_t>(mainJob.GetLevel()));
            objectWriter.Write<int8_t>(0); // sub job level
            objectWriter.Write<int8_t>(0); // byte padding

            objectWriter.Write<int32_t>(static_cast<int32_t>(mainJob.GetId()));
            objectWriter.Write<int32_t>(0); // sub job id
            objectWriter.Write<int32_t>(static_cast<int32_t>(position.x()));
            objectWriter.Write<int32_t>(static_cast<int32_t>(position.y()));

            objectWriter.WriteFixeSizeString("", 128);
            objectWriter.WriteFixeSizeString(zone.zoneDesc, 256); // position

            if (const auto& partyComponent = targetPlayer.GetPartyComponent();
                partyComponent.HasParty())
            {
                objectWriter.WriteFixeSizeString(partyComponent.GetPartyName(), 32);
            }
            else
            {
                objectWriter.WriteFixeSizeString("", 32);
            }

            objectWriter.WriteFixeSizeString("", 32);

            writer.WriteObject(objectWriter);
        }

        return writer.Flush();
    }

    auto CharacterMessageCreator::CreateProfileFail(const std::string& targetPlayerName) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_CHR_MESSAGE);
        writer.WriteString(targetPlayerName);
        writer.Write(CharacterMessageType::ShowYouAck);
        writer.Write<int32_t>(0);

        return writer.Flush();
    }

    auto CharacterMessageCreator::CreateIamHere(const std::string& targetPlayerName, int8_t type, float x, float y, float hp) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_CHR_MESSAGE);
        writer.WriteString(targetPlayerName);
        writer.Write(CharacterMessageType::IamHere);
        writer.Write<int8_t>(type);
        writer.WriteString(""); // unk

        writer.Write<float>(x);
        writer.Write<float>(y);
        writer.Write<int32_t>(0);
        writer.Write<float>(hp);

        return writer.Flush();
    }

    auto CharacterMessageCreator::CreateMessageResult(const std::string& targetPlayerName, CharacterMessageType type, bool success) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_CHR_MESSAGE_RESULT);
        writer.WriteString(targetPlayerName);
        writer.Write<int8_t>(success ? 1 : 0);
        writer.Write(type);

        if (type == CharacterMessageType::WhereAreYou)
        {
            writer.Write<int8_t>(success ? 0 : 1);
        }

        return writer.Flush();
    }

    auto CharacterMessageCreator::CreateJoinResult(const std::string& partyName, ChannelJoinResult result) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::CHANNEL_JOIN_RESULT);
        writer.Write(result);
        writer.Write(GameChannelType::Party);
        writer.WriteString(partyName);

        return writer.Flush();
    }

    auto CharacterMessageCreator::CreatePartyInvite(const std::string& inviter, const std::string& partyName, const std::string& unk1, int8_t creation) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_CHR_MESSAGE);
        writer.WriteString(inviter);
        writer.Write(CharacterMessageType::ChannelInvite);
        writer.WriteString(partyName);
        writer.WriteString(unk1);
        writer.Write(GameChannelType::Party);
        writer.Write<int8_t>(creation);

        return writer.Flush();
    }

    auto CharacterMessageCreator::CreatePartyInviteResult(const std::string& inviter, ChannelInviteResult result) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_CHR_MESSAGE);
        writer.WriteString(inviter);
        writer.Write(CharacterMessageType::ChannelInviteResult);
        writer.Write(result);

        return writer.Flush();
    }

    auto CharacterMessageCreator::CreatePartyCreate(const PartyInformation& party, const PartyPlayerInformation& leader, const PartyPlayerInformation& member) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::CHANNEL_CREATE_RESULT);
        writer.Write<int32_t>(0);
        writer.Write(GameChannelType::Party);
        writer.WriteString(party.partyName);
        writer.Write<int8_t>(0);
        writer.Write<int8_t>(0);
        writer.WriteObject(WritePartyPlayer(leader));
        writer.WriteObject(WritePartyPlayer(member));
        writer.WriteObject(WriteParty(party));

        return writer.Flush();
    }

    auto CharacterMessageCreator::CreatePartyList(std::span<const PartyInformation> parties) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::CHANNEL_ENUM_USER_RESULT);
        writer.Write<int32_t>(0);
        writer.Write<int8_t>(1);
        writer.Write<int32_t>(static_cast<int32_t>(std::ssize(parties))); // count

        for (const PartyInformation& party : parties)
        {
            writer.WriteObject(WriteParty(party));
        }

        return writer.Flush();
    }

    auto CharacterMessageCreator::CreatePartyQueryResult(const std::string& partyName, std::span<const PartyPlayerInformation> members) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::PARTY_QUERY_RESULT);
        writer.Write<int32_t>(0);
        writer.WriteString(partyName);
        writer.Write<int32_t>(static_cast<int32_t>(std::ssize(members)));

        for (const PartyPlayerInformation& member : members)
        {
            writer.WriteObject(WritePartyPlayer(member));
        }

        return writer.Flush();
    }

    auto CharacterMessageCreator::CreatePartyMemberAdd(const std::string& partyName, const PartyPlayerInformation& member) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_CHR_MESSAGE);
        writer.WriteString(partyName);
        writer.Write(CharacterMessageType::ChannelNotifyMsg);
        writer.Write(ChannelNotifyType::PartyPlayerUpdate);
        writer.Write(GameChannelType::Party);
        writer.WriteString(partyName);
        writer.WriteString(member.name);
        writer.WriteObject(WritePartyPlayer(member));

        return writer.Flush();
    }

    auto CharacterMessageCreator::CreatePartyMemberEnterNotify(const std::string& partyName, const std::string& target) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_CHR_MESSAGE);
        writer.WriteString(partyName);
        writer.Write(CharacterMessageType::ChannelNotifyMsg);
        writer.Write(ChannelNotifyType::Enter);
        writer.Write(GameChannelType::Party);
        writer.WriteString(partyName);
        writer.WriteString(target);

        return writer.Flush();
    }

    auto CharacterMessageCreator::CreatePartyLeave() -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_NORMAL_MESSAGE);
        writer.Write(ZoneMessageType::CHANNEL_LEAVE_RESULT);
        writer.Write<int32_t>(0);
        writer.Write(GameChannelType::Party);

        return writer.Flush();
    }

    auto CharacterMessageCreator::CreatePartyMemberLeave(const std::string& partyName, const std::string& memberName) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_CHR_MESSAGE);
        writer.WriteString(partyName);
        writer.Write(CharacterMessageType::ChannelNotifyMsg);
        writer.Write(ChannelNotifyType::Leave);
        writer.Write(GameChannelType::Party);
        writer.WriteString(partyName);
        writer.WriteString(memberName);
        writer.WriteString("");

        return writer.Flush();
    }

    auto CharacterMessageCreator::CreatePartyForceExit(const std::string& partyName, const std::string& targetName) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_CHR_MESSAGE);
        writer.WriteString(partyName);
        writer.Write(CharacterMessageType::ChannelNotifyMsg);
        writer.Write(ChannelNotifyType::ForceLeave);
        writer.Write(GameChannelType::Party);
        writer.WriteString(partyName);
        writer.WriteString(targetName);
        writer.WriteString("");

        return writer.Flush();
    }

    auto CharacterMessageCreator::CreatePartyDisband(const std::string& partyName, bool autoDisband) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_CHR_MESSAGE);
        writer.WriteString(partyName);
        writer.Write(CharacterMessageType::ChannelNotifyMsg);
        writer.Write(autoDisband ? ChannelNotifyType::PartyAutoDisband : ChannelNotifyType::PartyDisband);
        writer.Write(GameChannelType::Party);

        return writer.Flush();
    }

    auto CharacterMessageCreator::CreatePartyLeaderChange(const std::string& partyName, const std::string& newLeaderName) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_CHR_MESSAGE);
        writer.WriteString(partyName);
        writer.Write(CharacterMessageType::ChannelNotifyMsg);
        writer.Write(ChannelNotifyType::PartyLeaderChange);
        writer.Write(GameChannelType::Party);
        writer.WriteString("");
        writer.WriteString("");
        writer.WriteString(newLeaderName);

        return writer.Flush();
    }

    auto CharacterMessageCreator::CreatePartyOptionChange(const PartyInformation& party) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_CHR_MESSAGE);
        writer.WriteString(party.partyName);
        writer.Write(CharacterMessageType::ChannelNotifyMsg);
        writer.Write(ChannelNotifyType::PartyOptionChange);
        writer.Write(GameChannelType::Party);
        writer.WriteString("");
        writer.WriteString("");
        writer.WriteObject(WriteParty(party));

        return writer.Flush();
    }

    auto CharacterMessageCreator::CreatePartyJoinRequest(const std::string& requesterName) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_CHR_MESSAGE);
        writer.WriteString(requesterName);
        writer.Write(CharacterMessageType::JoinReq);

        return writer.Flush();
    }

    auto CharacterMessageCreator::CreatePartyJoinRejected(const std::string& partyLeaderName) -> Buffer
    {
        SlPacketWriter writer;
        writer.Write(ZonePacketS2C::NMS_DELIVER_MESSAGE);
        writer.Write(ZoneMessageDeliverType::MSG_SC_CHR_MESSAGE);
        writer.WriteString(partyLeaderName);
        writer.Write(CharacterMessageType::JoinReje);

        return writer.Flush();
    }

    auto CharacterMessageCreator::WritePartyPlayer(const PartyPlayerInformation& player) -> Buffer
    {
        PacketWriter writer;
        writer.WriteFixeSizeString(player.name, 32);
        writer.Write<int8_t>(0);
        writer.Write<int32_t>(player.jobId);
        writer.Write<int8_t>(player.jobLevel);
        writer.Write<int32_t>(player.hp);
        writer.Write<int32_t>(player.maxHP);

        return writer.Flush();
    }

    auto CharacterMessageCreator::WriteParty(const PartyInformation& party) -> Buffer
    {
        PacketWriter writer;
        writer.WriteFixeSizeString(party.partyName, 32);
        writer.Write<int8_t>(0);
        writer.WriteFixeSizeString(party.leaderName, 32);
        writer.Write<int8_t>(0);
        writer.WriteZeroBytes(257);

        writer.Write<int8_t>(party.memberCount);
        writer.Write<int8_t>(party.isPublic ? 1 : 0);
        writer.Write<int8_t>(party.goldDistribution ? 1 : 0);
        writer.Write<int8_t>(party.itemDistribution ? 1 : 0);
        writer.Write<int8_t>(party.leaderCharacterLevel);
        writer.Write<int32_t>(party.leaderZoneId);

        return writer.Flush();
    }
}
