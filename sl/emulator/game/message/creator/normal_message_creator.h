#pragma once

namespace sunlight
{
    class NormalMessageCreator
    {
    public:
        NormalMessageCreator() = delete;

        static auto CreateChangeRoom(int32_t destStageId, int32_t destX, int32_t destY) -> Buffer;
    };
}
