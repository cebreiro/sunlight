#pragma once

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

        int32_t level = 0;

        Buffer receiveBuffer;
    };
}
