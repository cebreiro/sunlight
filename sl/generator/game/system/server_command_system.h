#pragma once
#include "sl/generator/game/system/game_system.h"

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
        ServerCommandSystem(const ServiceLocator& serviceLocator, int32_t zoneId, int32_t stageId);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;

        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        void AddCommand(SharedPtrNotNull<IServerCommand> command);

        auto GetServiceLocator() const -> const ServiceLocator&;
        auto GetZoneId() const -> int32_t;
        auto GetStageId() const -> int32_t;
        auto GetRandomEngine() -> std::mt19937&;

    private:
        void HandleCommand(const ZoneCommunityMessage& message);

    private:
        void LogRequest(const GamePlayer& player, const std::string& command);
        void LogResult(const GamePlayer& player, const std::string& command, bool result);

    private:
        const ServiceLocator& _serviceLocator;
        int32_t _zoneId = 0;
        int32_t _stageId = 0;

        std::vector<std::string> _splitStringBuffer;
        std::unordered_map<std::string, SharedPtrNotNull<IServerCommand>> _commands;

        std::mt19937 _mt;
    };
}
