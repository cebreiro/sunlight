#pragma once
#include "sl/generator/api/generated/request.pb.h"
#include "sl/generator/service/control/gateway/handler/generator_control_reqeust_handler_interface.h"

namespace sunlight
{
    class GeneratorControlRequestRouter
    {
    public:
        template <typename T> requires std::derived_from<T, google::protobuf::Message>
        bool AddHandler(SharedPtrNotNull<GeneratorControlRequestHandlerT<T>> requestHandler);

        auto Route(SharedPtrNotNull<GeneratorControlAPIGatewayConnection> connection, api::Request request) -> Future<void>;

    private:
        using handler_type = std::function<Future<void>(SharedPtrNotNull<GeneratorControlAPIGatewayConnection>, int32_t, const google::protobuf::Message&)>;

        std::unordered_map<std::string, handler_type> _handlers;
    };

    template <typename T> requires std::derived_from<T, google::protobuf::Message>
    bool GeneratorControlRequestRouter::AddHandler(SharedPtrNotNull<GeneratorControlRequestHandlerT<T>> requestHandler)
    {
        assert(requestHandler);

        const google::protobuf::Descriptor* descriptor = T::GetDescriptor();
        assert(descriptor);

        auto handler = [](SharedPtrNotNull<GeneratorControlAPIGatewayConnection> connection, int32_t requestId, const google::protobuf::Message& message,
            SharedPtrNotNull<GeneratorControlRequestHandlerT<T>> requestHandler, std::string destName) -> Future<void>
            {
                const T* casted = dynamic_cast<const T*>(&message);
                if (!casted)
                {
                    throw std::runtime_error(fmt::format("fail to dynamic_cast 'oneof' field. src: {}, dest: {}", message.GetDescriptor()->name(), destName));
                }

                co_await requestHandler->HandleRequest(*connection, requestId, *casted);

                co_return;
            };

        return _handlers.try_emplace(descriptor->name(), std::bind_back(handler, requestHandler, descriptor->name())).second;
    }
}
