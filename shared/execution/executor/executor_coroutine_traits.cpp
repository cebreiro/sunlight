#include "executor_coroutine_traits.h"

#include "shared/execution/context/execution_context.h"
#include "shared/execution/executor/operation/post.h"

namespace sunlight::execution
{
    ExecutorAwaiter::ExecutorAwaiter(SharedPtrNotNull<IExecutor> executor)
        : _executor(std::move(executor))
    {
    }

    bool ExecutorAwaiter::await_ready() const
    {
        return ExecutionContext::IsEqualTo(*_executor);
    }

    void ExecutorAwaiter::await_suspend(std::coroutine_handle<> handle)
    {
        Post(*_executor, [handle]()
            {
                handle();
            });
    }

    void ExecutorAwaiter::await_resume()
    {
    }

    auto operator co_await(IExecutor& executor) -> ExecutorAwaiter
    {
        return ExecutorAwaiter(executor.SharedFromThis());
    }
}
