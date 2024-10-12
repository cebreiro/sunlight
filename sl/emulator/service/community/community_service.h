#pragma once
#include <boost/unordered/unordered_flat_map.hpp>

#include "sl/emulator/game/time/game_time.h"
#include "sl/emulator/service/emulation_service_interface.h"
#include "sl/emulator/service/community/command/community_command_interface.h"
#include "sl/emulator/service/community/notification/community_notification_interface.h"

namespace sunlight
{
    struct CommunityCommandPlayerRegister;
    struct CommunityCommandPlayerDeregister;
    struct CommunityCommandPlayerDeregisterTimer;
    struct CommunityCommandPlayerUpdateInformation;

    class CommunityPlayerStorage;
    class CommunityPartyService;
}

namespace sunlight
{
    class CommunityService final
        : public IEmulationService
        , public std::enable_shared_from_this<CommunityService>
    {
    public:
        CommunityService(const ServiceLocator& serviceLocator, execution::IExecutor& executor);
        ~CommunityService() override;

        auto Run() -> Future<void> override;
        void Shutdown() override;
        void Join(boost::system::error_code& ec) override;

        auto GetName() const -> std::string_view override;
        auto GetServiceLocator() const -> const ServiceLocator&;
        auto GetStrand() const -> Strand&;

        auto GetPlayerStorage() -> CommunityPlayerStorage&;
        auto GetPlayerStorage() const -> const CommunityPlayerStorage&;

    public:
        auto StartStreaming(int32_t zoneId, AsyncEnumerable<SharedPtrNotNull<ICommunityCommand>> commandChannel)
            -> AsyncEnumerable<SharedPtrNotNull<ICommunityNotification>>;

        void Notify(int32_t zoneId, SharedPtrNotNull<ICommunityNotification> notification);

    private:
        auto RunInputStreaming(int32_t zoneId, AsyncEnumerable<SharedPtrNotNull<ICommunityCommand>> commandChannel) -> Future<void>;

        void HandleCommand(const CommunityCommandPlayerRegister& command);
        void HandleCommand(const CommunityCommandPlayerDeregister& command);
        void HandleCommand(const CommunityCommandPlayerDeregisterTimer& command);
        void HandleCommand(const CommunityCommandPlayerUpdateInformation& command);

    private:
        void ProcessRemove(int64_t playerId);

    private:
        auto FindNotificationChannel(int32_t zoneId) -> Channel<SharedPtrNotNull<ICommunityNotification>>*;

    private:
        ServiceLocator _serviceLocator;
        SharedPtrNotNull<Strand> _strand;
        std::atomic<bool> _shutdown = false;

        std::unordered_map<int32_t, SharedPtrNotNull<Channel<SharedPtrNotNull<ICommunityCommand>>>> _commandChannels;
        std::unordered_map<int32_t, SharedPtrNotNull<Channel<SharedPtrNotNull<ICommunityNotification>>>> _notificationChannels;

        boost::unordered::unordered_flat_map<int64_t, game_time_point_type> _playerRemoveTimes;

        UniquePtrNotNull<CommunityPlayerStorage> _playerStorage;
        UniquePtrNotNull<CommunityPartyService> _partyService;
    };
}
