#include "promise.h"

namespace sunlight
{
    bool Promise<void>::IsCanceled() const noexcept
    {
        return _context->IsCanceled();
    }

    void Promise<void>::Set()
    {
        return _context->OnSuccess();
    }

    void Promise<void>::SetException(const std::exception_ptr& exception)
    {
        _context->OnFailure(exception);
    }

    void Promise<void>::Reset()
    {
        _context->Reset();
    }

    auto Promise<void>::GetFuture() const -> Future<void>
    {
        return Future<void>(_context);
    }
}
