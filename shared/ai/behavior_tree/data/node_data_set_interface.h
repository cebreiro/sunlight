#pragma once
#include <vector>
#include "shared/type/not_null_pointer.h"

namespace sunlight::bt
{
    class INode;
    class NodeSerializer;
}

namespace sunlight::bt
{
    class INodeDataSet
    {
    public:
        virtual ~INodeDataSet() = default;

        virtual auto Deserialize(const NodeSerializer& serializer) const -> std::vector<UniquePtrNotNull<INode>> = 0;
    };
}
