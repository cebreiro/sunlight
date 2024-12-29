#pragma once
#include "shared/snowflake/snowflake.h"

namespace sunlight
{
    class GameItemUniqueIdPublisher final : public IService
    {
    public:
        GameItemUniqueIdPublisher(int32_t zoneId, int32_t snowflakeValue);

        auto GetName() const -> std::string_view override;

    public:
        auto Publish() -> int64_t;

    private:
        std::string _name;
        UniqueSnowflake<> _snowflake;
    };
}
