#include "party.h"

namespace sunlight
{
    Party::Party(int64_t partyId, std::string partyName, int64_t leaderId, int64_t memberId)
        : _id(partyId)
        , _name(std::move(partyName))
        , _leaderId(leaderId)
    {
        _members.reserve(8);

        _members.push_back(leaderId);
        _members.push_back(memberId);
    }

    bool Party::IsFull() const
    {
        return std::ssize(_members) >= 8;
    }

    bool Party::Contains(int64_t playerId)
    {
        return std::ranges::contains(_members, playerId);
    }

    void Party::Add(int64_t playerId)
    {
        assert(!Contains(playerId));

        _members.push_back(playerId);
    }

    bool Party::IsPublic() const
    {
        return _isPublic;
    }

    bool Party::IsSetGoldDistribution() const
    {
        return _goldDistribution;
    }

    bool Party::IsSetItemDistribution() const
    {
        return _itemDistribution;
    }

    auto Party::GetId() const -> int64_t
    {
        return _id;
    }

    auto Party::GetName() const -> const std::string&
    {
        return _name;
    }

    auto Party::GetLeaderId() const -> int64_t
    {
        return _leaderId;
    }

    auto Party::GetMembers() const -> const std::vector<int64_t>&
    {
        return _members;
    }
}
