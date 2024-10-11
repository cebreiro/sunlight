#pragma once

namespace sunlight
{
    class Party
    {
    public:
        Party() = default;
        Party(int64_t partyId, std::string partyName, int64_t leaderId, int64_t memberId);

        bool IsPublic() const;
        bool IsSetGoldDistribution() const;
        bool IsSetItemDistribution() const;

        auto GetId() const -> int64_t;
        auto GetName() const -> const std::string&;
        auto GetLeaderId() const -> int64_t;
        auto GetMembers() const -> const std::vector<int64_t>&;

    private:
        int64_t _id = 0;
        std::string _name;

        int64_t _leaderId = 0;
        std::vector<int64_t> _members;

        bool _isPublic = false;
        bool _goldDistribution = false;
        bool _itemDistribution = false;
    };
}
