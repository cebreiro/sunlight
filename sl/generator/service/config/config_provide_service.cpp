#include "config_provide_service.h"

namespace sunlight
{
    void ConfigProvideService::Initialize(const AppConfig& appConfig, const std::filesystem::path& gameConfigPath)
    {
        _appConfig = &appConfig;

        std::ifstream ifs(gameConfigPath);
        if (!ifs.is_open())
        {
            throw std::runtime_error(fmt::format("fail to open game config file. path: {}", gameConfigPath.string()));
        }

        nlohmann::json json = nlohmann::json::parse(ifs);
        json.get_to<GameConfig>(_gameConfig);
    }

    auto ConfigProvideService::GetName() const -> std::string_view
    {
        return "config_provide_service";
    }

    auto ConfigProvideService::GetAppConfig() const -> const AppConfig&
    {
        assert(_appConfig);

        return *_appConfig;
    }

    auto ConfigProvideService::GetGameConfig() const -> const GameConfig&
    {
        return _gameConfig;
    }
}
