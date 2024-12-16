#pragma once
#include "shared/network/server/server.h"

namespace sunlight
{
    struct GeneratorControlAPIGatewayConnection;

    class GeneratorControlRequestRouter;
    class IGeneratorControlRequestHandler;
}

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
        auto FindConnection(session::id_type id) -> SharedPtrNotNull<GeneratorControlAPIGatewayConnection>;
        auto FindConnection(session::id_type id) const ->SharedPtrNotNull<const GeneratorControlAPIGatewayConnection>;

    private:
        template <typename T>
        void AddHandlerToRouter(SharedPtrNotNull<T> handler);

    private:
        ServiceLocator _serviceLocator;

        tbb::concurrent_hash_map<session::id_type, SharedPtrNotNull<GeneratorControlAPIGatewayConnection>> _connections;

        SharedPtrNotNull<GeneratorControlRequestRouter> _router;
        std::vector<SharedPtrNotNull<IGeneratorControlRequestHandler>> _handlers;
    };
}
