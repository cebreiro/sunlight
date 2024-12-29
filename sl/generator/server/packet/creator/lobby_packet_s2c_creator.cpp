#include "lobby_packet_s2c_creator.h"

#include "sl/generator/server/packet/lobby_packet_s2c.h"
#include "sl/generator/server/packet/dto/lobby_s2c_endpoint.h"
#include "sl/generator/server/packet/io/sl_packet_writer.h"
#include "sl/generator/service/database/dto/lobby_character.h"
#include "sl/generator/service/gamedata/item/item_data_provider.h"

namespace sunlight
{
    auto LobbyPacketS2CCreator::CreateClientVersionCheckResult(bool success) -> Buffer
    {
        SlPacketWriter writer;

        writer.Write(LobbyPacketS2C::ClientVersion);
        writer.Write<int32_t>(!success);

        return writer.Flush();
    }

    auto LobbyPacketS2CCreator::CreateAuthenticationResult(bool success, const std::string& unk) -> Buffer
    {
        SlPacketWriter writer;

        writer.Write(LobbyPacketS2C::Authentication);
        writer.Write<int32_t>(!success);
        writer.WriteString(unk);

        return writer.Flush();
    }

    auto LobbyPacketS2CCreator::CreateCharacterList(const ItemDataProvider& itemDataProvider, const std::vector<db::dto::LobbyCharacter>& characters) -> Buffer
    {
        SlPacketWriter writer;

        writer.Write(LobbyPacketS2C::CharacterList);
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(static_cast<int32_t>(std::ssize(characters)));
        writer.Write<int32_t>(0);

        const auto getModelId = [&itemDataProvider](int32_t itemId) -> int32_t
            {
                if (itemId == 0)
                {
                    return 0;
                }

                const ItemData* itemData = itemDataProvider.Find(itemId);
                if (!itemData)
                {
                    return 0;
                }

                return itemData->GetModelId();
            };

        for (const db::dto::LobbyCharacter& character : characters)
        {
            {
                PacketWriter objectWriter;

                const int32_t hatModelId = getModelId(character.hatItemId);

                objectWriter.Write<int32_t>(hatModelId != 0 ? character.hair : hatModelId);
                objectWriter.Write<int32_t>(character.hairColor);
                objectWriter.Write<int32_t>(character.skinColor);
                objectWriter.Write<int32_t>(character.face);
                objectWriter.Write<int32_t>(0);
                objectWriter.Write<int32_t>(0);
                objectWriter.Write<int32_t>(hatModelId != 0 ? hatModelId : character.hair);
                objectWriter.Write<int32_t>(0);
                objectWriter.Write<int32_t>(0);

                objectWriter.Write<int32_t>(character.jacketItemId);
                objectWriter.Write<int32_t>(0);
                objectWriter.Write<int32_t>(0);

                objectWriter.Write<int32_t>(getModelId(character.glovesItemId));
                objectWriter.Write<int32_t>(0);
                objectWriter.Write<int32_t>(0);

                objectWriter.Write<int32_t>(character.pantsItemId);
                objectWriter.Write<int32_t>(0);
                objectWriter.Write<int32_t>(0);

                objectWriter.Write<int32_t>(character.shoesItemId);
                objectWriter.Write<int32_t>(0);
                objectWriter.Write<int32_t>(0);

                objectWriter.Write<int32_t>(0);
                objectWriter.Write<int32_t>(0);
                objectWriter.Write<int32_t>(0);

                writer.WriteObject(objectWriter);
            }
            {
                PacketWriter objectWriter;

                {
                    std::array<char, 32> name = {};
                    const int64_t count = std::min(std::ssize(character.name), std::ssize(name));

                    std::copy_n(character.name.begin(), count, name.begin());

                    objectWriter.WriteBuffer(name);
                } // ~32

                objectWriter.WriteZeroBytes(32); // ~64;

                {
                    std::array<char, 32> zone = {};
                    const std::string str = std::to_string(character.zone);

                    const int64_t count = std::min(std::ssize(str), std::ssize(zone));

                    std::copy_n(str.begin(), count, zone.begin());

                    objectWriter.WriteBuffer(zone);
                } // 96

                objectWriter.WriteZeroBytes(32); // ~128;

                objectWriter.WriteZeroBytes(28);
                objectWriter.Write<int32_t>(character.job2 != 0 ? character.job2 : character.job1); // ~192

                objectWriter.Write<int8_t>(character.arms);
                objectWriter.Write<int32_t>(0);
                objectWriter.Write<int8_t>(character.slot);
                objectWriter.Write<int32_t>(0);
                objectWriter.Write<int8_t>(true);
                objectWriter.WriteZeroBytes(5);

                objectWriter.Write<int32_t>(0);
                objectWriter.Write<int32_t>(0);

                objectWriter.WriteZeroBytes(20);
                objectWriter.WriteZeroBytes(20);
                objectWriter.WriteZeroBytes(24);

                objectWriter.Write<int32_t>(character.gender);
                objectWriter.Write<int32_t>(character.level);

                writer.WriteObject(objectWriter);
            }
        }

        return writer.Flush();
    }

    auto LobbyPacketS2CCreator::CreateCharacterNameCheckResult(bool success, const std::string& name) -> Buffer
    {
        SlPacketWriter writer;

        writer.Write(LobbyPacketS2C::CharacterNameCheck);
        writer.Write<int32_t>(!success);
        writer.WriteString(name);

        return writer.Flush();
    }

    auto LobbyPacketS2CCreator::CreateCharacterCreateResult(bool success) -> Buffer
    {
        SlPacketWriter writer;

        writer.Write(LobbyPacketS2C::CharacterCreate);
        writer.Write<int32_t>(!success);

        return writer.Flush();
    }

    auto LobbyPacketS2CCreator::CreateCharacterDeleteResult(bool success) -> Buffer
    {
        SlPacketWriter writer;

        writer.Write(LobbyPacketS2C::CharacterDelete);
        writer.Write<int32_t>(!success);

        return writer.Flush();
    }

    auto LobbyPacketS2CCreator::CreateCharacterSelectSuccess(uint32_t auth, const std::string& playerName, const LobbyS2CEndpoint& endpoint) -> Buffer
    {
        constexpr bool success = true;

        SlPacketWriter writer;

        writer.Write(LobbyPacketS2C::CharacterSelect);
        writer.Write<int32_t>(!success);
        writer.Write<uint32_t>(auth);
        writer.WriteString(playerName);
        writer.WriteObject(endpoint.Serialize());

        return writer.Flush();
    }

    auto LobbyPacketS2CCreator::CreateCharacterSelectFail() -> Buffer
    {
        constexpr bool success = false;

        SlPacketWriter writer;

        writer.Write(LobbyPacketS2C::CharacterSelect);
        writer.Write<int32_t>(!success);

        return writer.Flush();
    }
}
