#pragma once
#include "sl/generator/service/community/player/community_player.h"

namespace sunlight
{
    class CommunityPlayerStorage
    {
    public:
        CommunityPlayerStorage() = default;

        auto Add(int64_t id, const std::string& name) -> CommunityPlayer*;
        bool Remove(int64_t id);

        auto Find(int64_t id) -> CommunityPlayer*;
        auto Find(int64_t id) const -> const CommunityPlayer*;
        auto FindByName(const std::string& name) -> CommunityPlayer*;
        auto FindByName(const std::string& name) const -> const CommunityPlayer*;

        auto GetCount() const -> int64_t;

    private:
        std::unordered_map<int64_t, CommunityPlayer> _players;
        std::unordered_map<std::string, PtrNotNull<CommunityPlayer>> _playerNameIndex;
    };
}
