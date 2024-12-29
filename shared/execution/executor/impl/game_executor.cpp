#include "game_executor.h"

#include "shared/execution/context/execution_context.h"

namespace sunlight::execution
{
    thread_local int64_t GameExecutor::post_index = 0;

    GameExecutor::GameExecutor(int64_t concurrency, int64_t spinCount)
        : _concurrency(std::max<int64_t>(1, concurrency))
        , _maxSpinCount(std::max<int64_t>(1, spinCount))
    {
    }

    GameExecutor::~GameExecutor()
    {
        Stop();
    }

    void GameExecutor::Run()
    {
        _running = true;

        for (int64_t i = 0; i < _concurrency; ++i)
        {
            Worker& worker = _workers[i];
            worker.index = i;

            WorkerContext& context = _contexts[i];

            context.running = true;
            worker.worker = std::jthread([&worker, this]()
                {
                    this->Run(worker);
                });
        }
    }

    void GameExecutor::Stop()
    {
        if (!_running)
        {
            return;
        }

        for (int64_t i = 0; i < _concurrency; ++i)
        {
            WorkerContext& context = _contexts[i];

            {
                std::lock_guard lock(context.mutex);

                context.shutdown = true;
            }

            context.conditionVariable.notify_one();

            _workers[i].worker.join();
        }

        _running = false;
    }

    void GameExecutor::Post(const std::function<void()>& function)
    {
        Post(task_type(function));
    }

    void GameExecutor::Post(std::move_only_function<void()> function)
    {
        Post(task_type(std::move(function)));
    }

    void GameExecutor::Dispatch(const std::function<void()>& function)
    {
        Dispatch(task_type(function));
    }

    void GameExecutor::Dispatch(std::move_only_function<void()> function)
    {
        Dispatch(task_type(std::move(function)));
    }

    auto GameExecutor::GetConcurrency() const -> int64_t
    {
        return _concurrency;
    }

    auto GameExecutor::SharedFromThis() -> SharedPtrNotNull<IExecutor>
    {
        return shared_from_this();
    }

    auto GameExecutor::SharedFromThis() const -> SharedPtrNotNull<const IExecutor>
    {
        return shared_from_this();
    }

    void GameExecutor::Post(task_type task)
    {
        const int64_t index = post_index++ % _concurrency;

        std::condition_variable_any* conditionVariable = nullptr;
        {
            WorkerContext& context = _contexts[index];

            std::unique_lock lock(context.mutex);

            context.tasks.emplace_back(std::move(task));

            if (!context.running)
            {
                context.running = true;

                conditionVariable = &context.conditionVariable;
            }
        }

        if (conditionVariable)
        {
            conditionVariable->notify_one();
        }
    }

    void GameExecutor::Dispatch(task_type task)
    {
        if (ExecutionContext::GetExecutor() == this)
        {
            std::visit([]<typename T>(T & item)
            {
                item();

                std::atomic_thread_fence(std::memory_order::release);

            }, task);

            return;
        }

        Post(std::move(task));
    }

    void GameExecutor::Run(Worker& worker)
    {
        worker.threadId = std::this_thread::get_id();

        while (true)
        {
            bool swapBuffer = false;
            {
                WorkerContext& sharedContext = _contexts[worker.index];

                std::unique_lock lock(sharedContext.mutex);

                if (sharedContext.tasks.empty())
                {
                    if (worker.spinCount > _maxSpinCount)
                    {
                        sharedContext.running = false;

                        sharedContext.conditionVariable.wait(lock, [&sharedContext]()
                        {
                            return sharedContext.shutdown || sharedContext.running;
                        });
                    }
                }

                if (sharedContext.shutdown)
                {
                    break;
                }

                if (!sharedContext.tasks.empty())
                {
                    swapBuffer = true;

                    sharedContext.tasks.swap(worker.tasks);
                }
            }

            if (!swapBuffer)
            {
                ++worker.spinCount;

                // work stealing
                for (int64_t i = 0; i < _concurrency - 1; ++i)
                {
                    WorkerContext& sharedContext = _contexts[(i + 1 + worker.index) % _concurrency];

                    if (sharedContext.mutex.try_lock())
                    {
                        if (!sharedContext.tasks.empty())
                        {
                            swapBuffer = true;

                            sharedContext.tasks.swap(worker.tasks);
                        }
                        else
                        {
                            ++worker.spinCount;
                        }

                        sharedContext.mutex.unlock();
                    }
                    else
                    {
                        constexpr int32_t thread_interference_weight = 100;

                        worker.spinCount += thread_interference_weight;
                    }

                    if (swapBuffer || worker.spinCount > _maxSpinCount)
                    {
                        break;
                    }
                }
            }

            if (swapBuffer)
            {
                worker.spinCount = 0;

                assert(!worker.tasks.empty());

                for (task_type& task : worker.tasks)
                {
                    std::visit([]<typename T>(T & item)
                    {
                        item();

                        std::atomic_thread_fence(std::memory_order::release);

                    }, task);
                }

                worker.tasks.clear();
            }
        }
    }
}
