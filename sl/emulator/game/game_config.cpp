#include "game_config.h"

namespace sunlight
{
    void from_json(const nlohmann::json& j, GameConfig& config)
    {
        if (const auto iter = j.find("no_spawn_monster"); iter != j.end())
        {
            config.noSpawnMonster = iter->get<bool>();
        }

        if (const auto iter = j.find("drop_item_ownership_seconds"); iter != j.end())
        {
            config.dropItemOwnershipSeconds = std::chrono::seconds(std::max(1, iter->get<int32_t>()));
        }

        if (const auto iter = j.find("drop_item_keep_seconds"); iter != j.end())
        {
            config.dropItemKeepSeconds = std::chrono::seconds(std::max(1, iter->get<int32_t>()));
        }
    }
}
