#pragma once
#include "sl/generator/game/component/game_component.h"

namespace sunlight
{
    class PlayerPartyComponent final : public GameComponent
    {
    public:
        PlayerPartyComponent();

        bool HasParty() const;

        void Clear();

        void AddMemberId(int64_t id);
        void RemoveMemberId(int64_t id);

        auto GetPartyName() const -> const std::string&;
        auto GetMemberIds() const -> std::span<const int64_t>;

        void SetHasParty(bool value);
        void SetPartyName(const std::string& partyName);

    private:
        bool _hasParty = false;
        std::string _partyName;

        std::vector<int64_t> _memberIds;
    };
}
