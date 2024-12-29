#pragma once
#include <exception>
#include <type_traits>
#include <boost/callable_traits.hpp>
#include "shared/execution/context/execution_context.h"
#include "shared/execution/executor/operation/post.h"
#include "shared/execution/future/future.h"

namespace sunlight::execution
{
    template <typename Callable, typename Result = boost::callable_traits::return_type_t<Callable>>
    auto StartAsync(IExecutor& executor, Callable&& callable) -> Future<Result>
    {
        auto context = std::make_shared<future::SharedContext<Result>>();
        context->SetExecutor(executor.SharedFromThis());

        Post(executor, [context, callable = std::forward<Callable>(callable)]() mutable
            {
                ExecutionContext::CancelableGuard guard(context.get());

                try
                {
                    if constexpr (std::is_same_v<Result, void>)
                    {
                        callable();
                        context->OnSuccess();
                    }
                    else
                    {
                        context->OnSuccess(callable());
                    }
                }
                catch (...)
                {
                    context->OnFailure(std::current_exception());
                }
            });

        return Future<Result>(std::move(context));
    }

    template <typename Callable, typename... Args, typename Result = boost::callable_traits::return_type_t<Callable>>
    auto StartAsync(IExecutor& executor, Callable&& callable, Args&&... args)
        -> std::enable_if_t<std::is_invocable_v<Callable, Args...>, Future<Result>>
    {
        auto context = std::make_shared<future::SharedContext<Result>>();
        context->SetExecutor(executor.SharedFromThis());

        Post(executor, [context, callable = std::forward<Callable>(callable), ...args = std::forward<Args>(args)]() mutable
            {
                ExecutionContext::CancelableGuard guard(context.get());

                try
                {
                    if constexpr (std::is_same_v<Result, void>)
                    {
                        callable(std::forward<Args>(args)...);
                        context->OnSuccess();
                    }
                    else
                    {
                        context->OnSuccess(callable(std::forward<Args>(args)...));
                    }
                }
                catch (...)
                {
                    context->OnFailure(std::current_exception());
                }
            });

        return Future<Result>(std::move(context));
    }
}
