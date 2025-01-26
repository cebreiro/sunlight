#include "player_index_system.h"

#include "sl/generator/game/entity/game_player.h"

namespace sunlight
{
    auto PlayerIndexSystem::GetName() const -> std::string_view
    {
        return "player_index_system";
    }

    auto PlayerIndexSystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<PlayerIndexSystem>();
    }

    void PlayerIndexSystem::Visit(const std::function<void(GamePlayer&)>& function)
    {
        for (GamePlayer* player : _cidIndex | std::views::values)
        {
            function(*player);
        }
    }

    void PlayerIndexSystem::OnStageEnter(GamePlayer& player)
    {
        assert(!_cidIndex.contains(player.GetCId()));
        assert(!_nameIndex.contains(player.GetName()));

        _cidIndex[player.GetCId()] = &player;
        _nameIndex[player.GetName()] = &player;
    }

    void PlayerIndexSystem::OnStageExit(GamePlayer& player)
    {
        _cidIndex.erase(player.GetCId());
        _nameIndex.erase(player.GetName());
    }

    auto PlayerIndexSystem::FindByCId(int64_t cid) -> GamePlayer*
    {
        const auto iter = _cidIndex.find(cid);

        return iter != _cidIndex.end() ? iter->second : nullptr;
    }

    auto PlayerIndexSystem::FindByCId(int64_t cid) const -> const GamePlayer*
    {
        const auto iter = _cidIndex.find(cid);

        return iter != _cidIndex.end() ? iter->second : nullptr;
    }

    auto PlayerIndexSystem::FindByName(const std::string& name) -> GamePlayer*
    {
        const auto iter = _nameIndex.find(name);

        return iter != _nameIndex.end() ? iter->second : nullptr;
    }

    auto PlayerIndexSystem::FindByName(const std::string& name) const -> const GamePlayer*
    {
        const auto iter = _nameIndex.find(name);

        return iter != _nameIndex.end() ? iter->second : nullptr;
    }

    auto PlayerIndexSystem::GetPlayerCount() const -> int64_t
    {
        return std::ssize(_cidIndex);
    }
}
