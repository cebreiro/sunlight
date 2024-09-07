#include "snowflake_service.h"

#include "shared/snowflake/snowflake.h"

namespace sunlight
{
    SnowflakeService::SnowflakeService(const ServiceLocator& serviceLocator, execution::IExecutor& executor)
        : _serviceLocator(serviceLocator)
        , _strand(std::make_shared<Strand>(executor.SharedFromThis()))
    {
    }

    auto SnowflakeService::GetName() const -> std::string_view
    {
        return "snowflake_service";
    }

    auto SnowflakeService::Publish(SnowflakeCategory category) -> Future<std::optional<int32_t>>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        co_return PublishKey(category);
    }

    auto SnowflakeService::TakeBack(SnowflakeCategory category, int32_t value) -> Future<void>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        co_await *_strand;
        assert(ExecutionContext::IsEqualTo(*_strand));

        Get(category).recycleQueue.push(value);

        co_return;
    }

    auto SnowflakeService::PublishKey(SnowflakeCategory category) -> std::optional<int32_t>
    {
        Context& context = Get(category);

        if (!context.recycleQueue.empty())
        {
            const int32_t front = context.recycleQueue.front();
            context.recycleQueue.pop();

            return front;
        }

        constexpr int32_t max = (1 << (snowflake::MACHINE_BIT + snowflake::DATACENTER_BIT));

        if (int32_t key = context.nextKey++; key <= max)
        {
            return key;
        }

        return std::nullopt;
    }

    auto SnowflakeService::Get(SnowflakeCategory category) -> Context&
    {
        const int64_t index = static_cast<int64_t>(category);
        assert(index >= 0 && index < std::ssize(_contexts));

        return _contexts[index];
    }
}
