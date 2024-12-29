#pragma once
#include <boost/circular_buffer.hpp>
#include "sl/generator/service/log_memcache/log_mem_cache_query_result.h"

namespace sunlight
{
    struct LogMemCacheQueryOption;
}

namespace sunlight
{
    class LogRingBuffer
    {
    public:
        using date_time_type = std::chrono::system_clock::time_point;

    public:
        explicit LogRingBuffer(int64_t ringBufferSize);

        void Add(LogLevel logLevel, std::string log);

        void Get(std::vector<LogMemCacheQueryResult>& result, const LogMemCacheQueryOption& option) const;

        void Clear();
        void Clear(LogLevel logLevel);

    private:
        struct Item
        {
            std::chrono::system_clock::time_point dateTime = {};
            std::string log;
        };

        auto GetBuffer(LogLevel logLevel) -> boost::circular_buffer<Item>&;
        auto GetBuffer(LogLevel logLevel) const -> const boost::circular_buffer<Item>&;

    private:
        static constexpr int64_t array_size = static_cast<int32_t>(LogLevel::Critical) + 1;
        static constexpr int64_t ring_buffer_min_size = 256;

        int64_t _ringBufferSize = 0;
        std::array<boost::circular_buffer<Item>, array_size> _buffers;
    };
}
