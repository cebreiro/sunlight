#include "generator_control_api_gateway_server.h"

#include "shared/execution/executor/impl/asio_executor.h"
#include "shared/network/session/session.h"
#include "sl/generator/api/generated/request.pb.h"
#include "sl/generator/api/generated/response.pb.h"
#include "sl/generator/service/control/gateway/zero_copy_input_stream.h"

namespace sunlight
{
    GeneratorControlAPIGatewayServer::GeneratorControlAPIGatewayServer(execution::AsioExecutor& executor)
        : Server(std::string(GetName()), executor)
    {
    }

    void GeneratorControlAPIGatewayServer::Initialize(ServiceLocator& serviceLocator)
    {
        Server::Initialize(serviceLocator);

        _serviceLocator = serviceLocator;
    }

    void GeneratorControlAPIGatewayServer::OnAccept(Session& session)
    {
        auto strand = std::make_shared<Strand>(GetExecutor().SharedFromThis());

        auto connection = std::make_shared<Connection>();
        connection->strand = strand;
        connection->state = Connection::State::Connected;
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
                const SharedPtrNotNull<Connection>& connection = FindConnection(id);

                if (!connection)
                {
                    return;
                }

                if (connection->state == Connection::State::Connected)
                {
                    connection->session->Close();
                }
            });
    }

    void GeneratorControlAPIGatewayServer::OnReceive(Session& session, Buffer buffer)
    {
        const SharedPtrNotNull<Connection>& connection = FindConnection(session.GetId());

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

                // temp test code
                Request request;

                if (request.ParseFromCodedStream(&codedInputStream))
                {
                    Response response;
                    response.set_request_id(request.request_id());

                    AuthenticationResponse* authentication = response.mutable_authentication();
                    authentication->set_success(false);

                    const int32_t bodySize = static_cast<int32_t>(response.ByteSizeLong());

                    buffer::Fragment body = buffer::Fragment::Create(bodySize);
                    response.SerializeToArray(body.GetData(), bodySize);

                    Buffer sendBuffer;
                    sendBuffer.Add(buffer::Fragment::Create(4));
                    sendBuffer.Add(std::move(body));

                    BufferWriter writer(sendBuffer);
                    writer.Write<int32_t>(bodySize + 4);

                    connection->session->Send(std::move(sendBuffer));

                    SUNLIGHT_LOG_INFO(_serviceLocator, fmt::format("[{}] parse success", GetName()));
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

    auto GeneratorControlAPIGatewayServer::FindConnection(session::id_type id) -> SharedPtrNotNull<Connection>
    {
        decltype(_connections)::const_accessor accessor;

        if (_connections.find(accessor, id))
        {
            return accessor->second;
        }

        return {};
    }

    auto GeneratorControlAPIGatewayServer::FindConnection(session::id_type id) const -> SharedPtrNotNull<const Connection>
    {
        decltype(_connections)::const_accessor accessor;

        if (_connections.find(accessor, id))
        {
            return accessor->second;
        }

        return {};
    }
}
