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
    };
}
