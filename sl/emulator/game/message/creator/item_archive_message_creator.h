#pragma once

namespace sunlight
{
    class GamePlayer;
}

namespace sunlight
{
    class ItemArchiveMessageCreator
    {
    public:
        ItemArchiveMessageCreator() = delete;

        static auto CreateInit(const GamePlayer& player) -> Buffer;
    };
}
