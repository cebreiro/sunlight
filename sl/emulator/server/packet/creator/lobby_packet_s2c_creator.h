#pragma once

namespace sunlight::dto
{
    struct LobbyCharacter;
}

namespace sunlight
{
    class ItemDataProvider;
    struct LobbyS2CEndpoint;
}

namespace sunlight
{
    class LobbyPacketS2CCreator
    {
    public:
        LobbyPacketS2CCreator() = delete;
    
        static auto CreateClientVersionCheckResult(bool success)  -> Buffer;
        static auto CreateAuthenticationResult(bool success, const std::string& unk)  -> Buffer;

        static auto CreateCharacterList(const ItemDataProvider& itemDataProvider, const std::vector<dto::LobbyCharacter>& characters)  -> Buffer;
        static auto CreateCharacterNameCheckResult(bool success, const std::string& name)  -> Buffer;
        static auto CreateCharacterCreateResult(bool success) -> Buffer;
        static auto CreateCharacterDeleteResult(bool success) -> Buffer;
        static auto CreateCharacterSelectSuccess(int32_t unused, const std::string& key, const LobbyS2CEndpoint& endpoint) -> Buffer;
        static auto CreateCharacterSelectFail() -> Buffer;
    };
}
