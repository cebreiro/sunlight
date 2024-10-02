#include "character_message_creator.h"

#include "sl/emulator/game/component/player_appearance_component.h"
#include "sl/emulator/game/component/player_job_component.h"
#include "sl/emulator/game/component/player_profile_component.h"
#include "sl/emulator/game/component/player_stat_component.h"
#include "sl/emulator/game/component/scene_object_component.h"
#include "sl/emulator/game/data/sox/zone.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/game/message/character_message_type.h"
#include "sl/emulator/game/message/zone_message_deliver_type.h"
#include "sl/emulator/server/packet/zone_packet_s2c.h"
#include "sl/emulator/server/packet/io/sl_packet_writer.h"

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
            // client 0x4372A0, 0x436D70

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
            // client 0x4373C0, UIWindowProfileOtherCharacter

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

            objectWriter.WriteFixeSizeString("", 128); // no reference
            objectWriter.WriteFixeSizeString(zone.zoneDesc, 256); // position
            objectWriter.WriteFixeSizeString("TODO: party", 32); // TODO: party
            objectWriter.WriteFixeSizeString("", 32); // no reference 

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
}
