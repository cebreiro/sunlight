#pragma once
#include <vector>
#include <thread>
#include "shared/type/not_null_pointer.h"

namespace sunlight
{
    namespace execution
    {
        class IExecutor;
        class ICancelable;
    }

    class ExecutionContext
    {
    public:
        ExecutionContext() = delete;

    public:
        static void PushExecutor(PtrNotNull<execution::IExecutor> executor, std::thread::id id = std::this_thread::get_id());
        static void PopExecutor(std::thread::id id = std::this_thread::get_id());

        static void PushCancelable(PtrNotNull<execution::ICancelable> cancelable, std::thread::id id = std::this_thread::get_id());
        static void PopCancelable(std::thread::id id = std::this_thread::get_id());

    public:
        static bool IsEqualTo(const execution::IExecutor& e);
        static bool Contains(const execution::IExecutor& e);

        static auto GetExecutor() -> execution::IExecutor*;
        static auto GetCancelable() -> execution::ICancelable&;

    public:
        struct ExecutorGuard
        {
            ExecutorGuard() = delete;

            explicit ExecutorGuard(PtrNotNull<execution::IExecutor> executor);
            ~ExecutorGuard();
        };

        struct CancelableGuard
        {
            CancelableGuard() = delete;

            explicit CancelableGuard(PtrNotNull<execution::ICancelable> cancelable);
            ~CancelableGuard();
        };

    private:
        static thread_local std::vector<std::pair<execution::IExecutor*, std::thread::id>> _executors;
        static thread_local std::vector<std::pair<execution::ICancelable*, std::thread::id>> _cancelables;
    };
}
