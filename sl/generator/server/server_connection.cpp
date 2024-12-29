#include "server_connection.h"

#include "shared/network/session/session.h"
#include "sl/generator/server/server_constant.h"
#include "sl/generator/server/client/game_client.h"
#include "sl/generator/server/packet/codec/packet_codec_interface.h"
#include "sl/generator/server/packet/handler/packet_c2s_handler_interface.h"

namespace sunlight
{
    ServerConnection::ServerConnection(const ServiceLocator& serviceLocator, ServerType type,
        SharedPtrNotNull<Session> session, SharedPtrNotNull<Strand> strand, SharedPtrNotNull<IPacketCodec> codec,
        SharedPtrNotNull<IPacketC2SHandler> handler)
        : _serviceLocator(serviceLocator)
        , _type(type)
        , _session(std::move(session))
        , _strand(std::move(strand))
        , _codec(std::move(codec))
        , _handler(std::move(handler))
        , _receiveChannel(std::make_shared<Channel<Buffer>>())
    {
    }

    ServerConnection::~ServerConnection()
    {
    }

    void ServerConnection::Start()
    {
        Post(*_strand, [self = shared_from_this()]() mutable
            {
                self->Run().Then(*self->_strand,
                    [self]()
                    {
                        SUNLIGHT_LOG_DEBUG(self->_serviceLocator,
                            fmt::format("[{}] client connection end. session: {}",
                                ToString(self->_type), self->_session->GetId()));
                    });
            });
    }

    void ServerConnection::Stop()
    {
        bool expected = false;

        if (_shutdown.compare_exchange_strong(expected, true))
        {
            _session->Close();
            _receiveChannel->Close();
        }
    }

    void ServerConnection::Receive(Buffer buffer)
    {
        Post(*_strand, [self = shared_from_this(), buffer = std::move(buffer)]() mutable
            {
                self->ReceiveImpl(std::move(buffer));
            });
    }

    void ServerConnection::Send(Buffer buffer)
    {
        Post(*_strand, [self = shared_from_this(), buffer = std::move(buffer)]() mutable
            {
                self->SendImpl(std::move(buffer));
            });
    }

    void ServerConnection::Send(std::vector<Buffer> buffers)
    {
        Post(*_strand, [self = shared_from_this(), buffers = std::move(buffers)]() mutable
            {
                for (auto& buffer : buffers)
                {
                    self->SendImpl(std::move(buffer));
                }
            });
    }

    auto ServerConnection::GetStrand() -> Strand&
    {
        assert(_strand);

        return *_strand;
    }

    auto ServerConnection::GetStrand() const -> const Strand&
    {
        assert(_strand);

        return *_strand;
    }

    auto ServerConnection::GetSession() -> Session&
    {
        return *_session;
    }

    auto ServerConnection::GetSession() const -> const Session&
    {
        return *_session;
    }

    auto ServerConnection::GetGameClientPtr() -> GameClient*
    {
        return _client.get();
    }

    auto ServerConnection::GetGameClientPtr() const -> const GameClient*
    {
        return _client.get();
    }

    auto ServerConnection::GetStorage() -> std::any&
    {
        return _storage;
    }

    void ServerConnection::SetGameClient(std::shared_ptr<GameClient> client)
    {
        _client = std::move(client);
    }

    auto ServerConnection::Run() -> Future<void>
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        [[maybe_unused]]
        const auto self = shared_from_this();

        AsyncEnumerable<Buffer> asyncEnumerable(_receiveChannel);

        try
        {
            const int64_t packetHeadSize = GetPacketHeadSize(_type);

            Buffer header;

            while (asyncEnumerable.HasNext())
            {
                Buffer received = co_await asyncEnumerable;
                assert(ExecutionContext::IsEqualTo(*_strand));

                _receiveBuffer.MergeBack(std::move(received));

                while (true)
                {
                    if (header.Empty())
                    {
                        if (_receiveBuffer.GetSize() < packetHeadSize)
                        {
                            break;
                        }

                        [[maybe_unused]]
                        const bool sliced = _receiveBuffer.SliceFront(header, packetHeadSize);
                        assert(sliced);

                        if (_type != ServerType::Login)
                        {
                            _codec->Decode(header.begin(), header.end());
                        }
                    }

                    const int64_t bodySize = GetPacketBodySize(_type, header);
                    if (_receiveBuffer.GetSize() < bodySize)
                    {
                        break;
                    }

                    if (!IsValidPacketHeader(header))
                    {
                        SUNLIGHT_LOG_ERROR(_serviceLocator,
                            fmt::format("[{}] invalid packet header detected. session: {} header: {}, receive_buffer: {}",
                                ToString(_type), _session->GetId(), header.ToString(), _receiveBuffer.ToString()));

                        _session->Close();

                        co_return;
                    }

                    Buffer packetBuffer;

                    (void)_receiveBuffer.SliceFront(packetBuffer, bodySize);

                    _codec->Decode(packetBuffer.begin(), packetBuffer.end());

                    SUNLIGHT_LOG_INFO(_serviceLocator,
                        fmt::format("{}, {}\nremain: {}", header.ToString(), packetBuffer.ToString(), _receiveBuffer.ToString()));

                    co_await _handler->HandlePacket(*this, std::move(packetBuffer));

                    header.Clear();
                }
            }
        }
        catch (const AsyncEnumerableClosedException& e)
        {
            SUNLIGHT_LOG_DEBUG(_serviceLocator,
                fmt::format("[{}] server connection is stopped. session: {}, e: {}",
                    ToString(_type), _session->GetId(), e.what()));
        }
        catch (const std::exception& e)
        {
            SUNLIGHT_LOG_ERROR(_serviceLocator,
                fmt::format("[{}] exception throws. session: {}, exception: {}",
                    ToString(_type), _session->GetId(), e.what()));

            _session->Close();
        }

        co_return;
    }

    void ServerConnection::ReceiveImpl(Buffer buffer)
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        _receiveChannel->Send(std::move(buffer), channel::ChannelSignal::NotifyOne);
    }

    void ServerConnection::SendImpl(Buffer buffer)
    {
        assert(ExecutionContext::IsEqualTo(*_strand));

        Buffer header = MakePacketHeader(buffer);

        SUNLIGHT_LOG_INFO(_serviceLocator, fmt::format("[send] {}, {}", header.ToString(), buffer.ToString()));

        switch (_type)
        {
        case ServerType::Login:
        {
            _codec->Encode(buffer.begin(), buffer.end());

            header.MergeBack(std::move(buffer));
        }
        break;
        case ServerType::Lobby:
        case ServerType::Zone:
        {

            header.MergeBack(std::move(buffer));

            _codec->Encode(header.begin(), header.end());
        }
        break;
        default:
            assert(false);
        }

        _session->Send(std::move(header));
    }

    auto ServerConnection::MakePacketHeader(const Buffer& body) -> Buffer
    {
        Buffer result;

        const int64_t packetHeaderSize = GetPacketHeadSize(_type);

        if (_sendPacketHeaderBuffer.GetSize() < packetHeaderSize)
        {
            _sendPacketHeaderBuffer.Add(buffer::Fragment::Create(1024));
        }

        _sendPacketHeaderBuffer.SliceFront(result, packetHeaderSize);

        switch (_type)
        {
        case ServerType::Login:
        {
            BufferWriter writer(result);

            writer.Write<uint16_t>(static_cast<uint16_t>(body.GetSize()) - 1);
        }
        break;
        case ServerType::Lobby:
        case ServerType::Zone:
        {
            BufferWriter writer(result);

            const uint32_t size = static_cast<uint32_t>(packetHeaderSize + body.GetSize());
            const int8_t sum = [](uint32_t size)
                {
                    int8_t result = 0;
                    for (int32_t i = 0; i < 4; ++i)
                    {
                        result += *(reinterpret_cast<const int8_t*>(&size) + i);
                    }

                    return result;
                }(size);

            writer.Write<uint32_t>(size);
            writer.Write<uint32_t>(size - 9);
            writer.Write<int8_t>(sum);
            writer.Write<uint16_t>(static_cast<uint16_t>((size - 9) & 0xFFFF));
        }
        break;
        default:
            assert(false);
        }

        return result;
    }

    bool ServerConnection::IsValidPacketHeader(const Buffer& buffer) const
    {
        const int64_t headerSize = GetPacketHeadSize(_type);
        assert(buffer.GetSize() >= headerSize);

        switch (_type)
        {
        case ServerType::Login:
        {
            BufferReader reader(buffer.begin(), buffer.end());

            return reader.Read<uint16_t>() > 0;
        }
        case ServerType::Lobby:
        case ServerType::Zone:
        {
            BufferReader reader(buffer.begin(), buffer.end());

            const uint32_t size = reader.Read<uint32_t>();

            if (size <= headerSize || size > std::numeric_limits<uint32_t>::max())
            {
                return false;
            }

            const uint32_t bodySize = reader.Read<uint32_t>();
            if (bodySize != size - 9)
            {
                return false;
            }

            if (const int8_t value = reader.Read<int8_t>();
                value != [](uint32_t size) -> int8_t
                {
                    int8_t result = 0;
                    for (int32_t i = 0; i < 4; ++i)
                    {
                        result += *(reinterpret_cast<const int8_t*>(&size) + i);
                    }

                    return result;
                }(size))
            {
                return false;
            }

            if (reader.Read<uint16_t>() != (bodySize & 0xFFFF))
            {
                return false;
            }

            return size - headerSize;
        }
        default:;
        }

        assert(false);

        return false;
    }

    auto ServerConnection::GetPacketHeadSize(ServerType type) -> int64_t
    {
        switch (type)
        {
        case ServerType::Login:
            return 2;
        case ServerType::Lobby:
        case ServerType::Zone:
            return 11;
        default:;
        }

        assert(false);

        return -1;
    }

    auto ServerConnection::GetPacketBodySize(ServerType type, const Buffer& buffer) -> int64_t
    {
        BufferReader reader(buffer.begin(), buffer.end());

        switch (type)
        {
        case ServerType::Login:
        {
            return reader.Read<uint16_t>() + 1;
        }
        case ServerType::Lobby:
        case ServerType::Zone:
        {
            return reader.Read<uint32_t>() - 11;
        }
        default:;
        }

        assert(false);

        return -1;
    }
}
