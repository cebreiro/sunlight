#pragma once

namespace sunlight
{
    struct GameConfig
    {
        bool noSpawnMonster = false;


        friend void from_json(const nlohmann::json& j, GameConfig& config);
    };
}
