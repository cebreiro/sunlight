#pragma once
#include <string>
#include "shared/ai/behavior_tree/node/bt_node.h"

namespace pugi
{
    class xml_node;
}

namespace sunlight::bt
{
    template <typename T>
    concept node_concept = requires (T t, const pugi::xml_node & xml)
    {
        requires std::derived_from<T, INode>;
        { T::name } -> std::convertible_to<std::string>;
        from_xml(t, xml);
    };
}
