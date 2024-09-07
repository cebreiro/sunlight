#pragma once

namespace sunlight::sl::emulator
{
    struct DatabaseConfig
    {
        std::string address;
        uint16_t port = 0;
        std::string user;
        std::string password;
        std::string schema;
        int32_t connectionCount = 0;

        

        friend void from_json(const nlohmann::json& j, DatabaseConfig& config)
        {
            config.address = j.at("address").get<std::string>();
            config.port = j.at("port").get<uint16_t>();
            config.user = j.at("user").get<std::string>();
            config.password = j.at("password").get<std::string>();
            config.schema = j.at("schema").get<std::string>();
            config.connectionCount = j.at("connection_count").get<int32_t>();
        }
    };

    struct ZoneConfig
    {
        uint16_t port = 0;
        int32_t zoneId = 0;

        friend void from_json(const nlohmann::json& j, ZoneConfig& config)
        {
            config.port = j.at("port").get<uint16_t>();
            config.zoneId = j.at("zone_id").get<int32_t>();
        }
    };

    struct WorldConfig
    {
        int8_t id = 0;

        std::vector<ZoneConfig> zoneConfig;

        friend void from_json(const nlohmann::json& j, WorldConfig& config)
        {
            config.id = j.at("id").get<int8_t>();

            j.at("zone_config").get_to<std::vector<ZoneConfig>>(config.zoneConfig);
        }
    };

    struct AppConfig
    {
        int64_t ioThreadCount = 0;
        int64_t gameThreadCount = 0;

        std::string logFilePath;
        std::string publicAddress;

        DatabaseConfig databaseConfig;
        std::vector<WorldConfig> worldConfig;

        friend void from_json(const nlohmann::json& j, AppConfig& config)
        {
            config.ioThreadCount = j.at("io_thread_count").get<int64_t>();
            config.gameThreadCount = j.at("io_thread_count").get<int64_t>();

            config.logFilePath = j.at("log_file_path").get<std::string>();
            config.publicAddress = j.at("public_address").get<std::string>();

            j.at("database_config").get_to<DatabaseConfig>(config.databaseConfig);
            j.at("world_config").get_to<std::vector<WorldConfig>>(config.worldConfig);
        }
    };
}
