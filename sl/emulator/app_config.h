#pragma once

namespace sunlight
{
    struct PathConfig
    {
        std::string logFilePath;
        std::string gameConfigFilePath;
        std::string cheatLogPath;
        std::string assetPath;
        std::string scriptPath;

        friend void from_json(const nlohmann::json& j, PathConfig& config);
    };

    struct DatabaseConfig
    {
        std::string address;
        uint16_t port = 0;
        std::string user;
        std::string password;
        std::string schema;
        int32_t connectionCount = 0;

        friend void from_json(const nlohmann::json& j, DatabaseConfig& config);
    };

    struct ZoneConfig
    {
        uint16_t port = 0;
        int32_t zoneId = 0;

        friend void from_json(const nlohmann::json& j, ZoneConfig& config);
    };

    struct WorldConfig
    {
        int8_t id = 0;

        std::vector<ZoneConfig> zoneConfig;

        friend void from_json(const nlohmann::json& j, WorldConfig& config);
    };

    struct AppConfig
    {
        int64_t ioThreadCount = 0;
        int64_t gameThreadCount = 0;
        std::string publicAddress;

        PathConfig pathConfig;
        DatabaseConfig databaseConfig;
        std::vector<WorldConfig> worldConfig;

        friend void from_json(const nlohmann::json& j, AppConfig& config);
    };
}
