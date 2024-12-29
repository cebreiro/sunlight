#include "game_group.h"

#include "sl/generator/game/entity/game_player.h"

namespace sunlight
{
    GameGroup::GameGroup(int32_t id, GameGroupType type, GamePlayer& host)
        : _id(id)
        , _type(type)
        , _host(host)
    {
    }

    bool GameGroup::IsHost(const GamePlayer& player) const
    {
        return &_host == &player;
    }

    bool GameGroup::IsGuest(const GamePlayer& player) const
    {
        return std::ranges::contains(_guests, &player);
    }

    bool GameGroup::HasGuest() const
    {
        return !_guests.empty();
    }

    void GameGroup::AddGuest(GamePlayer& player)
    {
        assert(!std::ranges::contains(_guests, &player));

        _guests.emplace_back(&player);
    }

    bool GameGroup::RemoveGuest(const GamePlayer& player)
    {
        const auto iter = std::ranges::find(_guests, &player);
        if (iter != _guests.end())
        {
            _tradeConfirmPlayers.erase(player.GetCId());

            _guests.erase(iter);

            return true;
        }

        return false;
    }

    void GameGroup::AddTradeConfirmPlayer(const GamePlayer& player)
    {
        if (!IsHost(player) && !IsGuest(player))
        {
            assert(false);

            return;
        }

        _tradeConfirmPlayers.emplace(player.GetCId());
    }

    void GameGroup::Broadcast(Buffer buffer, std::optional<game_entity_id_type> exceptEntityId)
    {
        for (GamePlayer& guest : _guests | notnull::reference)
        {
            if (exceptEntityId.has_value() && *exceptEntityId == guest.GetId())
            {
                continue;
            }

            guest.Send(buffer.DeepCopy());
        }

        if (!exceptEntityId.has_value() || *exceptEntityId != _host.GetId())
        {
            _host.Send(std::move(buffer));
        }
    }

    auto GameGroup::GetId() const -> int32_t
    {
        return _id;
    }

    auto GameGroup::GetType() const -> GameGroupType
    {
        return _type;
    }

    auto GameGroup::GetMemberCount() const -> int64_t
    {
        return 1 + std::ssize(_guests);
    }

    auto GameGroup::GetHost() const -> GamePlayer&
    {
        return _host;
    }

    auto GameGroup::GetGuests() const -> const std::vector<PtrNotNull<GamePlayer>>&
    {
        return _guests;
    }

    auto GameGroup::GetTradeConfirmPlayerCount() const -> int64_t
    {
        return std::ssize(_tradeConfirmPlayers);
    }
}
