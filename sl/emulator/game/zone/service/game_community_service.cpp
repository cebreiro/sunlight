#include "game_community_service.h"

#include "sl/emulator/game/system/player_channel_system.h"
#include "sl/emulator/game/system/player_index_system.h"
#include "sl/emulator/game/zone/stage.h"
#include "sl/emulator/game/zone/zone.h"
#include "sl/emulator/service/community/notification/community_notification.h"
#include "sl/emulator/service/community/notification/party_notification.h"

namespace sunlight
{
    GameCommunityService::GameCommunityService(Zone& zone,
        SharedPtrNotNull<Channel<SharedPtrNotNull<ICommunityCommand>>> commandChannel,
        SharedPtrNotNull<Channel<SharedPtrNotNull<ICommunityNotification>>> notificationChannel)
        : _zone(zone)
        , _commandChannel(std::move(commandChannel))
        , _notificationChannel(std::move(notificationChannel))
    {
    }

    auto GameCommunityService::GetName() const -> std::string_view
    {
        return "game_community_service";
    }

    void GameCommunityService::Start()
    {
        Post(_zone.GetStrand(), [self = shared_from_this()]()
            {
                self->_runFuture = self->Run();
            });
    }

    void GameCommunityService::Shutdown()
    {
        _commandChannel->Close();
        _notificationChannel->Close();
    }

    auto GameCommunityService::Join() -> Future<void>
    {
        if (_runFuture.IsValid())
        {
            co_await _runFuture;

            _runFuture = Future<void>();
        }

        co_return;
    }

    void GameCommunityService::Send(SharedPtrNotNull<ICommunityCommand> command)
    {
        _commandChannel->Send(std::move(command), channel::ChannelSignal::NotifyOne);
    }

    auto GameCommunityService::Run() -> Future<void>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        try
        {
            AsyncEnumerable<SharedPtrNotNull<ICommunityNotification>> enumerable(_notificationChannel);

            while (enumerable.HasNext())
            {
                SharedPtrNotNull<ICommunityNotification> notification = co_await enumerable;
                assert(ExecutionContext::IsEqualTo(_zone.GetStrand()));

                Visit([this]<typename T>(const T& notification)
                    {
                        if constexpr (std::is_same_v<T, CommunityNotificationPlayerKick>)
                        {
                            Stage* stage = _zone.FindPlayerStage(notification.playerId);
                            if (!stage)
                            {
                                return;
                            }

                            const GamePlayer* player = stage->Get<PlayerIndexSystem>().FindByCId(notification.playerId);
                            if (!player)
                            {
                                assert(false);

                                return;
                            }

                            SUNLIGHT_LOG_WARN(_zone.GetServiceLocator(),
                                fmt::format("player removed by {}. cid: {}",
                                    ToString(notification.GetType()), notification.playerId));

                            _zone.LogoutPlayer(player->GetClientId());
                        }
                        else if constexpr (requires
                        {
                            { notification.playerId };
                            requires std::same_as<decltype(notification.playerId), int64_t>;
                        })
                        {
                            Stage* stage = _zone.FindPlayerStage(notification.playerId);
                            if (!stage)
                            {
                                return;
                            }

                            if constexpr (requires
                            {
                                { std::declval<PlayerChannelSystem>().HandleNotification(notification) };

                            })
                            {
                                stage->Get<PlayerChannelSystem>().HandleNotification(notification);
                            }
                            else
                            {
                                static_assert(!sizeof(T), "not implemented");
                            }
                        }
                        else
                        {
                            static_assert(!sizeof(T), "not implemented");
                        }

                    }, *notification);
            }
        }
        catch (const AsyncEnumerableClosedException&)
        {
        }
        catch (const std::exception& e)
        {
            SUNLIGHT_LOG_ERROR(_zone.GetServiceLocator(),
                fmt::format("[{}] throws an exception. exception: {}",
                    GetName(), e.what()));
        }

        co_return;
    }
}
