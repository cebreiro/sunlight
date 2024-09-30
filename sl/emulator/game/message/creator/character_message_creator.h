#pragma once

namespace sunlight::sox
{
    struct Zone;
}

namespace sunlight
{
    class GamePlayer;
}

namespace sunlight
{
    class CharacterMessageCreator
    {
    public:
        CharacterMessageCreator() = delete;

        static auto CreateProfile(const GamePlayer& targetPlayer, const sox::Zone& zone) -> Buffer;
        static auto CreateProfileFail(const std::string& targetPlayerName) -> Buffer;
    };
}
