#pragma once
#include <concepts>
#include <coroutine>
#include "shared/execution/context/execution_context.h"
#include "shared/type/not_null_pointer.h"
#include "shared/execution/future/future.h"

namespace sunlight::future
{
    template <typename T>
    class FuturePromise;

    template <>
    class FuturePromise<void>
    {
    public:
        using context_type = Future<void>::context_type;

    public:
        auto get_return_object() -> Future<void>;
        auto initial_suspend() -> std::suspend_never;
        auto final_suspend() noexcept -> std::suspend_never;
        void unhandled_exception();
        void return_void();

    private:
        SharedPtrNotNull<context_type> _context = std::make_shared<context_type>();
    };

    template <std::move_constructible T>
    class FuturePromise<T>
    {
    public:
        using context_type = typename Future<T>::context_type;

    public:
        auto get_return_object() -> Future<T>;
        auto initial_suspend();
        auto final_suspend() noexcept;
        void unhandled_exception();
        void return_value(const T& value);
        void return_value(T&& value);

    private:
        SharedPtrNotNull<context_type> _context = std::make_shared<context_type>();
    };

    template <typename T>
    class FutureAwaiter
    {
    public:
        using context_type = typename Future<T>::context_type;

        explicit FutureAwaiter(Future<T> future);

        bool await_ready() const;
        void await_suspend(std::coroutine_handle<> handle);
        auto await_resume() -> decltype(auto);

    private:
        SharedPtrNotNull<context_type> _context;
    };

    template <std::move_constructible T>
    auto FuturePromise<T>::get_return_object() -> Future<T>
    {
        return Future<T>(_context);
    }

    template <std::move_constructible T>
    auto FuturePromise<T>::initial_suspend()
    {
        return std::suspend_never{};
    }

    template <std::move_constructible T>
    auto FuturePromise<T>::final_suspend() noexcept
    {
        return std::suspend_never{};
    }

    template <std::move_constructible T>
    void FuturePromise<T>::unhandled_exception()
    {
        _context->OnFailure(std::current_exception());
    }

    template <std::move_constructible T>
    void FuturePromise<T>::return_value(const T& value)
    {
        _context->OnSuccess(value);
    }

    template <std::move_constructible T>
    void FuturePromise<T>::return_value(T&& value)
    {
        _context->OnSuccess(std::move(value));
    }

    template <typename T>
    FutureAwaiter<T>::FutureAwaiter(Future<T> future)
        : _context(std::move(future._context))
    {
        assert(_context);
    }

    template <typename T>
    bool FutureAwaiter<T>::await_ready() const
    {
        return !_context->IsPending();
    }

    template <typename T>
    void FutureAwaiter<T>::await_suspend(std::coroutine_handle<> handle)
    {
        execution::IExecutor* executor = ExecutionContext::GetExecutor();
        assert(executor);

        Future<T>(_context).ContinuationWith(*executor,
            [handle]([[maybe_unused]] Future<T>& self)
            {
                handle.resume();
            });
    }

    template <typename T>
    auto FutureAwaiter<T>::await_resume() -> decltype(auto)
    {
        return _context->Get();
    }
}

namespace sunlight
{
    template <typename T>
    auto operator co_await(Future<T>& future) -> future::FutureAwaiter<T>
    {
        return future::FutureAwaiter<T>(std::move(future));
    }

    template <typename T>
    auto operator co_await(Future<T>&& future) -> future::FutureAwaiter<T>
    {
        return future::FutureAwaiter<T>(std::move(future));
    }
}
