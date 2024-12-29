#include "execution_context.h"

#include <algorithm>
#include <cassert>
#include <ranges>

namespace sunlight
{
    thread_local std::vector<std::pair<execution::IExecutor*, std::thread::id>> ExecutionContext::_executors;
    thread_local std::vector<std::pair<execution::ICancelable*, std::thread::id>> ExecutionContext::_cancelables;

    void ExecutionContext::PushExecutor(PtrNotNull<execution::IExecutor> executor, std::thread::id id)
    {
        _executors.emplace_back(executor, id);
    }

    void ExecutionContext::PopExecutor(std::thread::id id)
    {
        assert(!_executors.empty());
        assert(_executors.back().second == id);
        (void)id;

        _executors.pop_back();
    }

    void ExecutionContext::PushCancelable(PtrNotNull<execution::ICancelable> cancelable, std::thread::id id)
    {
        _cancelables.emplace_back(cancelable, id);
    }

    void ExecutionContext::PopCancelable(std::thread::id id)
    {
        assert(!_cancelables.empty());
        assert(_cancelables.back().second == id);
        (void)id;

        _cancelables.pop_back();
    }

    bool ExecutionContext::IsEqualTo(const execution::IExecutor& e)
    {
        return GetExecutor() == &e;
    }

    bool ExecutionContext::Contains(const execution::IExecutor& e)
    {
        return std::ranges::any_of(_executors | std::views::elements<0>,
            [e = &e](const execution::IExecutor* executor)
            {
                return executor == e;
            });
    }

    auto ExecutionContext::GetExecutor() -> execution::IExecutor*
    {
        return _executors.back().first;
    }

    auto ExecutionContext::GetCancelable() -> execution::ICancelable&
    {
        assert(!_cancelables.empty());
        return *_cancelables.back().first;
    }

    ExecutionContext::ExecutorGuard::ExecutorGuard(PtrNotNull<execution::IExecutor> executor)
    {
        PushExecutor(executor);
    }

    ExecutionContext::ExecutorGuard::~ExecutorGuard()
    {
        PopExecutor();
    }

    ExecutionContext::CancelableGuard::CancelableGuard(PtrNotNull<execution::ICancelable> cancelable)
    {
        PushCancelable(cancelable);
    }

    ExecutionContext::CancelableGuard::~CancelableGuard()
    {
        PopCancelable();
    }
}
