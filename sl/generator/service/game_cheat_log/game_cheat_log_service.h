#pragma once
#include "sl/generator/service/emulation_service_interface.h"
#include "sl/generator/service/game_cheat_log/game_cheat_log_type.h"

namespace sunlight
{
    class GameCheatLogService final
        : public IEmulationService
        , public std::enable_shared_from_this<GameCheatLogService>
    {
    public:
        GameCheatLogService(const ServiceLocator& serviceLocator, execution::IExecutor& executor);

        void Initialize(const std::filesystem::path& cheatLogBasePath);

        auto Run() -> Future<void> override;
        void Shutdown() override;
        void Join(boost::system::error_code& ec) override;

        auto GetName() const -> std::string_view override;

    public:
        void Log(GameCheatLogType type, std::string playerName, std::string message);

    private:
        void LogImpl(GameCheatLogType type, const std::string& playerName, const std::string& message);

        auto CreateLogger(const std::string& playerName) const -> SharedPtrNotNull<ILogger>;

    private:
        std::atomic<bool> _shutdown = false;

        const ServiceLocator& _serviceLocator;
        SharedPtrNotNull<Strand> _strand;
        std::filesystem::path _basePath;

        std::unordered_map<std::string, std::pair<SharedPtrNotNull<ILogger>, std::chrono::system_clock::time_point>> _loggers;
    };
}
