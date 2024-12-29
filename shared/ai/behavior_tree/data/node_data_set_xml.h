#pragma once
#include <pugixml.hpp>
#include "shared/ai/behavior_tree/data/node_data_set_interface.h"

namespace sunlight::bt
{
    class NodeDataSetXML : public INodeDataSet
    {
    public:
        explicit NodeDataSetXML(const pugi::xml_node& xmlNode);

        auto Deserialize(const NodeSerializer& serializer) const -> std::vector<UniquePtrNotNull<INode>> override;

    private:
        auto Deserialize(const pugi::xml_node& xmlNode, const NodeSerializer& serializer, std::vector<UniquePtrNotNull<INode>>& storage) const -> INode*;

    private:
        pugi::xml_node _xmlNode;
    };
}
