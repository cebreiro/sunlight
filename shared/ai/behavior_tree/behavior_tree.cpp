#include "behavior_tree.h"

#include "shared/ai/behavior_tree/data/node_data_set_interface.h"
#include "shared/ai/behavior_tree/log/behavior_tree_logger_interface.h"
#include "shared/ai/behavior_tree/node/bt_node.h"
#include "shared/ai/behavior_tree/node/bt_node_result.h"

namespace sunlight
{
    BehaviorTree::BehaviorTree(bt::BlackBoard& blackBoard)
        : _blackBoard(blackBoard)
    {
    }

    BehaviorTree::~BehaviorTree()
    {
        Finalize();
    }

    void BehaviorTree::Initialize(std::string name, std::vector<UniquePtrNotNull<bt::INode>> nodes)
    {
        assert(!nodes.empty());

        _name = std::move(name);
        _storage = std::move(nodes);

        _rootNode = _storage.front()->GetNodePtr();

        for (const UniquePtrNotNull<bt::INode>& node : _storage)
        {
            bt::NodePtr nodePtr = node->GetNodePtr();

            if (auto pptr = std::get_if<PtrNotNull<bt::Leaf>>(&nodePtr); pptr)
            {
                bt::Leaf& leaf = **pptr;

                leaf.SetBehaviorTree(*this);
            }
        }
    }

    bool BehaviorTree::IsAwaiting() const
    {
        return !_stack.empty();
    }

    bool BehaviorTree::CanResume() const
    {
        if (!IsAwaiting())
        {
            return false;
        }

        if (!_runningNodeCoroutine)
        {
            return false;
        }

        return _runningNodeCoroutine.promise().HasEvent();
    }

    bool BehaviorTree::StopRequested() const
    {
        return _stopRequested;
    }

    void BehaviorTree::RunOnce()
    {
        assert(!IsAwaiting());

        if (_logger)
        {
            _logger->LogBehaviorTreeStart(GetName());
        }

        _stack.push_back(_rootNode);

        Traverse();
    }

    void BehaviorTree::Resume()
    {
        assert(CanResume());
        assert(IsAwaiting());
        assert(_runningNodeCoroutine);

        _runningNodeCoroutine.resume();

        Traverse();
    }

    void BehaviorTree::RequestStop()
    {
        _stopRequested = true;

        Signal();
    }

    bool BehaviorTree::IsWaitFor(const std::type_info& typeInfo) const
    {
        if (_currentState != bt::node::State::Running)
        {
            return false;
        }

        assert(_runningNodeCoroutine);
        bt::node::Result::promise_type& promise = _runningNodeCoroutine.promise();

        return promise.IsWaitingFor(typeInfo);
    }

    void BehaviorTree::SetOrExecuteSignalHandler(const std::function<void()>& function)
    {
        assert(!_signalHandler);

        if (CanResume() || _stopRequested)
        {
            function();
        }
        else
        {
            _signalHandler = function;
        }
    }

    void BehaviorTree::Notify(const std::any& any)
    {
        if (_currentState != bt::node::State::Running)
        {
            return;
        }

        assert(_runningNodeCoroutine);

        if (bt::node::Result::promise_type& promise = _runningNodeCoroutine.promise();
            promise.IsWaitingFor(any.type()))
        {
            if (_logger)
            {
                _logger->LogNodeResumeEvent(GetName(), GetCurrentNodeName(), any.type().name());
            }

            promise.SetEvent(any);

            Signal();
        }
    }

    void BehaviorTree::Finalize()
    {
        if (std::exchange(_finalized, true))
        {
            return;
        }

        FinalizeTraverse();

        _stack.clear();

        if (_runningNodeCoroutine)
        {
            _runningNodeCoroutine = nullptr;
        }

        if (_logger)
        {
            _logger->LogBehaviorTreeFinalize(GetName());
        }
    }

    void BehaviorTree::NotifyAndResume(const std::any& any)
    {
        if (_currentState != bt::node::State::Running)
        {
            return;
        }

        assert(_runningNodeCoroutine);

        if (bt::node::Result::promise_type& promise = _runningNodeCoroutine.promise();
            promise.IsWaitingFor(any.type()))
        {
            if (_logger)
            {
                _logger->LogNodeResumeEvent(GetName(), GetCurrentNodeName(), any.type().name());
            }

            promise.SetEvent(any);

            _runningNodeCoroutine.resume();

            Traverse();
        }
    }

    auto BehaviorTree::GetName() const -> const std::string&
    {
        return _name;
    }

    auto BehaviorTree::GetBlackBoard() -> bt::BlackBoard&
    {
        return _blackBoard;
    }

    auto BehaviorTree::GetBlackBoard() const -> const bt::BlackBoard&
    {
        return _blackBoard;
    }

    auto BehaviorTree::GetCurrentVisitNodeName() const -> std::optional<std::string>
    {
        if (_stack.empty())
        {
            return std::nullopt;
        }

        return GetCurrentNodeName();
    }

    auto BehaviorTree::GetCurrentAwaitEventName() const -> std::optional<std::string>
    {
        if (_currentState != bt::node::State::Running)
        {
            return std::nullopt;
        }

        assert(_runningNodeCoroutine);

        return _runningNodeCoroutine.promise().GetAwaitEventNames();
    }

    void BehaviorTree::SetLogger(IBehaviorTreeLogger* logger)
    {
        _logger = logger;
    }

    void BehaviorTree::Traverse()
    {
        assert(!_stack.empty());

        while (!_stack.empty())
        {
            bt::NodePtr current = _stack.back();

            if (auto pptr = std::get_if<PtrNotNull<bt::Branch>>(&current); pptr)
            {
                bt::Branch& branch = **pptr;

                if (_visited.contains(&branch))
                {
                    assert(_currentState != bt::node::State::Running);

                    if (branch.ShouldContinue(_currentState))
                    {
                        if (branch.HasNext())
                        {
                            _stack.emplace_back(branch.Next());

                            continue;
                        }
                    }
                }
                else
                {
                    assert(branch.HasNext());

                    (void)_visited.insert(&branch);
                    if (_logger)
                    {
                        _logger->LogNodeStart(GetName(), GetCurrentNodeName());
                    }

                    _stack.emplace_back(branch.Next());

                    continue;
                }
            }

            if (auto pptr = std::get_if<PtrNotNull<bt::Decorator>>(&current); pptr)
            {
                bt::Decorator& decorator = **pptr;

                if (_visited.contains(&decorator))
                {
                    assert(_currentState != bt::node::State::Running);

                    _currentState = decorator.Decorate(_currentState);
                }
                else
                {
                    assert(decorator.HasChild());

                    (void)_visited.insert(&decorator);
                    if (_logger)
                    {
                        _logger->LogNodeStart(GetName(), GetCurrentNodeName());
                    }

                    _stack.push_back(decorator.GetChild());

                    continue;
                }
            }

            if (auto pptr = std::get_if<PtrNotNull<bt::Leaf>>(&current); pptr)
            {
                bt::Leaf& leaf = **pptr;

                if (_visited.contains(&leaf))
                {
                    assert(_currentState == bt::node::State::Running);
                    assert(_runningNodeCoroutine && _runningNodeCoroutine == leaf.GetPromise());

                    _currentState = _runningNodeCoroutine.promise().GetState();

                    if (_currentState == bt::node::State::Running)
                    {
                        break;
                    }

                    _runningNodeCoroutine = nullptr;
                }
                else
                {
                    _visited.insert(&leaf);
                    if (_logger)
                    {
                        _logger->LogNodeStart(GetName(), GetCurrentNodeName());
                    }

                    _currentState = leaf.Execute();

                    if (_currentState == bt::node::State::Running)
                    {
                        _runningNodeCoroutine = leaf.GetPromise();

                        if (_logger)
                        {
                            _logger->LogNodeSuspend(GetName(), GetCurrentNodeName());
                        }

                        break;
                    }
                }
            }

            if (_logger)
            {
                assert(_currentState != bt::node::State::Running);

                _logger->LogNodeEnd(GetName(), GetCurrentNodeName(), _currentState == bt::node::State::Success);
            }

            _stack.pop_back();
        }

        if (_stack.empty())
        {
            assert(_currentState != bt::node::State::Running);

            if (_logger)
            {
                _logger->LogBehaviorTreeEnd(GetName(), _currentState == bt::node::State::Success);
            }
            
            FinalizeTraverse();
        }
    }

    void BehaviorTree::FinalizeTraverse()
    {
        for (bt::INode* node : _visited)
        {
            node->Reset();
        }

        _visited.clear();
        _currentState = bt::node::State::Success;
    }

    void BehaviorTree::Signal()
    {
        if (const auto signalHandler = std::exchange(_signalHandler, {}))
        {
            signalHandler();
        }
    }

    auto BehaviorTree::GetCurrentNodeName() const -> std::string
    {
        assert(!_stack.empty());

        return std::visit([]<typename T>(const T & item) -> std::string
        {
            return std::string(item->GetName());

        }, _stack.back());
    }
}
