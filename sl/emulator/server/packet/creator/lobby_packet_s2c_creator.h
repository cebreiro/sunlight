#pragma once

namespace sunlight::dto
{
    struct LobbyCharacter;
}

namespace sunlight
{
    class LobbyPacketS2CCreator final : public IService
    {
    public:
        explicit LobbyPacketS2CCreator(const ServiceLocator& serviceLocator);

        auto GetName() const -> std::string_view override;

    public:
        auto CreateClientVersionCheckResult(bool success) const -> Buffer;
        auto CreateAuthenticationResult(bool success, const std::string& unk) const -> Buffer;

        auto CreateCharacterList(const std::vector<dto::LobbyCharacter>& characters) const -> Buffer;
        auto CreateCharacterNameCheckResult(bool success, const std::string& name) const -> Buffer;
        auto CreateCharacterCreateResult(bool success) -> Buffer;
        auto CreateCharacterDeleteResult(bool success) -> Buffer;

    private:
        const ServiceLocator& _serviceLocator;
    };
}
