#include "schedule.h"

#include "shared/execution/future/future_coroutine_traits.h"
#include "shared/execution/future/operation/delay.h"

namespace sunlight::execution
{
    auto RepeatDelayExecution(SharedPtrNotNull<IExecutor> executor, std::function<void()> function,
        std::chrono::milliseconds delay, std::stop_token token) -> Future<void>
    {
        (void)executor;

        while (true)
        {
            co_await Delay(delay);
            assert(ExecutionContext::GetExecutor() == executor.get());

            if (token.stop_requested())
            {
                break;
            }

            function();
        }

        co_return;
    }

    auto Schedule(IExecutor& executor, const std::function<void()>& function, std::chrono::milliseconds firstDelay,
        std::chrono::milliseconds interval) -> std::stop_source
    {
        std::stop_source stopSource;
        std::stop_token token = stopSource.get_token();

        Delay(firstDelay).Then(executor, [function, token, e = executor.SharedFromThis(), interval]() mutable
            {
                if (token.stop_requested())
                {
                    return;
                }

                function();

                RepeatDelayExecution(std::move(e), function, interval, token);
            });


        return stopSource;
    }
}
