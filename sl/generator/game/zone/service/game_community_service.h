#pragma once

namespace sunlight
{
    class ICommunityCommand;
    class ICommunityNotification;

    class Zone;
}

namespace sunlight
{
    class GameCommunityService final
        : public IService
        , public std::enable_shared_from_this<GameCommunityService>
    {
    public:
        GameCommunityService(Zone& zone,
            SharedPtrNotNull<Channel<SharedPtrNotNull<ICommunityCommand>>> commandChannel,
            SharedPtrNotNull<Channel<SharedPtrNotNull<ICommunityNotification>>> notificationChannel);

        auto GetName() const -> std::string_view override;

    public:
        void Start();
        void Shutdown();
        auto Join() -> Future<void>;

        void Send(SharedPtrNotNull<ICommunityCommand> command);

    private:
        auto Run() -> Future<void>;

    private:
        Zone& _zone;
        Future<void> _runFuture;

        SharedPtrNotNull<Channel<SharedPtrNotNull<ICommunityCommand>>> _commandChannel;
        SharedPtrNotNull<Channel<SharedPtrNotNull<ICommunityNotification>>> _notificationChannel;
    };
}
