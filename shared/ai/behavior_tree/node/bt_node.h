#pragma once
#include <coroutine>
#include <variant>
#include <boost/container/small_vector.hpp>
#include "shared/type/not_null_pointer.h"
#include "shared/ai/behavior_tree/node/bt_node_state.h"
#include "shared/ai/behavior_tree/node/bt_node_result.h"

namespace pugi
{
    class xml_node;
}

namespace sunlight
{
    class BehaviorTree;
}

namespace sunlight::bt
{
    class BlackBoard;
}
namespace sunlight::bt
{
    class Branch;
    class Decorator;
    class Leaf;

    using NodePtr = std::variant<PtrNotNull<Branch>, PtrNotNull<Decorator>, PtrNotNull<Leaf>>;

    class INode
    {
    public:
        virtual ~INode() = default;

        virtual void Reset() = 0;
        virtual auto GetName() const -> std::string_view = 0;
        virtual auto GetNodePtr() -> NodePtr = 0;
    };

    class Branch : public INode
    {
    public:
        using children_container_type = boost::container::small_vector<NodePtr, 4>;

    public:
        virtual bool ShouldContinue(node::State childState) const = 0;

        virtual bool HasNext() const;
        virtual auto Next() -> NodePtr;

        void Reset() override;
        auto GetNodePtr() -> NodePtr final;

        bool HasChild() const;

        void AddChild(NodePtr child);

        auto GetChildren() -> const children_container_type&;
        auto GetChildren() const -> const children_container_type&;

    private:
        int64_t _index = 0;
        children_container_type _children;
    };

    class Decorator : public INode
    {
    public:
        bool HasChild() const;

        auto GetChild() -> NodePtr;
        void SetChild(NodePtr ptr);

        virtual auto Decorate(node::State state) const -> node::State = 0;

        void Reset() override;
        auto GetNodePtr() -> NodePtr final;

    private:
        NodePtr _node;
    };

    class Leaf : public INode
    {
    public:
        auto Execute() -> node::State;

        void Reset() override;
        auto GetNodePtr() -> NodePtr final;
        auto GetPromise() const -> std::coroutine_handle<node::Result::promise_type>;

        void SetBehaviorTree(BehaviorTree& bt);

    private:
        virtual auto Run() -> node::Result = 0;

    protected:
        auto GetBlackBoard() -> BlackBoard&;
        auto GetBlackBoard() const -> const BlackBoard&;

    private:
        node::State _state = node::State::Success;
        std::coroutine_handle<node::Result::promise_type> _handle;

    private:
        BehaviorTree* _bt = nullptr;
    };
}
