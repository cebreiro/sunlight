#include "log_ring_buffer.h"

#include "sl/generator/service/log_memcache/log_mem_cache_query_option.h"

namespace sunlight
{
    LogRingBuffer::LogRingBuffer(int64_t ringBufferSize)
        : _ringBufferSize(ringBufferSize)
    {
        for (boost::circular_buffer<Item>& ringBuffer : _buffers)
        {
            ringBuffer.set_capacity(std::max(_ringBufferSize, ring_buffer_min_size));
        }
    }

    void LogRingBuffer::Add(LogLevel logLevel, std::string log)
    {
        GetBuffer(logLevel).push_back(Item{
            .dateTime = log_mem_cache_date_time_type::clock::now(),
            .log = std::move(log),
            });
    }

    void LogRingBuffer::Get(std::vector<LogMemCacheQueryResult>& result, const LogMemCacheQueryOption& option) const
    {
        const auto dateLowerBound = option.dateLowerBound.value_or(log_mem_cache_date_time_type::min());
        const auto dateUpperBound = option.dateUpperBound.value_or(log_mem_cache_date_time_type::max());

        if (dateLowerBound > dateUpperBound)
        {
            assert(false);

            return;
        }

        const boost::circular_buffer<Item>& buffer = GetBuffer(option.logLevel);
        if (buffer.empty())
        {
            return;
        }

        const auto predicate = [](const Item& item, log_mem_cache_date_time_type dateTime)
            {
                return item.dateTime < dateTime;
            };

        auto begin = std::lower_bound(buffer.begin(), buffer.end(), dateLowerBound, predicate);
        auto end = std::lower_bound(begin, buffer.end(), dateUpperBound, predicate);

        for (auto iter = begin; iter != end; ++iter)
        {
            result.emplace_back(LogMemCacheQueryResult{
                .logLevel = option.logLevel,
                .dateTime = iter->dateTime,
                .log = iter->log,
                });
        }
    }

    void LogRingBuffer::Clear()
    {
        Clear(LogLevel::Debug);
        Clear(LogLevel::Info);
        Clear(LogLevel::Warn);
        Clear(LogLevel::Error);
        Clear(LogLevel::Critical);
    }

    void LogRingBuffer::Clear(LogLevel logLevel)
    {
        GetBuffer(logLevel).clear();
    }

    auto LogRingBuffer::GetBuffer(LogLevel logLevel) -> boost::circular_buffer<Item>&
    {
        const int64_t index = static_cast<int64_t>(logLevel);
        assert(index >= 0 && index < std::ssize(_buffers));

        return _buffers[index];
    }

    auto LogRingBuffer::GetBuffer(LogLevel logLevel) const -> const boost::circular_buffer<Item>&
    {
        const int64_t index = static_cast<int64_t>(logLevel);
        assert(index >= 0 && index < std::ssize(_buffers));

        return _buffers[index];
    }
}
