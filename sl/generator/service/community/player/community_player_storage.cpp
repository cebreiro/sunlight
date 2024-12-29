#include "community_player_storage.h"

namespace sunlight
{
    auto CommunityPlayerStorage::Add(int64_t id, const std::string& name) -> CommunityPlayer*
    {
        const auto [iter, inserted] = _players.try_emplace(id, CommunityPlayer(id, name));
        if (!inserted)
        {
            return nullptr;
        }

        _playerNameIndex[name] = &iter->second;

        return &iter->second;
    }

    bool CommunityPlayerStorage::Remove(int64_t id)
    {
        const auto iter = _players.find(id);
        if (iter == _players.end())
        {
            return false;
        }

        _playerNameIndex.erase(iter->second.GetName());
        _players.erase(iter);

        return true;
    }

    auto CommunityPlayerStorage::Find(int64_t id) -> CommunityPlayer*
    {
        const auto iter = _players.find(id);

        return iter != _players.end() ? &iter->second : nullptr;
    }

    auto CommunityPlayerStorage::Find(int64_t id) const -> const CommunityPlayer*
    {
        const auto iter = _players.find(id);

        return iter != _players.end() ? &iter->second : nullptr;
    }

    auto CommunityPlayerStorage::FindByName(const std::string& name) -> CommunityPlayer*
    {
        const auto iter = _playerNameIndex.find(name);

        return iter != _playerNameIndex.end() ? iter->second : nullptr;
    }

    auto CommunityPlayerStorage::FindByName(const std::string& name) const -> const CommunityPlayer*
    {
        const auto iter = _playerNameIndex.find(name);

        return iter != _playerNameIndex.end() ? iter->second : nullptr;
    }

    auto CommunityPlayerStorage::GetCount() const -> int64_t
    {
        return std::ssize(_players);
    }
}
