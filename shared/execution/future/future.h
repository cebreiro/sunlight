#pragma once
#include <cassert>
#include <chrono>
#include <type_traits>
#include <boost/callable_traits.hpp>
#include "shared/type/not_null_pointer.h"
#include "shared/execution/context/execution_context.h"
#include "shared/execution/executor/executor_interface.h"
#include "shared/execution/executor/operation/post.h"
#include "shared/execution/future/shared_context.h"

namespace sunlight
{
    namespace future
    {
        template <typename T>
        class FuturePromise;

        template <typename T>
        class FutureAwaiter;
    }

    template <typename T>
    class Future
    {
    public:
        using value_type = T;
        using context_type = future::SharedContext<value_type>;
        using promise_type = future::FuturePromise<T>;

        template <typename U>
        friend class future::FutureAwaiter;

    public:
        Future(const Future&) = delete;
        Future& operator=(const Future&) = delete;

        Future() = default;
        Future(Future&& other) noexcept = default;
        Future& operator=(Future&& other) noexcept = default;

        explicit Future(SharedPtrNotNull<context_type> context);
        ~Future() = default;

        bool IsValid() const noexcept;
        bool IsPending() const noexcept;
        bool IsComplete() const noexcept;
        bool IsCanceled() const noexcept;

        bool Cancel();

        void Wait();

        auto Get() -> std::conditional_t<std::is_same_v<T, void>, void, T>;

        template <typename Callable, typename Result = boost::callable_traits::return_type_t<Callable>>
        auto Then(execution::IExecutor& executor, Callable&& callable) -> Future<Result>;

        template <typename Callable, typename Result = boost::callable_traits::return_type_t<Callable>>
        auto ContinuationWith(execution::IExecutor& executor, Callable&& callable) -> Future<Result>;

    public:
        friend bool operator==(const Future& lhs, const Future& rhs)
        {
            return lhs._context == rhs._context;
        }

        friend bool operator!=(const Future& lhs, const Future& rhs)
        {
            return lhs._context != rhs._context;
        }

    private:
        SharedPtrNotNull<context_type> _context;
    };

    template <typename T>
    Future<T>::Future(SharedPtrNotNull<context_type> context)
        : _context(std::move(context))
    {
    }

    template <typename T>
    bool Future<T>::IsValid() const noexcept
    {
        return _context.operator bool();
    }

    template <typename T>
    bool Future<T>::IsPending() const noexcept
    {
        assert(IsValid());
        return _context->IsPending();
    }

    template <typename T>
    bool Future<T>::IsComplete() const noexcept
    {
        assert(IsValid());
        return _context->IsComplete();
    }

    template <typename T>
    bool Future<T>::IsCanceled() const noexcept
    {
        assert(IsValid());
        return _context->IsCanceled();
    }

    template <typename T>
    bool Future<T>::Cancel()
    {
        assert(IsValid());
        return _context->Cancel();
    }

    template <typename T>
    void Future<T>::Wait()
    {
        assert(IsValid());
        _context->Wait();
    }

    template <typename T>
    auto Future<T>::Get() -> std::conditional_t<std::is_same_v<T, void>, void, T>
    {
        assert(IsValid());
        if constexpr (std::is_same_v<T, void>)
        {
            return _context->Get();
        }
        else
        {
            return std::move(_context->Get());
        }
    }

    template <typename T>
    template <typename Callable, typename Result>
    auto Future<T>::Then(execution::IExecutor& executor, Callable&& callable) -> Future<Result>
    {
        auto then = std::make_shared<future::SharedContext<Result>>();
        then->SetExecutor(executor.SharedFromThis());

        assert(IsValid());
        _context->SetContinuation([context = _context, then, callable = std::forward<Callable>(callable)]() mutable
            {
                execution::IExecutor& executor = then->GetExecutor();

                Post(executor, [context = std::move(context),
                    then = std::move(then), callable = std::forward<Callable>(callable)]() mutable
                    {
                        ExecutionContext::CancelableGuard guard(then.get());

                        try
                        {
                            if constexpr (std::tuple_size_v<boost::callable_traits::args_t<Callable>> == 1)
                            {
                                if constexpr (std::is_same_v<Result, void>)
                                {
                                    callable(context->Get());
                                    then->OnSuccess();
                                }
                                else
                                {
                                    then->OnSuccess(callable(context->Get()));
                                }
                            }
                            else
                            {
                                (void)context->Get();

                                if constexpr (std::is_same_v<Result, void>)
                                {
                                    callable();
                                    then->OnSuccess();
                                }
                                else
                                {
                                    then->OnSuccess(callable());
                                }
                            }
                        }
                        catch (...)
                        {
                            then->OnFailure(std::current_exception());
                        }

                    });
            });

        return Future<Result>(std::move(then));
    }

    template <typename T>
    template <typename Callable, typename Result>
    auto Future<T>::ContinuationWith(execution::IExecutor& executor, Callable&& callable) -> Future<Result>
    {
        auto continuation = std::make_shared<future::SharedContext<Result>>();
        continuation->SetExecutor(executor.SharedFromThis());

        assert(IsValid());
        _context->SetContinuation([context = _context, continuation, callable = std::forward<Callable>(callable)]() mutable
            {
                execution::IExecutor& executor = continuation->GetExecutor();

                Post(executor, [context = std::move(context),
                    continuation = std::move(continuation), callable = std::forward<Callable>(callable)]() mutable
                    {
                        ExecutionContext::CancelableGuard guard(continuation.get());

                        try
                        {
                            Future<T> temp(std::move(context));

                            if constexpr (std::is_same_v<Result, void>)
                            {
                                callable(temp);
                                continuation->OnSuccess();
                            }
                            else
                            {
                                continuation->OnSuccess(callable(temp));
                            }
                        }
                        catch (...)
                        {
                            continuation->OnFailure(std::current_exception());
                        }
                    });
            });

        return Future<Result>(std::move(continuation));
    }
}
