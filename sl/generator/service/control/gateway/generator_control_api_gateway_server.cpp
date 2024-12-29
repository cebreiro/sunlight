#include "generator_control_api_gateway_server.h"

#include "shared/execution/executor/impl/asio_executor.h"
#include "shared/network/session/session.h"
#include "sl/generator/api/generated/request.pb.h"
#include "sl/generator/api/generated/response.pb.h"
#include "sl/generator/service/control/gateway/generator_control_api_gateway_cipher.h"
#include "sl/generator/service/control/gateway/generator_control_api_gateway_connection.h"
#include "sl/generator/service/control/gateway/handler/generator_control_reqeust_router.h"
#include "sl/generator/service/control/gateway/handler/generator_control_request_registry.h"

namespace sunlight
{
    GeneratorControlAPIGatewayServer::GeneratorControlAPIGatewayServer(const ServiceLocator& serviceLocator, execution::AsioExecutor& executor)
        : Server(serviceLocator, std::string(GetName()), executor)
        , _router(std::make_shared<GeneratorControlRequestRouter>())
        , _handlerRegistry(std::make_shared<GeneratorControlRequestRegistry>())
    {
        [[maybe_unused]]
        const bool registered = _handlerRegistry->Register(*_router, serviceLocator);
        assert(registered);
    }

    void GeneratorControlAPIGatewayServer::OnAccept(Session& session)
    {
        auto strand = std::make_shared<Strand>(GetExecutor().SharedFromThis());

        auto connection = std::make_shared<GeneratorControlAPIGatewayConnection>(strand,
            session.shared_from_this(), std::make_shared<GeneratorControlAPIGatewayCipher>("klasgdghasdhgfwiqufdvasbdjhkfqwygifaqsnmbfasdhjqwed"));

        {
            decltype(_connections)::accessor accessor;

            if (_connections.insert(accessor, session.GetId()))
            {
                accessor->second = std::move(connection);
            }
            else
            {
                assert(false);

                return;
            }
        }

        /*Delay(std::chrono::seconds(3)).Then(*strand,
            [holder = shared_from_this(), this, id = session.GetId()]()
            {
                const SharedPtrNotNull<GeneratorControlAPIGatewayConnection>& connection = FindConnection(id);

                if (!connection)
                {
                    return;
                }

                if (connection->GetState() == GeneratorControlAPIGatewayConnection::State::Connected)
                {
                    connection->Close();
                }
            });*/
    }

    void GeneratorControlAPIGatewayServer::OnReceive(Session& session, Buffer buffer)
    {
        const SharedPtrNotNull<GeneratorControlAPIGatewayConnection>& connection = FindConnection(session.GetId());

        Post(connection->GetStrand(), [holder = shared_from_this(), this, connection, buffer = std::move(buffer)]() mutable
            {
                connection->Receive(std::move(buffer));

                try
                {
                    while (true)
                    {
                        std::optional<api::Request> request = connection->ParseReceived();
                        if (!request.has_value())
                        {
                            break;
                        }

                        const int32_t requestId = request->request_id();

                        _router->Route(connection, std::move(*request)).ContinuationWith(GetExecutor(),
                            [holder = std::move(holder), this, requestId, connection](Future<void>& future)
                            {
                                try
                                {
                                    future.Get();
                                }
                                catch (const std::exception& e)
                                {
                                    SUNLIGHT_LOG_ERROR(GetServiceLocator(),
                                        fmt::format("[{}] fail to handle request. session: {}, request_id: {}, exception: {}",
                                            GetName(), connection->GetSessionId(), requestId, e.what()));

                                    connection->Close();
                                }
                            });
                    }

                }
                catch (const std::exception& e)
                {
                    SUNLIGHT_LOG_ERROR(GetServiceLocator(), fmt::format("[{}] fail to parse request. session: {}, exception: {}",
                        GetName(), connection->GetSessionId(), e.what()));
                }
            });
    }

    void GeneratorControlAPIGatewayServer::OnError(Session& session, const boost::system::error_code& error)
    {
        SUNLIGHT_LOG_INFO(GetServiceLocator(),
            fmt::format("[{}] session disconnected. id: {}, error: {}",
                GetName(), session.GetId(), error.message()));

        _connections.erase(session.GetId());
    }

    auto GeneratorControlAPIGatewayServer::GetName() -> std::string_view
    {
        return "generator_control_server";
    }

    auto GeneratorControlAPIGatewayServer::FindConnection(session::id_type id) -> SharedPtrNotNull<GeneratorControlAPIGatewayConnection>
    {
        decltype(_connections)::const_accessor accessor;

        if (_connections.find(accessor, id))
        {
            return accessor->second;
        }

        return {};
    }

    auto GeneratorControlAPIGatewayServer::FindConnection(session::id_type id) const -> SharedPtrNotNull<const GeneratorControlAPIGatewayConnection>
    {
        decltype(_connections)::const_accessor accessor;

        if (_connections.find(accessor, id))
        {
            return accessor->second;
        }

        return {};
    }
}
