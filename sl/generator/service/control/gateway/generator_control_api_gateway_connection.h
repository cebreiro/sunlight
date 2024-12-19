#pragma once

namespace sunlight::api
{
    class Response;
}

namespace sunlight
{
    class Session;
}

namespace sunlight
{
    struct GeneratorControlAPIGatewayConnection
    {
        enum class State
        {
            Connected,
            Authenticated,
        };

        SharedPtrNotNull<Strand> strand;

        State state = State::Connected;
        SharedPtrNotNull<Session> session;

        int8_t level = 0;

        Buffer receiveBuffer;

        auto Send(api::Response response) -> Future<void>;

    private:
        Buffer bufferPool;
    };
}
