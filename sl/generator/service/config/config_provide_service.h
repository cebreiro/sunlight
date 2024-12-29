#pragma once
#include "sl/generator/game/game_config.h"

namespace sunlight
{
    struct AppConfig;
}

namespace sunlight
{
    class ConfigProvideService final : public IService
    {
    public:
        void Initialize(const AppConfig& appConfig, const std::filesystem::path& gameConfigPath);

        auto GetName() const->std::string_view override;

    public:
        auto GetAppConfig() const -> const AppConfig&;
        auto GetGameConfig() const -> const GameConfig&;

    private:
        PtrNotNull<const AppConfig> _appConfig = nullptr;
        GameConfig _gameConfig;
    };
}
