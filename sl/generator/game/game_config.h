#pragma once

namespace sunlight
{
    struct GameConfig
    {
        bool noSpawnMonster = false;
        std::chrono::seconds dropItemOwnershipSeconds{ 30 };
        std::chrono::seconds dropItemKeepSeconds{ 60 };

        friend void from_json(const nlohmann::json& j, GameConfig& config);
    };
}
