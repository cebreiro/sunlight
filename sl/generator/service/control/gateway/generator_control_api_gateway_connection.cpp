#include "generator_control_api_gateway_connection.h"

#include <boost/scope/scope_exit.hpp>
#include "shared/network/session/session.h"
#include "sl/generator/api/generated/response.pb.h"

namespace sunlight
{
    auto GeneratorControlAPIGatewayConnection::Send(api::Response response) -> Future<void>
    {
        if (!ExecutionContext::Contains(*strand))
        {
            co_await *strand;
        }

        assert(session);

        constexpr int64_t packetHeaderSize = 4;
        const int32_t bodySize = static_cast<int32_t>(response.ByteSizeLong());

        const int64_t packetSize = packetHeaderSize + bodySize;

        if (bufferPool.GetSize() < packetSize)
        {
            bufferPool.Add(buffer::Fragment::Create(std::max(int64_t{ 1024 }, packetSize)));
        }

        Buffer buffer;

        [[maybe_unused]]
        const bool sliced = bufferPool.SliceFront(buffer, packetSize);
        assert(sliced);

        char* stackBuffer = static_cast<char*>(_malloca(bodySize));

        boost::scope::scope_exit exit([stackBuffer]()
            {
                _freea(stackBuffer);
            });

        response.SerializeToArray(stackBuffer, bodySize);

        BufferWriter writer(buffer);
        writer.Write<int32_t>(bodySize + 4);
        writer.WriteBuffer(std::span<const char>(stackBuffer, bodySize));

        session->Send(std::move(buffer));
    }
}
