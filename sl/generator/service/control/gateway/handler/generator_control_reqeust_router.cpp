#include "generator_control_reqeust_router.h"

#include "sl/generator/api/generated/request.pb.h"

namespace sunlight
{
    auto GeneratorControlRequestRouter::Route(SharedPtrNotNull<GeneratorControlAPIGatewayConnection> connection, api::Request request) -> Future<void>
    {
        const google::protobuf::Reflection* reflection = api::Request::GetReflection();
        const google::protobuf::Descriptor* descriptor = api::Request::GetDescriptor();

        assert(reflection && descriptor);

        const google::protobuf::OneofDescriptor* oneOfDescriptor = descriptor->FindOneofByName("payload");
        if (!oneOfDescriptor)
        {
            throw std::runtime_error("payload 'oneof' not found");
        }

        const google::protobuf::FieldDescriptor* oneOfFieldDescriptor = reflection->GetOneofFieldDescriptor(request, oneOfDescriptor);
        if (!oneOfFieldDescriptor)
        {
            throw std::runtime_error("'oneof' field is null");
        }

        if (oneOfFieldDescriptor->cpp_type() != google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE)
        {
            throw std::runtime_error("'oneof' field's type is not google::protobuf::Message");
        }

        const google::protobuf::Message& message = reflection->GetMessage(request, oneOfFieldDescriptor);

        const std::string& name = message.GetDescriptor()->name();

        const auto iter = _handlers.find(name);
        if (iter == _handlers.end())
        {
            throw std::runtime_error(fmt::format("fail to find handler. field name: {}", name));
        }

        co_await iter->second(std::move(connection), request.request_id(), message);

        co_return;
    }
}
