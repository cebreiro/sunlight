#include "login_packet_s2c_creator.h"

#include <WS2tcpip.h>

#include "sl/generator/server/packet/login_packet_s2c.h"

namespace
{
    auto ConvertAddressStringToInt(const std::string& address) -> int32_t
    {
        SOCKADDR_IN sa = {};

        if (!::inet_pton(AF_INET, address.data(), &sa.sin_addr.s_addr))
        {
            assert(false);

            return -1;
        }

        return sa.sin_addr.s_addr;
    }
}

namespace sunlight
{
    auto LoginPacketS2CCreator::CreateHello(uint32_t key1, uint32_t key2) -> Buffer
    {
        PacketWriter writer;

        writer.Write(LoginPacketS2C::Hello);
        writer.Write<int32_t>(static_cast<int32_t>(key1));
        writer.Write<int32_t>(static_cast<int32_t>(key2));

        return writer.Flush();
    }

    auto LoginPacketS2CCreator::CreateServerList(const std::vector<std::pair<int8_t, std::string>>& list) -> Buffer
    {
        PacketWriter writer;

        writer.Write(LoginPacketS2C::WorldList);

        const int8_t count = static_cast<int8_t>(std::ssize(list));

        writer.Write(count);

        for (const auto& [serverId, address] : list)
        {
            writer.Write(serverId);
            writer.Write(ConvertAddressStringToInt(address)); // maybe lobby
            writer.Write(ConvertAddressStringToInt(address)); // maybe login(return address)
        }

        return writer.Flush();
    }

    auto LoginPacketS2CCreator::CreateLobbyPass(int32_t key1, int32_t key2, int8_t sid) -> Buffer
    {
        PacketWriter writer;

        writer.Write(LoginPacketS2C::LobbyPass);
        writer.Write<int32_t>(key1);
        writer.Write<int32_t>(key2);
        writer.Write<int8_t>(sid);

        return writer.Flush();
    }

    auto LoginPacketS2CCreator::CreateLoginFail(LoginResult result) -> Buffer
    {
        PacketWriter writer;

        writer.Write(LoginPacketS2C::LoginFail);
        writer.Write(result);

        return writer.Flush();
    }
}
