#pragma once
#include <any>
#include <coroutine>
#include <variant>
#include <functional>
#include "shared/ai/behavior_tree/node/bt_node_state.h"
#include "shared/ai/behavior_tree/node/bt_node_event.h"
#include "shared/type/not_null_pointer.h"

namespace sunlight::bt::node
{
    class Result
    {
    public:
        class promise_type
        {
        public:
            auto get_return_object() -> Result;
            auto initial_suspend() -> std::suspend_never;
            auto final_suspend() noexcept -> std::suspend_always;
            void unhandled_exception();
            void return_value(bool success);

            template <bt_event_concept... E>
            auto await_transform(Event<E...>);

            bool IsWaitingFor(const std::type_info& typeInfo) const;
            bool HasEvent() const;

            template <bt_event_concept E>
            void SetEvent(const E& e);
            void SetEvent(const std::any& any);

            auto GetState() const -> State;
            auto GetAwaitEventNames() const -> std::string;

        private:
            State _state = State::Success;
            std::any _event;
            std::function<bool(const std::type_info&)> _eventChecker;
            std::vector<const std::type_info*> _awaitEvents;
        };

    public:
        Result() = delete;

        explicit(false) Result(bool success);
        explicit(true) Result(std::coroutine_handle<promise_type> handle);

        auto GetState() const -> State;

        auto GetHandle() const -> const std::coroutine_handle<promise_type>&;

    private:
        State _state = State::Success;
        std::coroutine_handle<promise_type> _handle = nullptr;
    };

    template <bt_event_concept ... E>
    auto Result::promise_type::await_transform(Event<E...> e)
    {
        _eventChecker = [](const std::type_info& type) -> bool
            {
                return ((type == typeid(E)) || ...);
            };

        _awaitEvents.clear();
        std::ranges::copy(std::initializer_list<const std::type_info*>{ &typeid(E)... }, std::back_inserter(_awaitEvents));

        if (e.postOperation)
        {
            e.postOperation();
        }

        class Awaitable
        {
        public:
            explicit Awaitable(PtrNotNull<promise_type> promise)
                : _promise(promise)
            {
            }

            bool await_ready() const
            {
                return false;
            }

            void await_suspend(std::coroutine_handle<>) {}

            auto await_resume() const -> std::variant<E...>
            {
                std::variant<E...> event;

                std::any* any = &_promise->_event;

                (void)((any->type() == typeid(E) ?
                            (event = std::move(*std::any_cast<E>(any)), true) : false
                ) || ...);

                any->reset();
                _promise->_eventChecker = {};

                return event;
            }

        private:
            PtrNotNull<promise_type> _promise;
        };

        _state = State::Running;

        return Awaitable(this);
    }

    template <bt_event_concept E>
    void Result::promise_type::SetEvent(const E& e)
    {
        _event = e;
    }
}
