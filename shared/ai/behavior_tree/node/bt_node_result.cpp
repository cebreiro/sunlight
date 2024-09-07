#include "bt_node_result.h"

#include <cassert>
#include <sstream>

namespace sunlight::bt::node
{
    auto Result::promise_type::get_return_object() -> Result
    {
        return Result(std::coroutine_handle<promise_type>::from_promise(*this));
    }

    auto Result::promise_type::initial_suspend() -> std::suspend_never
    {
        return std::suspend_never{};
    }

    auto Result::promise_type::final_suspend() noexcept -> std::suspend_always
    {
        return std::suspend_always{};
    }

    void Result::promise_type::unhandled_exception()
    {
        assert(false);

        std::rethrow_exception(std::current_exception());
    }

    void Result::promise_type::return_value(bool success)
    {
        _state = success ? State::Success : State::Failure;
    }

    bool Result::promise_type::IsWaitingFor(const std::type_info& typeInfo) const
    {
        if (!_eventChecker)
        {
            return false;
        }

        return _eventChecker(typeInfo);
    }

    bool Result::promise_type::HasEvent() const
    {
        return _event.has_value();
    }

    void Result::promise_type::SetEvent(const std::any& any)
    {
        _event = any;
    }

    auto Result::promise_type::GetState() const -> State
    {
        return _state;
    }

    auto Result::promise_type::GetAwaitEventNames() const -> std::string
    {
        std::ostringstream oss;

        for (const std::type_info* typeInfo : _awaitEvents)
        {
            oss << typeInfo->name() << ' ';
        }

        std::string result = oss.str();
        if (!result.empty())
        {
            result.pop_back();
        }

        return result;
    }

    Result::Result(bool success)
        : _state(success ? State::Success : State::Failure)
    {
    }

    Result::Result(std::coroutine_handle<promise_type> handle)
        : _handle(handle)
    {
    }

    auto Result::GetState() const -> State
    {
        if (_handle)
        {
            return _handle.promise().GetState();
        }

        return _state;
    }

    auto Result::GetHandle() const -> const std::coroutine_handle<promise_type>&
    {
        assert(_handle);

        return _handle;
    }
}
