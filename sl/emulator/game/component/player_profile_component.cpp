#include "player_profile_component.h"

namespace sunlight
{
    bool PlayerProfileComponent::IsConfigured(PlayerProfileSetting setting) const
    {
        return _bitmask & static_cast<int8_t>(setting);
    }

    bool PlayerProfileComponent::IsLoadPendingIntroduction() const
    {
        return _loadPendingIntroduction;
    }

    void PlayerProfileComponent::Configure(PlayerProfileSetting setting, bool turnOn)
    {
        if (turnOn)
        {
            _bitmask |= static_cast<int8_t>(setting);
        }
        else
        {
            _bitmask &= (int8_t{ -1 } ^ static_cast<int8_t>(setting));
        }
    }

    void PlayerProfileComponent::AddIntroductionLoadingWaiter(const std::string& playerName)
    {
        _introductionLoadWaitPlayerNames.push_back(playerName);
    }

    void PlayerProfileComponent::ClearIntroductionLoadingWaiters()
    {
        _introductionLoadWaitPlayerNames.clear();
    }

    auto PlayerProfileComponent::GetBitmask() const -> int8_t
    {
        return _bitmask;
    }

    auto PlayerProfileComponent::GetIntroduction() const -> const std::optional<PlayerProfileIntroduction>&
    {
        return _introduction;
    }

    auto PlayerProfileComponent::GetIntroductionLoadingWaiters() const -> const std::vector<std::string>&
    {
        return _introductionLoadWaitPlayerNames;
    }

    void PlayerProfileComponent::SetBitmask(int8_t value)
    {
        _bitmask = value;
    }

    void PlayerProfileComponent::SetLoadPendingIntroduction(bool value)
    {
        _loadPendingIntroduction = value;
    }

    void PlayerProfileComponent::SetIntroduction(PlayerProfileIntroduction introduction)
    {
        _introduction = std::move(introduction);
    }
}
