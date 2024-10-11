#include "community_player.h"

namespace sunlight
{
    CommunityPlayer::CommunityPlayer(int64_t cid, std::string name)
        : _cid(cid)
        , _name(std::move(name))
    {
    }

    bool CommunityPlayer::HasParty() const
    {
        return _partyId.has_value();
    }

    auto CommunityPlayer::GetId() const -> int64_t
    {
        return _cid;
    }

    auto CommunityPlayer::GetName() const -> const std::string&
    {
        return _name;
    }

    auto CommunityPlayer::GetZoneId() const -> int32_t
    {
        return _zoneId;
    }

    auto CommunityPlayer::GetPartyId() const -> int64_t
    {
        assert(HasParty());

        return *_partyId;
    }

    auto CommunityPlayer::GetJobId() const -> int32_t
    {
        return _jobId;
    }

    auto CommunityPlayer::GetJobLevel() const -> int8_t
    {
        return _jobLevel;
    }

    auto CommunityPlayer::GetCharacterLevel() const -> int8_t
    {
        return _characterLevel;
    }

    auto CommunityPlayer::GetHP() const -> int32_t
    {
        return _hp;
    }

    auto CommunityPlayer::GetMaxHP() const -> int32_t
    {
        return _maxHP;
    }

    void CommunityPlayer::SetZoneId(int32_t zoneId)
    {
        _zoneId = zoneId;
    }

    void CommunityPlayer::SetPartyId(std::optional<int64_t> id)
    {
        _partyId = id;
    }

    void CommunityPlayer::SetJobId(int32_t jobId)
    {
        _jobId = jobId;
    }

    void CommunityPlayer::SetJobLevel(int8_t jobLevel)
    {
        _jobLevel = jobLevel;
    }

    void CommunityPlayer::SetCharacterLevel(int8_t level)
    {
        _characterLevel = level;
    }

    void CommunityPlayer::SetHP(int32_t hp)
    {
        _hp = hp;
    }

    void CommunityPlayer::SetMaxHP(int32_t maxHP)
    {
        _maxHP = maxHP;
    }
}
