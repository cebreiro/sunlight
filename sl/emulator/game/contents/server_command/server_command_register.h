#pragma once

namespace sunlight
{
    class ServerCommandSystem;
}

namespace sunlight
{
    class ServerCommandRegister
    {
    public:
        ServerCommandRegister() = delete;

        static void Register(ServerCommandSystem& system);
    };
}
