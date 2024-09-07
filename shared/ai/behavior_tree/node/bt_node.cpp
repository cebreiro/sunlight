#include "bt_node.h"

#include "shared/ai/behavior_tree/behavior_tree.h"

namespace sunlight::bt
{
    bool Branch::HasNext() const
    {
        return _index < std::ssize(_children);
    }

    auto Branch::Next() -> NodePtr
    {
        NodePtr result = _children[_index];
        ++_index;

        return result;
    }

    void Branch::Reset()
    {
        _index = 0;
    }

    auto Branch::GetNodePtr() -> NodePtr
    {
        return this;
    }

    bool Branch::HasChild() const
    {
        return !_children.empty();
    }

    void Branch::AddChild(NodePtr child)
    {
        _children.push_back(child);
    }

    auto Branch::GetChildren() -> const children_container_type&
    {
        return _children;
    }

    auto Branch::GetChildren() const -> const children_container_type&
    {
        return _children;
    }

    bool Decorator::HasChild() const
    {
        return std::visit([]<typename T>(const T & item) -> bool
        {
            return item != nullptr;

        }, _node);
    }

    auto Decorator::GetChild() -> NodePtr
    {
        return _node;
    }

    void Decorator::SetChild(NodePtr ptr)
    {
        _node = ptr;
    }

    void Decorator::Reset()
    {
    }

    auto Decorator::GetNodePtr() -> NodePtr
    {
        return this;
    }

    auto Leaf::Execute() -> node::State
    {
        const node::Result status = this->Run();
        _state = status.GetState();

        if (_state == node::State::Running)
        {
            _handle = status.GetHandle();
        }

        return _state;
    }

    void Leaf::Reset()
    {
        if (_handle)
        {
            _handle.destroy();

            _handle = nullptr;
        }
    }

    auto Leaf::GetNodePtr() -> NodePtr
    {
        return this;
    }

    auto Leaf::GetPromise() const -> std::coroutine_handle<node::Result::promise_type>
    {
        return _handle;
    }

    void Leaf::SetBehaviorTree(BehaviorTree& bt)
    {
        _bt = &bt;
    }

    auto Leaf::GetBlackBoard() -> BlackBoard&
    {
        assert(_bt);

        return _bt->GetBlackBoard();
    }

    auto Leaf::GetBlackBoard() const -> const BlackBoard&
    {
        assert(_bt);

        return _bt->GetBlackBoard();
    }
}
