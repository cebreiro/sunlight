#include "player_party_component.h"

namespace sunlight
{
    PlayerPartyComponent::PlayerPartyComponent()
    {
        _memberIds.reserve(8);
    }

    bool PlayerPartyComponent::HasParty() const
    {
        return _hasParty;
    }

    void PlayerPartyComponent::Clear()
    {
        _hasParty = false;
        _partyName.clear();
        _memberIds.clear();
    }

    void PlayerPartyComponent::AddMemberId(int64_t id)
    {
        _memberIds.push_back(id);
    }

    void PlayerPartyComponent::RemoveMemberId(int64_t id)
    {
        if (const auto iter = std::ranges::find(_memberIds, id);
            iter != _memberIds.end())
        {
            _memberIds.erase(iter);
        }
    }

    void PlayerPartyComponent::SetHasParty(bool value)
    {
        _hasParty = value;
    }

    void PlayerPartyComponent::SetPartyName(const std::string& partyName)
    {
        _partyName = partyName;
    }
}
