#pragma once

namespace sunlight
{
    class CommunityPlayer
    {
    public:
        CommunityPlayer() = default;
        CommunityPlayer(int64_t cid, std::string name);

        bool HasParty() const;

        auto GetId() const -> int64_t;
        auto GetName() const -> const std::string&;
        auto GetZoneId() const -> int32_t;
        auto GetPartyId() const -> int64_t;
        auto GetJobId() const -> int32_t;
        auto GetJobLevel() const -> int8_t;
        auto GetCharacterLevel() const -> int8_t;
        auto GetHP() const -> int32_t;
        auto GetMaxHP() const -> int32_t;

        void SetZoneId(int32_t zoneId);
        void SetPartyId(std::optional<int64_t> id);

        void SetJobId(int32_t jobId);
        void SetJobLevel(int8_t jobLevel);
        void SetCharacterLevel(int8_t level);
        void SetHP(int32_t hp);
        void SetMaxHP(int32_t maxHP);

    private:
        int64_t _cid = 0;
        std::string _name;
        int32_t _zoneId = 0;

        std::optional<int64_t> _partyId = std::nullopt;

        int32_t _jobId = 0;
        int8_t _jobLevel = 0;
        int8_t _characterLevel = 0;

        int32_t _hp = 0;
        int32_t _maxHP = 0;
    };
}
