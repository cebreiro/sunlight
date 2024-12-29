#include "generator_control_api_gateway_connection.h"

#include <boost/scope/scope_exit.hpp>
#include "shared/network/session/session.h"
#include "sl/generator/api/generated/request.pb.h"
#include "sl/generator/api/generated/response.pb.h"
#include "sl/generator/service/control/gateway/generator_control_api_gateway_cipher.h"
#include "sl/generator/service/control/gateway/zero_copy_input_stream.h"

namespace sunlight
{
    constexpr int32_t packetHeaderSize = 4;

    GeneratorControlAPIGatewayConnection::GeneratorControlAPIGatewayConnection(SharedPtrNotNull<Strand> strand,
        SharedPtrNotNull<Session> session, SharedPtrNotNull<GeneratorControlAPIGatewayCipher> cipher)
        : _strand(std::move(strand))
        , _session(std::move(session))
        , _cipher(std::move(cipher))
    {
        assert(_strand);
        assert(_session);
        assert(_cipher);
    }

    auto GeneratorControlAPIGatewayConnection::Send(api::Response response) -> Future<void>
    {
        if (!ExecutionContext::Contains(*_strand))
        {
            co_await *_strand;
        }

        const int32_t bodySize = static_cast<int32_t>(response.ByteSizeLong());

        const int32_t packetSize = packetHeaderSize + bodySize + GeneratorControlAPIGatewayCipher::TAG_SIZE;

        if (_bufferPool.GetSize() < packetSize)
        {
            _bufferPool.Add(buffer::Fragment::Create(std::max(1024, packetSize)));
        }

        Buffer packetBuffer;
        Buffer bodyBuffer;

        [[maybe_unused]]
        const bool sliced1 = _bufferPool.SliceFront(packetBuffer, packetHeaderSize);
        assert(sliced1);

        const bool sliced2 = _bufferPool.SliceFront(bodyBuffer, bodySize);
        assert(sliced2);

        // TODO: Implement ZeroCopyOutputStream and use it
        char* stackBuffer = static_cast<char*>(_malloca(bodySize));

        boost::scope::scope_exit exit([stackBuffer]()
            {
                _freea(stackBuffer);
            });

        response.SerializeToArray(stackBuffer, bodySize);

        BufferWriter bodyWriter(bodyBuffer);
        bodyWriter.WriteBuffer(std::span<const char>(stackBuffer, bodySize));

        std::span<const char> additionalData{ reinterpret_cast<const char*>(&packetSize), 4 };
        std::array<char, GeneratorControlAPIGatewayCipher::TAG_SIZE> tag = {};

        if (!_cipher->Encrypt(bodyBuffer, additionalData, tag))
        {
            throw std::runtime_error("fail to encrypt by ChaCha20-Polly1305");
        }

        bodyBuffer.MergeBack([this, &tag]() -> Buffer
            {
                Buffer temp;

                _bufferPool.SliceFront(temp, GeneratorControlAPIGatewayCipher::TAG_SIZE);

                BufferWriter writer(temp);
                writer.WriteBuffer(tag);

                return temp;
            }());

        BufferWriter headWriter(packetBuffer);
        headWriter.Write<int32_t>(packetSize);

        packetBuffer.MergeBack(std::move(bodyBuffer));

        _session->Send(std::move(packetBuffer));
    }

    void GeneratorControlAPIGatewayConnection::Receive(Buffer buffer)
    {
        _receiveBuffer.MergeBack(std::move(buffer));
    }

    auto GeneratorControlAPIGatewayConnection::ParseReceived() -> std::optional<api::Request>
    {
        const int64_t receivedSize = _receiveBuffer.GetSize();
        if (receivedSize < 4)
        {
            return std::nullopt;
        }

        BufferReader receiveBufferReader(_receiveBuffer.cbegin(), _receiveBuffer.cend());

        const int32_t packetSize = receiveBufferReader.Read<int32_t>();
        if (packetSize < packetHeaderSize + GeneratorControlAPIGatewayCipher::TAG_SIZE)
        {
            throw std::runtime_error("invalid packet length");
        }

        if (packetSize < receivedSize)
        {
            return std::nullopt;
        }

        Buffer headBuffer;
        Buffer bodyBuffer;
        Buffer tagBuffer;

        [[maybe_unused]]
        bool sliced1 = _receiveBuffer.SliceFront(headBuffer, packetHeaderSize);
        assert(sliced1);

        [[maybe_unused]]
        const bool sliced2 = _receiveBuffer.SliceFront(bodyBuffer, packetSize - packetHeaderSize - GeneratorControlAPIGatewayCipher::TAG_SIZE);
        assert(sliced2);

        [[maybe_unused]]
        const bool sliced3 = _receiveBuffer.SliceFront(tagBuffer, GeneratorControlAPIGatewayCipher::TAG_SIZE);
        assert(sliced3);

        std::span<const char> additionalData{ reinterpret_cast<const char*>(&packetSize), 4 };
        std::array<char, GeneratorControlAPIGatewayCipher::TAG_SIZE> tag = {};

        BufferReader headReader(tagBuffer.cbegin(), tagBuffer.cend());
        headReader.ReadBuffer(tag.data(), GeneratorControlAPIGatewayCipher::TAG_SIZE);

        if (!_cipher->Decrypt(bodyBuffer, additionalData, tag))
        {
            throw std::runtime_error("fail to decrypt packet");
        }

        ZeroCopyInputStream inputStream(bodyBuffer);
        google::protobuf::io::CodedInputStream codedInputStream(&inputStream);

        api::Request request;

        if (!request.ParseFromCodedStream(&codedInputStream))
        {
            throw std::runtime_error("fail to deserialize request");
        }

        return request;
    }

    void GeneratorControlAPIGatewayConnection::Close()
    {
        _session->Close();
    }

    auto GeneratorControlAPIGatewayConnection::GetStrand() -> Strand&
    {
        return *_strand;
    }

    auto GeneratorControlAPIGatewayConnection::GetSessionId() const -> session::id_type
    {
        return _session->GetId();
    }

    auto GeneratorControlAPIGatewayConnection::GetState() const -> State
    {
        return _state;
    }

    auto GeneratorControlAPIGatewayConnection::GetLevel() const -> int8_t
    {
        return _level;
    }

    void GeneratorControlAPIGatewayConnection::SetState(State state)
    {
        _state = state;
    }

    void GeneratorControlAPIGatewayConnection::SetLevel(int8_t level)
    {
        _level = level;
    }
}
