#pragma once

namespace sunlight
{
    class GamePlayer;
}

namespace sunlight
{
    class ChatMessageCreator
    {
    public:
        ChatMessageCreator() = delete;

        static auto CreateNormalChat(const GamePlayer& player, const std::string& message) -> Buffer;
        static auto CreateShoutChat(const std::string& sender, const std::string& message) -> Buffer;
        static auto CreateTradeChat(const std::string& sender, const std::string& message) -> Buffer;
        static auto CreateEchoChat(const std::string& sender, const std::string& message) -> Buffer;

        static auto CreatePartyChat(const std::string& partyName, const std::string& sender, const std::string& message) -> Buffer;
        static auto CreateWhisperChat(const std::string& sender, const std::string& message) -> Buffer;

        static auto CreateServerMessage(const std::string& message) -> Buffer;
        static auto CreateServerNotice(const std::string& message) -> Buffer;
    };
}
