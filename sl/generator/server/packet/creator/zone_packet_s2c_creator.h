#pragma once

namespace sunlight
{
    struct ClientMovement;

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
        static auto CreateObjectLeave(int32_t id) -> Buffer;

        static auto CreateObjectMove(const GameEntity& entity) -> Buffer;
        static auto CreateObjectMove(const GameEntity& entity, const ClientMovement& movement) -> Buffer;

        static auto CreateObjectForceMove(const GameEntity& entity) -> Buffer;
        static auto CreateObjectRoomChange(int32_t roomId) -> Buffer;
        static auto CreateObjectVisibleRange(float range) -> Buffer;

        static auto CreateZoneChange(const std::string& address, int32_t port, uint32_t auth) -> Buffer;
    };
}
