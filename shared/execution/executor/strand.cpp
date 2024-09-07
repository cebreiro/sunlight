#include "strand.h"

#include <cassert>
#include <algorithm>
#include <ranges>

#include "shared/execution/executor/operation/post.h"

namespace sunlight
{
    Strand::Strand(SharedPtrNotNull<IExecutor> executor)
        : _executor(std::move(executor))
    {
    }

    bool Strand::try_lock(const std::thread::id& tid)
    {
        std::lock_guard lock(_spinMutex);

        if (_owner.empty())
        {
            _owner.push_back(tid);

            return true;
        }

        return false;
    }

    void Strand::release(const std::thread::id& tid)
    {
        (void)tid;

        int32_t taskCount = 0;
        {
            std::lock_guard lock(_spinMutex);

            assert(!_owner.empty() && _owner.back() == tid);

            _owner.pop_back();
            taskCount = _taskCount;
        }

        if (taskCount > 0)
        {
            PostFlushTask();
        }
    }

    void Strand::Stop()
    {
    }

    void Strand::Post(const std::function<void()>& function)
    {
        Post(task_type(function));
    }

    void Strand::Post(std::move_only_function<void()> function)
    {
        Post(task_type(std::move(function)));
    }

    void Strand::Dispatch(const std::function<void()>& function)
    {
        Dispatch(task_type(function));
    }

    void Strand::Dispatch(std::move_only_function<void()> function)
    {
        Dispatch(task_type(std::move(function)));
    }

    auto Strand::GetConcurrency() const -> int64_t
    {
        return 1;
    }

    auto Strand::GetTaskCount() const -> int64_t
    {
        std::unique_lock lock(_spinMutex);

        return _taskCount;
    }

    auto Strand::SharedFromThis() -> SharedPtrNotNull<IExecutor>
    {
        return shared_from_this();
    }

    auto Strand::SharedFromThis() const -> SharedPtrNotNull<const IExecutor>
    {
        return shared_from_this();
    }

    void Strand::Post(task_type task)
    {
        bool shouldPostTask = false;
        {
            std::lock_guard lock(_spinMutex);

            ++_taskCount;
            shouldPostTask = std::exchange(_posted, true) == false;
        }

        _tasks.emplace(std::move(task));

        if (shouldPostTask)
        {
            PostFlushTask();
        }
    }

    void Strand::Dispatch(task_type task)
    {
        const auto threadId = std::this_thread::get_id();

        bool shouldInvokeImmediately = false;
        {
            std::lock_guard lock(_spinMutex);

            if (ExecutionContext::GetExecutor() == this)
            {
                if (!_owner.empty() && _owner.back() != threadId)
                {
                    shouldInvokeImmediately = false;
                }
                else
                {
                    shouldInvokeImmediately = true;

                    _owner.push_back(threadId);
                }
            }
        }

        if (shouldInvokeImmediately)
        {
            {
                ExecutionContext::ExecutorGuard guard(this);

                std::visit([]<typename T>(T && va)
                {
                    va();
                }, std::move(task));
            }

            bool shouldPost = false;
            {
                std::lock_guard lock(_spinMutex);

                assert(!_owner.empty());
                _owner.pop_back();

                shouldPost = _taskCount > 0;
            }

            if (shouldPost)
            {
                PostFlushTask();
            }
        }
        else
        {
            Post(std::move(task));
        }
    }

    void Strand::PostFlushTask()
    {
        execution::Post(*_executor, [self = SharedFromThis(), this]()
            {
                this->FlushTasks();
            });
    }

    void Strand::FlushTasks()
    {
        const auto threadId = std::this_thread::get_id();
        int32_t taskCount = 0;
        {
            std::lock_guard lock(_spinMutex);

            if (!_owner.empty() && _owner.back() != threadId)
            {
                return;
            }

            _owner.push_back(threadId);
            taskCount = _taskCount;
        }

        {
            ExecutionContext::ExecutorGuard guard(this);

            int32_t count = 0;

            while (count < taskCount)
            {
                task_type task;
                if (_tasks.try_pop(task))
                {
                    ++count;

                    std::visit([]<typename T>(T && va)
                    {
                        va();
                    }, std::move(task));
                }
            }
        }

        bool shouldContinue = false;
        {
            std::lock_guard lock(_spinMutex);

            assert(!_owner.empty());
            _owner.pop_back();

            _taskCount -= taskCount;
            shouldContinue = _taskCount > 0;

            if (!shouldContinue)
            {
                _posted = false;
            }
        }

        if (shouldContinue)
        {
            PostFlushTask();
        }
    }
}
