#pragma once
#include <cassert>
#include <boost/unordered/unordered_flat_set.hpp>
#include "shared/ai/behavior_tree/node/bt_node.h"
#include "shared/ai/behavior_tree/node/bt_node_state.h"
#include "shared/ai/behavior_tree/node/bt_node_event.h"

namespace sunlight::bt
{
    class BlackBoard;
}

namespace sunlight
{
    class IBehaviorTreeLogger;
}

namespace sunlight
{
    class BehaviorTree
    {
    public:
        BehaviorTree() = delete;
        explicit BehaviorTree(bt::BlackBoard& blackBoard);
        ~BehaviorTree();

        void Initialize(std::string name, std::vector<UniquePtrNotNull<bt::INode>> nodes);
        void Finalize();

        bool IsAwaiting() const;
        bool CanResume() const;

        bool StopRequested() const;

        void RunOnce();
        void Resume();

        void RequestStop();

        template <bt::bt_event_concept E>
        bool IsWaitFor() const;
        bool IsWaitFor(const std::type_info& typeInfo) const;

        void SetOrExecuteSignalHandler(const std::function<void()>& function);

        template <bt::bt_event_concept E>
        void Notify(const E& e);
        void Notify(const std::any& any);

        template <bt::bt_event_concept E>
        void NotifyAndResume(const E& e);
        void NotifyAndResume(const std::any& any);

        auto GetName() const -> const std::string&;
        auto GetBlackBoard() -> bt::BlackBoard&;
        auto GetBlackBoard() const -> const bt::BlackBoard&;
        auto GetCurrentVisitNodeName() const -> std::optional<std::string>;
        auto GetCurrentAwaitEventName() const ->std::optional<std::string>;;

        void SetLogger(IBehaviorTreeLogger* logger);

    private:
        void Traverse();
        void FinalizeTraverse();

        void Signal();

        auto GetCurrentNodeName() const -> std::string;

    private:
        bt::BlackBoard& _blackBoard;
        bool _finalized = false;

        std::string _name;
        std::vector<UniquePtrNotNull<bt::INode>> _storage;
        IBehaviorTreeLogger* _logger = nullptr;

        bt::NodePtr _rootNode;
        bt::node::State _currentState = bt::node::State::Success;

        std::vector<bt::NodePtr> _stack;
        boost::unordered::unordered_flat_set<bt::INode*> _visited;
        std::coroutine_handle<bt::node::Result::promise_type> _runningNodeCoroutine;

        bool _stopRequested = false;
        std::function<void()> _signalHandler;
    };

    template <bt::bt_event_concept E>
    bool BehaviorTree::IsWaitFor() const
    {
        return IsWaitFor(typeid(E));
    }

    template <bt::bt_event_concept E>
    void BehaviorTree::Notify(const E& e)
    {
        if (_currentState != bt::node::State::Running)
        {
            return;
        }

        assert(_runningNodeCoroutine);

        if (bt::node::Result::promise_type& promise = _runningNodeCoroutine.promise();
            promise.IsWaitingFor(typeid(E)))
        {
            promise.SetEvent(e);

            Signal();
        }
    }

    template <bt::bt_event_concept E>
    void BehaviorTree::NotifyAndResume(const E& e)
    {
        if (_currentState != bt::node::State::Running)
        {
            return;
        }

        assert(_runningNodeCoroutine);

        if (bt::node::Result::promise_type& promise = _runningNodeCoroutine.promise();
            promise.IsWaitingFor(typeid(E)))
        {
            promise.SetEvent(e);

            _runningNodeCoroutine.resume();

            Traverse();
        }
    }
}
