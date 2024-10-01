#pragma once

namespace sunlight::db::dto
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

        static auto CreateCharacterList(const ItemDataProvider& itemDataProvider, const std::vector<db::dto::LobbyCharacter>& characters)  -> Buffer;
        static auto CreateCharacterNameCheckResult(bool success, const std::string& name)  -> Buffer;
        static auto CreateCharacterCreateResult(bool success) -> Buffer;
        static auto CreateCharacterDeleteResult(bool success) -> Buffer;
        static auto CreateCharacterSelectSuccess(uint32_t auth, const std::string& playerName, const LobbyS2CEndpoint& endpoint) -> Buffer;
        static auto CreateCharacterSelectFail() -> Buffer;
    };
}
