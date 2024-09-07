#pragma once
#include <atomic>
#include <concepts>
#include <mutex>
#include <functional>
#include <optional>
#include "shared/execution/context/cancelable_interface.h"
#include "shared/execution/future/future_status.h"

namespace sunlight::execution
{
    class IExecutor;
}

namespace sunlight::future
{
    class SharedContextBase : public execution::ICancelable
    {
    public:
        SharedContextBase(const SharedContextBase&) = delete;
        SharedContextBase(SharedContextBase&&) noexcept = delete;
        SharedContextBase& operator=(const SharedContextBase&) = delete;
        SharedContextBase& operator=(SharedContextBase&&) noexcept = delete;

        SharedContextBase() = default;
        ~SharedContextBase() override = default;

        bool IsPending() const noexcept;
        bool IsComplete() const noexcept;
        bool IsCanceled() const noexcept final;

        void Wait();

        bool Cancel() final;
        void OnFailure(const std::exception_ptr& exception);

        void SetContinuation(std::move_only_function<void()> function);
        void SetExecutor(std::shared_ptr<execution::IExecutor> executor) noexcept;

        auto GetExecutor() const ->execution::IExecutor&;

    protected:
        std::mutex _mutex;
        std::move_only_function<void()> _continuation = {};
        std::exception_ptr _exception = nullptr;
        std::atomic<FutureStatus> _status = FutureStatus::Pending;
        std::shared_ptr<execution::IExecutor> _executor;
    };

    template <typename T>
    class SharedContext;

    template <>
    class SharedContext<void> : public SharedContextBase
    {
    public:
        void OnSuccess();
        void Get();

        void Reset();
    };

    template <std::move_constructible T>
    class SharedContext<T> : public SharedContextBase
    {
    public:
        void OnSuccess(T&& value);
        void OnSuccess(const T& value);

        auto Get() -> T;

        void Reset();

    private:
        std::optional<T> _value = std::nullopt;
    };

    template <std::move_constructible T>
    void SharedContext<T>::OnSuccess(T&& value)
    {
        std::move_only_function<void()> continuation;
        {
            std::lock_guard lock(_mutex);

            FutureStatus expected = FutureStatus::Pending;
            if (!_status.compare_exchange_strong(expected, FutureStatus::Complete))
            {
                return;
            }

            _value = std::move(value);
            _continuation.swap(continuation);
        }

        _status.notify_one();

        if (continuation)
        {
            continuation();
        }
    }

    template <std::move_constructible T>
    void SharedContext<T>::OnSuccess(const T& value)
    {
        T temp(value);

        OnSuccess(std::move(temp));
    }

    template <std::move_constructible T>
    auto SharedContext<T>::Get() -> T
    {
        switch (_status.load())
        {
        case FutureStatus::Pending:
        {
            Wait();
            return Get();
        }
        case FutureStatus::Complete:
        {
            std::optional<T> value = std::nullopt;
            {
                std::lock_guard lock(_mutex);
                if (_exception)
                {
                    std::rethrow_exception(std::exchange(_exception, nullptr));
                }

                if (_value.has_value())
                {
                    _value.swap(value);
                }
            }

            if (!value.has_value())
            {
                throw std::runtime_error("value is already used");
            }

            return std::move(value.value());
        }
        case FutureStatus::Canceled:
        default:
            throw std::runtime_error("operation aborted");
        }
    }

    template <std::move_constructible T>
    void SharedContext<T>::Reset()
    {
        std::lock_guard lock(_mutex);

        _continuation = {};
        _exception = nullptr;
        _status.store(FutureStatus::Pending);
        _value.reset();
    }
}
