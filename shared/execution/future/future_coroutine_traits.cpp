#include "future_coroutine_traits.h"

namespace sunlight::future
{
    auto FuturePromise<void>::get_return_object() -> Future<void>
    {
        return Future<void>(_context);
    }

    auto FuturePromise<void>::initial_suspend() -> std::suspend_never
    {
        return std::suspend_never{};
    }

    auto FuturePromise<void>::final_suspend() noexcept -> std::suspend_never
    {
        return std::suspend_never{};
    }

    void FuturePromise<void>::unhandled_exception()
    {
        _context->OnFailure(std::current_exception());
    }

    void FuturePromise<void>::return_void()
    {
        _context->OnSuccess();
    }
}
