#pragma once
#include "shared/network/server/server.h"

namespace sunlight
{
    class GeneratorControlAPIGatewayServer final : public Server
    {
    public:
        explicit GeneratorControlAPIGatewayServer(execution::AsioExecutor& executor);

        void Initialize(ServiceLocator& serviceLocator) override;

    private:
        void OnAccept(Session& session) override;
        void OnReceive(Session& session, Buffer buffer) override;
        void OnError(Session& session, const boost::system::error_code& error) override;

        static auto GetName() -> std::string_view;

    private:
        struct Connection
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

        auto FindConnection(session::id_type id) -> SharedPtrNotNull<Connection>;
        auto FindConnection(session::id_type id) const ->SharedPtrNotNull<const Connection>;

    private:
        ServiceLocator _serviceLocator;

        tbb::concurrent_hash_map<session::id_type, SharedPtrNotNull<Connection>> _connections;
    };
}
