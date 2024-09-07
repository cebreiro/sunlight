#pragma once
#include <coroutine>
#include "shared/execution/executor/executor_interface.h"

namespace sunlight::execution
{
    class ExecutorAwaiter
    {
    public:
        explicit ExecutorAwaiter(SharedPtrNotNull<IExecutor> executor);

        bool await_ready() const;
        void await_suspend(std::coroutine_handle<> handle);
        void await_resume();

    private:
        std::shared_ptr<IExecutor> _executor;
    };

    auto operator co_await(IExecutor& executor) -> ExecutorAwaiter;
}
