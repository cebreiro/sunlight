#pragma once

namespace sunlight
{
    class GamePlayer;
}

namespace sunlight
{
    class GamePlayerMessageCreator
    {
    public:
        GamePlayerMessageCreator() = delete;

        static auto CreateAllState(const GamePlayer& player) -> Buffer;
        static auto CreateRemotePlayerState(const GamePlayer& player) -> Buffer;

        static auto CreateCharacterExpGain(const GamePlayer& player, int32_t value) -> Buffer;
        static auto CreateCharacterLevelUp(const GamePlayer& player) -> Buffer;

        static auto CreatePlayerGainGroupItem(const GamePlayer& player, int32_t x, int32_t y) -> Buffer;
    };
}
