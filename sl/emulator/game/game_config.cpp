#include "game_config.h"

namespace sunlight
{
    void from_json(const nlohmann::json& j, GameConfig& config)
    {
        if (const auto iter = j.find("no_spawn_monster"); iter != j.end())
        {
            config.noSpawnMonster = iter->get<bool>();
        }
    }
}
