#pragma once
#include "shared/type/not_null_pointer.h"

namespace pugi
{
    class xml_node;
}

namespace sunlight::bt
{
    class INode;
}

namespace sunlight::bt
{
    class INodeSerializer
    {
    public:
        virtual ~INodeSerializer() = default;

        virtual auto Deserialize(const pugi::xml_node& xmlNode) const -> UniquePtrNotNull<INode> = 0;
    };
}
