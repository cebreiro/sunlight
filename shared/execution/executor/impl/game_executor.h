#pragma once
#include <array>
#include <memory>
#include <condition_variable>
#include <variant>
#include <tbb/spin_mutex.h>
#include "shared/execution/executor/executor_interface.h"

namespace sunlight::execution
{
    class GameExecutor final
        : public IExecutor
        , public std::enable_shared_from_this<GameExecutor>
    {
    public:
        using task_type = std::variant<std::function<void()>, std::move_only_function<void()>>;

        static constexpr int64_t max_concurrency = 36;

    public:
        GameExecutor() = delete;

        explicit GameExecutor(int64_t concurrency, int64_t spinCount = 2000);
        ~GameExecutor();

        void Run();
        void Stop() override;

        void Post(const std::function<void()>& function) override;
        void Post(std::move_only_function<void()> function) override;

        void Dispatch(const std::function<void()>& function) override;
        void Dispatch(std::move_only_function<void()> function) override;

        auto GetConcurrency() const->int64_t override;

        auto SharedFromThis() -> SharedPtrNotNull<IExecutor> override;
        auto SharedFromThis() const->SharedPtrNotNull<const IExecutor> override;

    private:
        void Post(task_type task);
        void Dispatch(task_type task);

        struct Worker
        {
            int64_t index = 0;
            std::jthread worker;
            std::thread::id threadId;

            std::vector<task_type> tasks;
            int32_t spinCount = 0;
        };

        void Run(Worker& worker);

    private:
#pragma warning(disable:4324)
        struct alignas(std::hardware_destructive_interference_size) WorkerContext
        {
            tbb::spin_mutex mutex;
            std::vector<task_type> tasks;

            bool running = false;
            bool shutdown = false;
            std::condition_variable_any conditionVariable;
        };
#pragma warning(default:4324)

        std::array<Worker, max_concurrency> _workers;
        std::array<WorkerContext, max_concurrency> _contexts;

        bool _running = false;

        int64_t _concurrency = 0;
        int64_t _maxSpinCount = 0;

        static thread_local int64_t post_index;
    };
}
