#pragma once
#include <boost/unordered/unordered_flat_map.hpp>

#include "sl/generator/game/time/game_time.h"
#include "sl/generator/service/emulation_service_interface.h"
#include "sl/generator/service/community/command/community_command_interface.h"
#include "sl/generator/service/community/notification/community_notification_interface.h"

namespace sunlight
{
    struct CommunityCommandPlayerRegister;
    struct CommunityCommandPlayerDeregister;
    struct CommunityCommandPlayerDeregisterTimer;
    struct CommunityCommandPlayerUpdateInformation;
    struct CommunityCommandChatDeliver;

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
        auto GetUserCount() const -> Future<int32_t>;

    public:
        auto StartStreaming(int32_t zoneId, AsyncEnumerable<SharedPtrNotNull<ICommunityCommand>> commandChannel)
            -> AsyncEnumerable<SharedPtrNotNull<ICommunityNotification>>;

        void Notify(int32_t zoneId, SharedPtrNotNull<ICommunityNotification> notification);
        void Visit(const std::function<void(Channel<SharedPtrNotNull<ICommunityNotification>>&)>& visitor);

    private:
        auto RunInputStreaming(int32_t zoneId, AsyncEnumerable<SharedPtrNotNull<ICommunityCommand>> commandChannel) -> Future<void>;

        void HandleCommand(const CommunityCommandPlayerRegister& command);
        void HandleCommand(const CommunityCommandPlayerDeregister& command);
        void HandleCommand(const CommunityCommandPlayerDeregisterTimer& command);
        void HandleCommand(const CommunityCommandPlayerUpdateInformation& command);
        void HandleCommand(const CommunityCommandChatDeliver& command);

    private:
        void ProcessRemove(int64_t playerId);

    private:
        auto FindNotificationChannel(int32_t zoneId) -> Channel<SharedPtrNotNull<ICommunityNotification>>*;

    private:
        const ServiceLocator& _serviceLocator;
        SharedPtrNotNull<Strand> _strand;
        std::atomic<bool> _shutdown = false;

        boost::unordered::unordered_flat_map<int32_t, SharedPtrNotNull<Channel<SharedPtrNotNull<ICommunityCommand>>>> _commandChannels;
        boost::unordered::unordered_flat_map<int32_t, SharedPtrNotNull<Channel<SharedPtrNotNull<ICommunityNotification>>>> _notificationChannels;

        boost::unordered::unordered_flat_map<int64_t, game_time_point_type> _playerRemoveTimes;

        UniquePtrNotNull<CommunityPlayerStorage> _playerStorage;
        UniquePtrNotNull<CommunityPartyService> _partyService;
    };
}
