#include "community_service.h"

#include "sl/emulator/service/community/command/community_command.h"
#include "sl/emulator/service/community/command/party_command.h"
#include "sl/emulator/service/community/notification/community_notification.h"
#include "sl/emulator/service/community/party/community_party_service.h"
#include "sl/emulator/service/community/player/community_player_storage.h"

namespace sunlight
{
    CommunityService::CommunityService(const ServiceLocator& serviceLocator, execution::IExecutor& executor)
        : _serviceLocator(serviceLocator)
        , _strand(std::make_shared<Strand>(executor.SharedFromThis()))
        , _playerStorage(std::make_unique<CommunityPlayerStorage>())
        , _partyService(std::make_unique<CommunityPartyService>(*this))
    {
    }

    CommunityService::~CommunityService()
    {
    }

    auto CommunityService::Run() -> Future<void>
    {
        co_return;
    }

    void CommunityService::Shutdown()
    {
        bool expected = false;

        if (_shutdown.compare_exchange_strong(expected, true))
        {
            for (const auto& commandChannel : _commandChannels | std::views::values)
            {
                commandChannel->Close();
            }
        }
    }

    void CommunityService::Join(boost::system::error_code& ec)
    {
        (void)ec;
    }

    auto CommunityService::GetName() const -> std::string_view
    {
        return "community_service";
    }

    auto CommunityService::GetServiceLocator() const -> const ServiceLocator&
    {
        return _serviceLocator;
    }

    auto CommunityService::GetStrand() const -> Strand&
    {
        assert(_strand);

        return *_strand;
    }

    auto CommunityService::GetPlayerStorage() -> CommunityPlayerStorage&
    {
        assert(_playerStorage);

        return *_playerStorage;
    }

    auto CommunityService::GetPlayerStorage() const -> const CommunityPlayerStorage&
    {
        assert(_playerStorage);

        return *_playerStorage;
    }

    auto CommunityService::StartStreaming(int32_t zoneId, AsyncEnumerable<SharedPtrNotNull<ICommunityCommand>> commandChannel)
        -> AsyncEnumerable<SharedPtrNotNull<ICommunityNotification>>
    {
        auto channel = std::make_shared<Channel<SharedPtrNotNull<ICommunityNotification>>>();
        
        Post(*_strand, [self = shared_from_this(), zoneId, channel, inputChannel = std::move(commandChannel)]() mutable
            {
                if (!self->_notificationChannels.try_emplace(zoneId, channel).second)
                {
                    assert(false);

                    channel->Close();

                    return;
                }

                assert(!self->_commandChannels.contains(zoneId));
                self->_commandChannels[zoneId] = inputChannel.GetChannel();

                self->RunInputStreaming(zoneId, std::move(inputChannel));
            });

        return AsyncEnumerable<SharedPtrNotNull<ICommunityNotification>>(std::move(channel));
    }

    void CommunityService::Notify(int32_t zoneId, SharedPtrNotNull<ICommunityNotification> notification)
    {
        auto channel = FindNotificationChannel(zoneId);
        if (!channel)
        {
            return;
        }

        channel->Send(std::move(notification), channel::ChannelSignal::NotifyOne);
    }

    auto CommunityService::RunInputStreaming(int32_t zoneId, AsyncEnumerable<SharedPtrNotNull<ICommunityCommand>> commandChannel) -> Future<void>
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        [[maybe_unused]]
        const auto self = shared_from_this();

        try
        {
            while (commandChannel.HasNext())
            {
                SharedPtrNotNull<ICommunityCommand> command = co_await commandChannel;

                Visit([this]<typename T>(const T& command)
                    {
                        if constexpr (requires
                        {
                            { std::declval<CommunityService>().HandleCommand(command) };
                        })
                        {
                            this->HandleCommand(command);
                        }
                        else if constexpr (requires
                        {
                            { std::declval<CommunityPartyService>().HandleCommand(command) };
                        })
                        {
                            _partyService->HandleCommand(command);
                        }
                        else
                        {
                            static_assert(!sizeof(T), "not implemented");
                        }

                    }, *command);
            }
        }
        catch (const AsyncEnumerableClosedException&)
        {
        }
        catch (const std::exception& e)
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] throw an exception. zone_id: {}, exception: {}",
                    GetName(), zoneId, e.what()));
        }

        co_return;
    }

    void CommunityService::HandleCommand(const CommunityCommandPlayerRegister& command)
    {
        CommunityPlayerStorage& playerStorage = GetPlayerStorage();

        CommunityPlayer* newPlayer = playerStorage.Add(command.playerId, command.name);
        if (!newPlayer)
        {

            CommunityPlayer* oldPlayer = playerStorage.Find(command.playerId);
            if (!oldPlayer)
            {
                assert(false);

                return;
            }

            auto notification = std::make_shared<CommunityNotificationPlayerKick>();
            notification->playerId = command.playerId;

            Notify(oldPlayer->GetZoneId(), std::move(notification));

            return;
        }

        newPlayer->SetZoneId(command.zoneId);
        newPlayer->SetJobId(command.jobId);
        newPlayer->SetJobLevel(command.jobLevel);
        newPlayer->SetCharacterLevel(command.characterLevel);
        newPlayer->SetMaxHP(command.maxHP);

        // TODO: remove timer
    }

    void CommunityService::HandleCommand(const CommunityCommandPlayerDeregister& command)
    {
        CommunityPlayerStorage& playerStorage = GetPlayerStorage();

        CommunityPlayer* player = playerStorage.Find(command.playerId);
        if (!player)
        {
            return;
        }

        // TODO: impl process remove

        playerStorage.Remove(command.playerId);
    }

    void CommunityService::HandleCommand(const CommunityCommandPlayerDeregisterTimer& command)
    {
        (void)command;

        // TODO: impl
    }

    void CommunityService::HandleCommand(const CommunityCommandPlayerUpdateInformation& command)
    {
        (void)command;
    }

    auto CommunityService::FindNotificationChannel(int32_t zoneId) -> Channel<SharedPtrNotNull<ICommunityNotification>>*
    {
        const auto iter = _notificationChannels.find(zoneId);

        return iter != _notificationChannels.end() ? iter->second.get() : nullptr;
    }
}
