#pragma once

namespace sunlight
{
    class SlPacketReader;

    class GameGroup;
    class GamePlayer;

    class PlayerGroupSystem;
}

namespace sunlight
{
    class IGroupMessageHandler
    {
    public:
        virtual ~IGroupMessageHandler() = default;

        virtual void Handle(PlayerGroupSystem& system, GameGroup& group, GamePlayer& player, SlPacketReader& reader) const = 0;
    };
}
