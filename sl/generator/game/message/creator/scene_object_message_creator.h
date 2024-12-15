#pragma once

namespace sunlight
{
    struct GameEntityState;

    class GameEntity;
    class GamePlayer;
    class GameNPC;
    class GameItem;
    class GameStoredItem;
    class GameMonster;
}

namespace sunlight
{
    class SceneObjectPacketCreator
    {
    public:
        SceneObjectPacketCreator() = delete;

        static auto CreateState(const GameEntity& entity) -> Buffer;
        static auto CreateState(const GameEntity& entity, const GameEntityState& state) -> Buffer;

        static auto CreateInformation(const GamePlayer& player, bool showSpawnEffect) -> Buffer;
        static auto CreateInformation(const GameNPC& npc) -> Buffer;
        static auto CreateInformation(const GameItem& item) -> Buffer;
        static auto CreateInformation(const GameStoredItem& item) -> Buffer;
        static auto CreateInformation(const GameMonster& monster, bool showSpawnEffect) -> Buffer;

        static auto CreateItemDisplay(const GameItem& item, int64_t characterId) -> Buffer;
        static auto CreateItemSpawn(const GameItem& item, int64_t characterId, Eigen::Vector2f originPos) -> Buffer;

        static auto CreateItemDisplay(const GameStoredItem& item) -> Buffer;
    };
}
