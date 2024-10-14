#pragma once
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
    class GamePlayer;
    class IServerCommand;
    struct ZoneCommunityMessage;
}

namespace sunlight
{
    class ServerCommandSystem final : public GameSystem
    {
    public:
        explicit ServerCommandSystem(const ServiceLocator& serviceLocator);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;

        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        void AddCommand(SharedPtrNotNull<IServerCommand> command);

        auto GetServiceLocator() const -> const ServiceLocator&;
        auto GetRandomEngine() -> std::mt19937&;

    private:
        void HandleCommand(const ZoneCommunityMessage& message);

    private:
        void LogRequest(const GamePlayer& player, const std::string& command);
        void LogResult(const GamePlayer& player, const std::string& command, bool result);

    private:
        const ServiceLocator& _serviceLocator;

        std::vector<std::string> _splitStringBuffer;
        std::unordered_map<std::string, SharedPtrNotNull<IServerCommand>> _commands;

        std::mt19937 _mt;
    };
}
