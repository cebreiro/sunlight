#pragma once
#include "shared/network/server/server.h"

namespace sunlight
{
    class GeneratorControlAPIGatewayConnection;

    class GeneratorControlRequestRouter;
    class GeneratorControlRequestRegistry;
}

namespace sunlight
{
    class GeneratorControlAPIGatewayServer final : public Server
    {
    public:
        GeneratorControlAPIGatewayServer(const ServiceLocator& serviceLocator, execution::AsioExecutor& executor);

    private:
        void OnAccept(Session& session) override;
        void OnReceive(Session& session, Buffer buffer) override;
        void OnError(Session& session, const boost::system::error_code& error) override;

        static auto GetName() -> std::string_view;

    private:
        auto FindConnection(session::id_type id) -> SharedPtrNotNull<GeneratorControlAPIGatewayConnection>;
        auto FindConnection(session::id_type id) const ->SharedPtrNotNull<const GeneratorControlAPIGatewayConnection>;

    private:
        tbb::concurrent_hash_map<session::id_type, SharedPtrNotNull<GeneratorControlAPIGatewayConnection>> _connections;

        SharedPtrNotNull<GeneratorControlRequestRouter> _router;
        SharedPtrNotNull<GeneratorControlRequestRegistry> _handlerRegistry;
    };
}
