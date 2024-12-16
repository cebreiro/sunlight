#include "generator_control_api_gateway_server.h"

#include "shared/execution/executor/impl/asio_executor.h"
#include "shared/network/session/session.h"
#include "sl/generator/api/generated/request.pb.h"
#include "sl/generator/api/generated/response.pb.h"
#include "sl/generator/service/control/gateway/generator_control_api_gateway_connection.h"
#include "sl/generator/service/control/gateway/zero_copy_input_stream.h"
#include "sl/generator/service/control/gateway/handler/generator_control_reqeust_router.h"
#include "sl/generator/service/control/gateway/handler/impl/generator_control_reqeust_handler.hpp"

namespace sunlight
{
    GeneratorControlAPIGatewayServer::GeneratorControlAPIGatewayServer(execution::AsioExecutor& executor)
        : Server(std::string(GetName()), executor)
        , _router(std::make_shared<GeneratorControlRequestRouter>())
    {
    }

    void GeneratorControlAPIGatewayServer::Initialize(ServiceLocator& serviceLocator)
    {
        Server::Initialize(serviceLocator);

        _serviceLocator = serviceLocator;

        auto a = std::make_shared<GeneratorControlAuthenticationHandler>(_serviceLocator);

        static_assert(std::derived_from<GeneratorControlAuthenticationHandler, GeneratorControlRequestHandlerT<api::AuthenticationRequest>>);

        AddHandlerToRouter(a);
    }

    void GeneratorControlAPIGatewayServer::OnAccept(Session& session)
    {
        auto strand = std::make_shared<Strand>(GetExecutor().SharedFromThis());

        auto connection = std::make_shared<GeneratorControlAPIGatewayConnection>();
        connection->strand = strand;
        connection->state = GeneratorControlAPIGatewayConnection::State::Connected;
        connection->session = session.shared_from_this();
        connection->level = -1;

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

        Delay(std::chrono::seconds(3)).Then(*strand,
            [holder = shared_from_this(), this, id = session.GetId()]()
            {
                const SharedPtrNotNull<GeneratorControlAPIGatewayConnection>& connection = FindConnection(id);

                if (!connection)
                {
                    return;
                }

                if (connection->state == GeneratorControlAPIGatewayConnection::State::Connected)
                {
                    connection->session->Close();
                }
            });
    }

    void GeneratorControlAPIGatewayServer::OnReceive(Session& session, Buffer buffer)
    {
        const SharedPtrNotNull<GeneratorControlAPIGatewayConnection>& connection = FindConnection(session.GetId());

        Post(*connection->strand, [holder = shared_from_this(), this, connection, buffer = std::move(buffer)]() mutable
            {
                Buffer& receiveBuffer = connection->receiveBuffer;

                receiveBuffer.MergeBack(std::move(buffer));

                const int64_t receivedSize = receiveBuffer.GetSize();
                if (receivedSize < 4)
                {
                    return;
                }

                BufferReader reader(receiveBuffer.cbegin(), receiveBuffer.cend());

                const int32_t packetSize = reader.Read<int32_t>();
                if (packetSize < 4)
                {
                    connection->session->Close();

                    return;
                }

                if (packetSize < receivedSize)
                {
                    return;
                }

                Buffer packetBuffer;

                [[maybe_unused]]
                const bool sliced = receiveBuffer.SliceFront(packetBuffer, packetSize);
                assert(sliced);

                Buffer temp;
                packetBuffer.SliceFront(temp, 4);

                ZeroCopyInputStream inputStream(packetBuffer);
                google::protobuf::io::CodedInputStream codedInputStream(&inputStream);

                api::Request request;

                if (request.ParseFromCodedStream(&codedInputStream))
                {
                    const int32_t requestId = request.request_id();

                    _router->Route(connection, std::move(request)).ContinuationWith(GetExecutor(),
                        [holder = std::move(holder), this, requestId, connection](Future<void>& future)
                        {
                            try
                            {
                                future.Get();
                            }
                            catch (const std::exception& e)
                            {
                                SUNLIGHT_LOG_ERROR(_serviceLocator,
                                    fmt::format("[{}] fail to handle request. session: {}, request_id: {}, exception: {}",
                                        GetName(), connection->session->GetId(), requestId, e.what()));

                                connection->session->Close();
                            }
                        });
                }
                else
                {
                    SUNLIGHT_LOG_ERROR(_serviceLocator, fmt::format("[{}] fail to parse request. session: {}, packet_size: {}, packet: {}",
                        GetName(), connection->session->GetId(), packetSize, packetBuffer.ToString()));
                }
            });
    }

    void GeneratorControlAPIGatewayServer::OnError(Session& session, const boost::system::error_code& error)
    {
        SUNLIGHT_LOG_INFO(_serviceLocator,
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

    template <typename T>
    void GeneratorControlAPIGatewayServer::AddHandlerToRouter(SharedPtrNotNull<T> handler)
    {
        static_assert(std::derived_from<T, IGeneratorControlRequestHandler>);

        [[maybe_unused]]
        const bool added = _router->AddHandler(std::dynamic_pointer_cast<GeneratorControlRequestHandlerT<typename T::request_type>>(handler));
        assert(added);

        _handlers.emplace_back(std::move(handler));
    }
}
