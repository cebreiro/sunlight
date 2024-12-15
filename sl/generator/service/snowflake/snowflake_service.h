#pragma once
#include "sl/generator/service/snowflake/category.h"

namespace sunlight
{
    class SnowflakeService final
        : public IService
        , public std::enable_shared_from_this<SnowflakeService>
    {
    public:
        SnowflakeService(const ServiceLocator& serviceLocator, execution::IExecutor& executor);

        auto GetName() const -> std::string_view override;

    public:
        auto Publish(SnowflakeCategory category) -> Future<std::optional<int32_t>>;
        auto TakeBack(SnowflakeCategory category, int32_t value) -> Future<void>;

    private:
        auto PublishKey(SnowflakeCategory category) -> std::optional<int32_t>;

        struct Context
        {
            int32_t nextKey = 0;
            std::queue<int32_t> recycleQueue;
        };

        auto Get(SnowflakeCategory category) -> Context&;

    private:
        const ServiceLocator& _serviceLocator;
        SharedPtrNotNull<Strand> _strand;

        std::array<Context, SnowflakeCategoryMeta::GetSize()> _contexts;
    };
}
