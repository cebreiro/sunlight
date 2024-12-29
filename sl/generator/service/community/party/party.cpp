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

    void Party::Remove(int64_t playerId)
    {
        assert(Contains(playerId));

        if (const auto iter = std::ranges::find(_members, playerId);
            iter != _members.end())
        {
            _members.erase(iter);
        }
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

    void Party::SetLeaderId(int64_t newLeaderId)
    {
        if (_leaderId == newLeaderId)
        {
            return;
        }

        const auto iter = std::ranges::find(_members, newLeaderId);
        if (iter == _members.end())
        {
            assert(false);

            return;
        }

        std::iter_swap(_members.begin(), iter);

        _leaderId = newLeaderId;
    }

    void Party::SetPublic(bool value)
    {
        _isPublic = value;
    }

    void Party::SetGoldDistribution(bool value)
    {
        _goldDistribution = value;
    }

    void Party::SetItemDistribution(bool value)
    {
        _itemDistribution = value;
    }
}
