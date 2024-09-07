#pragma once
#include "shared/execution/future/future.h"

namespace sunlight
{
    template <typename T>
    class Promise;

    template <>
    class Promise<void>
    {
    public:
        using value_type = void;
        using context_type = future::SharedContext<value_type>;

    public:
        bool IsCanceled() const noexcept;

        void Set();
        void SetException(const std::exception_ptr& exception);

        void Reset();

        auto GetFuture() const -> Future<void>;

    private:
        SharedPtrNotNull<context_type> _context = std::make_shared<context_type>();
    };

    template <std::move_constructible T>
    class Promise<T>
    {
    public:
        using value_type = T;
        using context_type = future::SharedContext<value_type>;

    public:
        bool IsValid() const;
        bool IsCanceled() const noexcept;

        void Set(T value);
        void SetException(const std::exception_ptr& exception);

        void Reset();

        auto GetFuture() const -> Future<T>;

    private:
        SharedPtrNotNull<context_type> _context = std::make_shared<context_type>();
    };

    template <std::move_constructible T>
    void Promise<T>::Reset()
    {
        _context->Reset();
    }

    template <std::move_constructible T>
    bool Promise<T>::IsValid() const
    {
        return _context.operator bool();
    }

    template <std::move_constructible T>
    bool Promise<T>::IsCanceled() const noexcept
    {
        return _context->IsCanceled();
    }

    template <std::move_constructible T>
    void Promise<T>::Set(T value)
    {
        return _context->OnSuccess(std::move(value));
    }

    template <std::move_constructible T>
    void Promise<T>::SetException(const std::exception_ptr& exception)
    {
        _context->OnFailure(exception);
    }

    template <std::move_constructible T>
    auto Promise<T>::GetFuture() const -> Future<T>
    {
        return Future<T>(_context);
    }
}
