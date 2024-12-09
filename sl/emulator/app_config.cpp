#include "app_config.h"

namespace sunlight
{
    void from_json(const nlohmann::json& j, PathConfig& config)
    {
        config.logFilePath = j.at("log_file_path").get<std::string>();
        config.gameConfigFilePath = j.at("game_config_file_path").get<std::string>();
        config.cheatLogPath = j.at("cheat_log_path").get<std::string>();
        config.assetPath = j.at("asset_path").get<std::string>();
        config.scriptPath = j.at("scriptPath").get<std::string>();
    }

    void from_json(const nlohmann::json& j, DatabaseConfig& config)
    {
        config.address = j.at("address").get<std::string>();
        config.port = j.at("port").get<uint16_t>();
        config.user = j.at("user").get<std::string>();
        config.password = j.at("password").get<std::string>();
        config.schema = j.at("schema").get<std::string>();
        config.connectionCount = j.at("connection_count").get<int32_t>();
    }

    void from_json(const nlohmann::json& j, ZoneConfig& config)
    {
        config.port = j.at("port").get<uint16_t>();
        config.zoneId = j.at("zone_id").get<int32_t>();
    }

    void from_json(const nlohmann::json& j, WorldConfig& config)
    {
        config.id = j.at("id").get<int8_t>();

        j.at("zone_config").get_to<std::vector<ZoneConfig>>(config.zoneConfig);
    }

    void from_json(const nlohmann::json& j, AppConfig& config)
    {
        config.ioThreadCount = j.at("io_thread_count").get<int64_t>();
        config.gameThreadCount = j.at("io_thread_count").get<int64_t>();
        config.publicAddress = j.at("public_address").get<std::string>();

        j.at("path_config").get_to<PathConfig>(config.pathConfig);
        j.at("database_config").get_to<DatabaseConfig>(config.databaseConfig);
        j.at("world_config").get_to<std::vector<WorldConfig>>(config.worldConfig);
    }
}
