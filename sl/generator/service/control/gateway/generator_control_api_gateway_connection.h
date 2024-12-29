#pragma once
#include "shared/network/session/id.h"
#include "sl/generator/api/generated/request.pb.h"
#include "sl/generator/api/generated/response.pb.h"

namespace sunlight
{
    class Session;
    class GeneratorControlAPIGatewayCipher;
}

namespace sunlight
{
    class GeneratorControlAPIGatewayConnection
    {
    public:
        enum class State
        {
            Connected,
            Authenticated,
        };

    public:
        GeneratorControlAPIGatewayConnection(SharedPtrNotNull<Strand> strand,
            SharedPtrNotNull<Session> session,
            SharedPtrNotNull<GeneratorControlAPIGatewayCipher> cipher);

        auto Send(api::Response response) -> Future<void>;
        void Receive(Buffer buffer);

        auto ParseReceived() -> std::optional<api::Request>;

        void Close();

    public:
        auto GetStrand() -> Strand&;
        auto GetSessionId() const -> session::id_type;
        auto GetState() const -> State;
        auto GetLevel() const -> int8_t;

        void SetState(State state);
        void SetLevel(int8_t level);

    private:
        SharedPtrNotNull<Strand> _strand;
        SharedPtrNotNull<Session> _session;
        SharedPtrNotNull<GeneratorControlAPIGatewayCipher> _cipher;

        State _state = State::Connected;
        int8_t _level = 0;

        Buffer _receiveBuffer;
        Buffer _bufferPool;
    };
}
