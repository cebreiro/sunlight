#pragma once

namespace sunlight
{
    class GameItem;
}

namespace sunlight
{
    class SceneObjectPacketCreator
    {
    public:
        SceneObjectPacketCreator() = delete;

        static auto CreateInformation(const GameItem& item) -> Buffer;
        static auto CreateItemDisplay(const GameItem& item, bool showOwnership, bool isMine) -> Buffer;
        static auto CreateItemSpawn(const GameItem& item, bool showOwnership, bool isMine, Eigen::Vector2f originPos) -> Buffer;
    };
}
