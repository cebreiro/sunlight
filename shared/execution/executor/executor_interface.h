#pragma once
#include <memory>
#include <functional>
#include "shared/type/not_null_pointer.h"

namespace sunlight::execution
{
    class IExecutor
    {
    public:
        IExecutor(const IExecutor& other) = delete;
        IExecutor(IExecutor&& other) noexcept = delete;
        IExecutor& operator=(const IExecutor& other) = delete;
        IExecutor& operator=(IExecutor&& other) noexcept = delete;

    public:
        IExecutor() = default;
        virtual ~IExecutor() = default;

        virtual void Stop() = 0;

        virtual void Post(const std::function<void()>& function) = 0;
        virtual void Post(std::move_only_function<void()> function) = 0;

        virtual void Dispatch(const std::function<void()>& function) = 0;
        virtual void Dispatch(std::move_only_function<void()> function) = 0;

        virtual auto GetConcurrency() const -> int64_t = 0;

        virtual auto SharedFromThis() -> SharedPtrNotNull<IExecutor> = 0;
        virtual auto SharedFromThis() const -> SharedPtrNotNull<const IExecutor> = 0;
    };
}
