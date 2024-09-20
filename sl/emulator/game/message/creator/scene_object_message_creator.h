#pragma once

namespace sunlight
{
    class GameEntity;
    class GamePlayer;
    class GameItem;
}

namespace sunlight
{
    class SceneObjectPacketCreator
    {
    public:
        SceneObjectPacketCreator() = delete;

        static auto CreateState(const GameEntity& entity) -> Buffer;

        static auto CreateInformation(const GamePlayer& player, bool showSpawnEffect) -> Buffer;
        static auto CreateInformation(const GameItem& item) -> Buffer;

        static auto CreateItemDisplay(const GameItem& item, int64_t characterId) -> Buffer;
        static auto CreateItemSpawn(const GameItem& item, int64_t characterId, Eigen::Vector2f originPos) -> Buffer;
    };
}
