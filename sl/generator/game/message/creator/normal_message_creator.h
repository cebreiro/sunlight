#pragma once

namespace sunlight
{
    class GamePlayer;
}

namespace sunlight
{
    class NormalMessageCreator
    {
    public:
        NormalMessageCreator() = delete;

        static auto CreateChangeRoom(int32_t destStageId, int32_t destX, int32_t destY) -> Buffer;

        static auto CreateItemStreetVendorCreateFail(int32_t groupId) -> Buffer;
    };
}
