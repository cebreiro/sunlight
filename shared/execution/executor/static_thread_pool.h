#pragma once
#include <chrono>
#include "shared/execution/executor/executor_interface.h"

namespace sunlight
{
    class StaticThreadPool
    {
        StaticThreadPool();

    public:
        void Delay(const std::function<void()>& function, std::chrono::milliseconds milliseconds);
        void Delay(std::move_only_function<void()> function, std::chrono::milliseconds milliseconds);

        auto GetConcurrency() const -> int64_t;

        operator execution::IExecutor& ();
        operator const execution::IExecutor& () const;

        static auto GetInstance() -> StaticThreadPool&;

    private:
        int64_t _concurrency = 1;
        SharedPtrNotNull<execution::IExecutor> _executor;
    };
}
