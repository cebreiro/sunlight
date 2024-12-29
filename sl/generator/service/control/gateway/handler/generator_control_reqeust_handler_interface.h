#pragma once

namespace sunlight
{
    class GeneratorControlAPIGatewayConnection;
}

namespace sunlight
{
    class IGeneratorControlRequestHandler
    {
    public:
        virtual ~IGeneratorControlRequestHandler() = default;
    };

    template <typename T>
    class GeneratorControlRequestHandlerT : public IGeneratorControlRequestHandler
    {
    public:
        using request_type = T;

    public:
        virtual auto HandleRequest(GeneratorControlAPIGatewayConnection& connection, int32_t requestId, const T& request) -> Future<void> = 0;
    };
}
