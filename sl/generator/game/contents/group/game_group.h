#pragma once
#include "sl/generator/game/contents/group/game_group_type.h"
#include "sl/generator/game/entity/game_entity_id_type.h"

namespace sunlight
{
    class GamePlayer;
}

namespace sunlight
{
    class GameGroup
    {
    public:
        GameGroup() = delete;
        GameGroup(int32_t id, GameGroupType type, GamePlayer& host);

        bool IsHost(const GamePlayer& player) const;
        bool IsGuest(const GamePlayer& player) const;

        bool HasGuest() const;

        void AddGuest(GamePlayer& player);
        bool RemoveGuest(const GamePlayer& player);

        void AddTradeConfirmPlayer(const GamePlayer& player);

        void Broadcast(Buffer buffer, std::optional<game_entity_id_type> exceptEntityId);

        auto GetId() const -> int32_t;
        auto GetType() const -> GameGroupType;
        auto GetMemberCount() const -> int64_t;
        auto GetHost() const -> GamePlayer&;
        auto GetGuests() const -> const std::vector<PtrNotNull<GamePlayer>>&;

        auto GetTradeConfirmPlayerCount() const -> int64_t;

    private:
        int32_t _id = 0;
        GameGroupType _type = GameGroupType::Null;
        GamePlayer& _host;

        std::vector<PtrNotNull<GamePlayer>> _guests;

        std::unordered_set<int64_t> _tradeConfirmPlayers;
    };
}
