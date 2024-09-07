#pragma once
#include <cstdint>
#include <cassert>
#include <concepts>
#include <coroutine>
#include <memory>
#include <stdexcept>
#include <optional>
#include "shared/execution/channel/channel.h"
#include "shared/execution/context/execution_context.h"
#include "shared/execution/executor/operation/dispatch.h"

namespace sunlight
{
    struct AsyncEnumerableClosedException : std::exception
    {
        AsyncEnumerableClosedException() = default;
        explicit AsyncEnumerableClosedException(const char* str)
            : std::exception(str)
        {
        }
    };

    template <std::move_constructible T, typename TChannel = Channel<T>>
    class AsyncEnumerable
    {
    public:
        using channel_type = TChannel;
        using value_type = typename Channel<T>::value_type;
        using item_type = typename Channel<T>::item_type;

    public:
        class Enumerator
        {
        public:
            explicit Enumerator(SharedPtrNotNull<Channel<T>> channel);

            bool await_ready();
            void await_suspend(std::coroutine_handle<> handle);
            auto await_resume() -> value_type;

            auto GetChannel() const -> const SharedPtrNotNull<channel_type>&;

        private:
            void RegisterHandleForCoroutineResume(SharedPtrNotNull<execution::IExecutor> ex, std::coroutine_handle<> handle);
            bool TrySetCurrent();

        private:
            SharedPtrNotNull<channel_type> _channel;
            std::optional<item_type> _current = std::nullopt;
        };

        class promise_type
        {
        public:
            auto get_return_object() -> AsyncEnumerable;
            auto initial_suspend() -> std::suspend_never;
            auto final_suspend() noexcept -> std::suspend_never;

            template <std::ranges::range R> requires std::convertible_to<std::ranges::range_value_t<R>, T>
            auto yield_value(R&& range) -> std::suspend_never
            {
                _channel->Send(std::forward<R>(range), channel::ChannelSignal::NotifyOne);

                return std::suspend_never{};
            }
            auto yield_value(T value) -> std::suspend_never;
            auto yield_value(const std::exception_ptr& exception) -> std::suspend_never;

            void return_void();
            void unhandled_exception();

        private:
            SharedPtrNotNull<channel_type> _channel = std::make_shared<channel_type>();
        };

    public:
        AsyncEnumerable(const AsyncEnumerable& other) = delete;
        AsyncEnumerable& operator=(const AsyncEnumerable& other) = delete;

        AsyncEnumerable() = default;
        ~AsyncEnumerable() = default;
        AsyncEnumerable(AsyncEnumerable&& other) noexcept = default;
        AsyncEnumerable& operator=(AsyncEnumerable&& other) noexcept = default;

        explicit AsyncEnumerable(SharedPtrNotNull<channel_type> channel);

        bool HasNext() const;

        auto GetChannel() const -> const std::shared_ptr<channel_type>&;

        auto operator co_await() const -> Enumerator;

    private:
        SharedPtrNotNull<channel_type> _channel;
    };

    template <std::move_constructible T, typename TChannel>
    auto AsyncEnumerable<T, TChannel>::Enumerator::GetChannel() const -> const SharedPtrNotNull<channel_type>&
    {
        return _channel;
    }

    template <std::move_constructible T, typename TChannel>
    auto AsyncEnumerable<T, TChannel>::promise_type::get_return_object() -> AsyncEnumerable
    {
        return AsyncEnumerable(_channel);
    }

    template <std::move_constructible T, typename TChannel>
    auto AsyncEnumerable<T, TChannel>::promise_type::initial_suspend() -> std::suspend_never
    {
        return std::suspend_never{};
    }

    template <std::move_constructible T, typename TChannel>
    auto AsyncEnumerable<T, TChannel>::promise_type::final_suspend() noexcept -> std::suspend_never
    {
        return std::suspend_never{};
    }

    template <std::move_constructible T, typename TChannel>
    auto AsyncEnumerable<T, TChannel>::promise_type::yield_value(T value) -> std::suspend_never
    {
        _channel->Send(std::move(value), channel::ChannelSignal::NotifyOne);

        return std::suspend_never{};
    }

    template <std::move_constructible T, typename TChannel>
    auto AsyncEnumerable<T, TChannel>::promise_type::yield_value(const std::exception_ptr& exception) -> std::suspend_never
    {
        _channel->Send(exception, channel::ChannelSignal::NotifyOne);

        return std::suspend_never{};
    }

    template <std::move_constructible T, typename TChannel>
    void AsyncEnumerable<T, TChannel>::promise_type::return_void()
    {
        [[maybe_unused]] const bool closed = _channel->Close();
        assert(closed);
    }

    template <std::move_constructible T, typename TChannel>
    void AsyncEnumerable<T, TChannel>::promise_type::unhandled_exception()
    {
        _channel->Send(std::current_exception());
        [[maybe_unused]] const bool closed = _channel->Close();
        assert(closed);
    }

    template <std::move_constructible T, typename TChannel>
    AsyncEnumerable<T, TChannel>::AsyncEnumerable(SharedPtrNotNull<channel_type> channel)
        : _channel(std::move(channel))
    {
    }

    template <std::move_constructible T, typename TChannel>
    bool AsyncEnumerable<T, TChannel>::HasNext() const
    {
        if (_channel->IsOpen())
        {
            return true;
        }

        return !_channel->Empty();
    }

    template <std::move_constructible T, typename TChannel>
    auto AsyncEnumerable<T, TChannel>::GetChannel() const -> const std::shared_ptr<channel_type>&
    {
        return _channel;
    }

    template <std::move_constructible T, typename TChannel>
    auto AsyncEnumerable<T, TChannel>::operator co_await() const -> Enumerator
    {
        return Enumerator(_channel);
    }

    template <std::move_constructible T, typename TChannel>
    AsyncEnumerable<T, TChannel>::Enumerator::Enumerator(SharedPtrNotNull<Channel<T>> channel)
        : _channel(std::move(channel))
    {
    }

    template <std::move_constructible T, typename TChannel>
    bool AsyncEnumerable<T, TChannel>::Enumerator::await_ready()
    {
        if (TrySetCurrent())
        {
            return true;
        }

        return _channel->IsClosed();
    }

    template <std::move_constructible T, typename TChannel>
    void AsyncEnumerable<T, TChannel>::Enumerator::await_suspend(std::coroutine_handle<> handle)
    {
        assert(handle);

        execution::IExecutor* executor = ExecutionContext::GetExecutor();
        assert(executor);

        RegisterHandleForCoroutineResume(executor->SharedFromThis(), handle);
    }

    template <std::move_constructible T, typename TChannel>
    auto AsyncEnumerable<T, TChannel>::Enumerator::await_resume() -> value_type
    {
        if (!_current.has_value())
        {
            assert(_channel->IsClosed());
            throw AsyncEnumerableClosedException("channel is closed by producer");
        }

        item_type temp;
        std::swap(temp, _current.value());
        _current.reset();

        if (std::holds_alternative<std::exception_ptr>(temp))
        {
            std::rethrow_exception(std::get<std::exception_ptr>(temp));
        }

        return std::move(std::get<value_type>(temp));
    }

    template <std::move_constructible T, typename TChannel>
    void AsyncEnumerable<T, TChannel>::Enumerator::RegisterHandleForCoroutineResume(SharedPtrNotNull<execution::IExecutor> ex,
        std::coroutine_handle<> handle)
    {
        _channel->AddOrExecuteSignalHandler([this, ex = std::move(ex), handle]() mutable
        {
            execution::IExecutor& executor = *ex;

            Dispatch(executor, [ex = std::move(ex), this, handle]() mutable
            {
                if (TrySetCurrent() || _channel->IsClosed())
                {
                    handle.resume();
                }
                else
                {
                    RegisterHandleForCoroutineResume(std::move(ex), handle);
                }
            });
        });
    }

    template <std::move_constructible T, typename TChannel>
    bool AsyncEnumerable<T, TChannel>::Enumerator::TrySetCurrent()
    {
        assert(!_current.has_value());

        item_type item;
        if (_channel->TryPop(item))
        {
            _current = std::move(item);
            return true;
        }

        return false;
    }
}
