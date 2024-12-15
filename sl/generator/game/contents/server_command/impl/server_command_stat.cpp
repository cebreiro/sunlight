#include "server_command_stat.h"

#include "sl/generator/game/component/player_stat_component.h"
#include "sl/generator/game/contents/stat/player_stat_type.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/system/player_stat_system.h"
#include "sl/generator/game/system/server_command_system.h"

namespace sunlight
{
    ServerCommandStatHPSet::ServerCommandStatHPSet(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandStatHPSet::GetName() const -> std::string_view
    {
        return "stat_hp_set";
    }

    auto ServerCommandStatHPSet::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandStatHPSet::Execute(GamePlayer& player, int32_t value) const
    {
        _system.Get<PlayerStatSystem>().SetHP(player, value, HPChangeFloaterType::None);

        return true;
    }

    ServerCommandStatSPSet::ServerCommandStatSPSet(ServerCommandSystem& system)
        : _system(system)
    {
    }

    auto ServerCommandStatSPSet::GetName() const -> std::string_view
    {
        return "stat_sp_set";
    }

    auto ServerCommandStatSPSet::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandStatSPSet::Execute(GamePlayer& player, int32_t value) const
    {
        _system.Get<PlayerStatSystem>().SetSP(player, value, SPChangeFloaterType::None);

        return true;
    }

    auto ServerCommandStatShow::GetName() const -> std::string_view
    {
        return "stat_show";
    }

    auto ServerCommandStatShow::GetRequiredGmLevel() const -> int8_t
    {
        return 0;
    }

    bool ServerCommandStatShow::Execute(GamePlayer& player) const
    {
        constexpr std::array statTypes{
            PlayerStatType::Str,
            PlayerStatType::Dex,
            PlayerStatType::Accr,
            PlayerStatType::Health,
            PlayerStatType::Wisdom,
            PlayerStatType::Intell,
            PlayerStatType::Will,
            PlayerStatType::MaxHP,
            PlayerStatType::MaxSP,
            PlayerStatType::Hit,
            PlayerStatType::PhysicalDefense,
            PlayerStatType::Protection,
        };

        const PlayerStatComponent& statComponent = player.GetStatComponent();

        for (PlayerStatType statType : statTypes)
        {
            std::ostringstream oss;

            oss << fmt::format("{}: {} ||", ToString(statType), statComponent.GetFinalStat(statType).As<int32_t>());
            
            for (StatOriginType originType : StatOriginTypeMeta::GetList())
            {
                const int32_t value = statComponent.Get(statType).Get(originType).As<int32_t>();
                if (value != 0)
                {
                    oss << fmt::format(" {}: {} ||", ToString(originType), value);
                }
            }

            player.Notice(oss.str());
        }

        return true;
    }


    struct Test
    {
        static constexpr auto asd = std::array{
            PlayerStatType::Str,
            PlayerStatType::Dex,
            PlayerStatType::Accr,
            PlayerStatType::Health,
            PlayerStatType::Wisdom,
            PlayerStatType::Will,
        };
    };
}
