#pragma once
#include "sl/generator/game/contents/profile/player_profile_setting.h"
#include "sl/generator/game/contents/profile/player_profile_introduction.h"
#include "sl/generator/game/component/game_component.h"

namespace sunlight
{
    class PlayerProfileComponent final : public GameComponent
    {
    public:
        bool IsConfigured(PlayerProfileSetting setting) const;
        bool IsLoadPendingIntroduction() const;

        void Configure(PlayerProfileSetting setting, bool turnOn);

        void AddIntroductionLoadingWaiter(const std::string& playerName);
        void ClearIntroductionLoadingWaiters();

        auto GetBitmask() const -> int8_t;
        auto GetIntroduction() const -> const std::optional<PlayerProfileIntroduction>&;
        auto GetIntroductionLoadingWaiters() const -> const std::vector<std::string>&;

        void SetBitmask(int8_t value);
        void SetLoadPendingIntroduction(bool value);
        void SetIntroduction(PlayerProfileIntroduction introduction);

    private:
        int8_t _bitmask = 0;
        bool _loadPendingIntroduction = false;

        std::optional<PlayerProfileIntroduction> _introduction = std::nullopt;
        std::vector<std::string> _introductionLoadWaitPlayerNames;
    };
}
