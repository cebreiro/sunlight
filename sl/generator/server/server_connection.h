#pragma once
#include "sl/generator/server/server_type.h"

namespace sunlight
{
    class GameClient;
    class Session;
    class IPacketCodec;
    class IPacketC2SHandler;
}

namespace sunlight
{
    class ServerConnection final : public std::enable_shared_from_this<ServerConnection>
    {
    public:
        ServerConnection() = delete;
        ServerConnection(const ServerConnection& other) = delete;
        ServerConnection(ServerConnection&& other) noexcept = delete;
        ServerConnection& operator=(const ServerConnection& other) = delete;
        ServerConnection& operator=(ServerConnection&& other) noexcept = delete;

        ServerConnection(
            const ServiceLocator& serviceLocator,
            ServerType type,
            SharedPtrNotNull<Session> session,
            SharedPtrNotNull<Strand> strand,
            SharedPtrNotNull<IPacketCodec> codec,
            SharedPtrNotNull<IPacketC2SHandler> handler);
        ~ServerConnection();

        void Start();
        void Stop();

        void Receive(Buffer buffer);
        void Send(Buffer buffer);
        void Send(std::vector<Buffer> buffer);

        auto GetStrand() -> Strand&;
        auto GetStrand() const -> const Strand&;
        auto GetSession() -> Session&;
        auto GetSession() const -> const Session&;
        auto GetGameClientPtr() -> GameClient*;
        auto GetGameClientPtr() const -> const GameClient*;
        auto GetStorage() -> std::any&;

        void SetGameClient(std::shared_ptr<GameClient> client);

        auto MakePacketHeader(const Buffer& body) -> Buffer;

    private:
        auto Run() -> Future<void>;

        void ReceiveImpl(Buffer buffer);
        void SendImpl(Buffer buffer);

        bool IsValidPacketHeader(const Buffer& buffer) const;

        static auto GetPacketHeadSize(ServerType type) -> int64_t;
        static auto GetPacketBodySize(ServerType type, const Buffer& buffer) -> int64_t;

    private:
        ServiceLocator _serviceLocator;
        ServerType _type = {};
        std::atomic<bool> _shutdown = false;

        SharedPtrNotNull<Session> _session;
        SharedPtrNotNull<Strand> _strand;
        SharedPtrNotNull<IPacketCodec> _codec;
        SharedPtrNotNull<IPacketC2SHandler> _handler;
        SharedPtrNotNull<Channel<Buffer>> _receiveChannel;
        Buffer _receiveBuffer;

        Buffer _sendPacketHeaderBuffer;

        SharedPtrNotNull<GameClient> _client;
        std::any _storage;
    };
}
