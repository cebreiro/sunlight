#pragma once

namespace sunlight
{
    class GameEntity;
    class GamePlayer;
}

namespace sunlight
{
    class ZonePacketS2CCreator
    {
    public:
        ZonePacketS2CCreator() = delete;

        static auto CreateLoginAccept(const GamePlayer& player, int32_t stageId) -> Buffer;
        static auto CreateLoginReject(const std::string& reason) -> Buffer;

        static auto CreateObjectLeave(const GameEntity& entity) -> Buffer;
        static auto CreateObjectMove(const GameEntity& entity) -> Buffer;
        static auto CreateObjectForceMove(const GameEntity& entity) -> Buffer;
        static auto CreateObjectRoomChange(int32_t roomId) -> Buffer;
        static auto CreateObjectVisibleRange(float range) -> Buffer;
    };
}
