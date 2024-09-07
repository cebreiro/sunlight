#include "node_data_set_xml.h"

#include <variant>
#include "shared/ai/behavior_tree/data/node_serializer.h"
#include "shared/ai/behavior_tree/node/bt_node.h"

namespace sunlight::bt
{
    NodeDataSetXML::NodeDataSetXML(const pugi::xml_node& xmlNode)
        : _xmlNode(xmlNode)
    {
    }

    auto NodeDataSetXML::Deserialize(const NodeSerializer& serializer) const -> std::vector<UniquePtrNotNull<INode>>
    {
        std::vector<UniquePtrNotNull<INode>> storage;

        (void)Deserialize(_xmlNode, serializer, storage);

        return storage;
    }

    auto NodeDataSetXML::Deserialize(const pugi::xml_node& xmlNode, const NodeSerializer& serializer,
        std::vector<UniquePtrNotNull<INode>>& storage) const -> INode*
    {
        INode* node = storage.emplace_back(serializer.Deserialize(xmlNode)).get();
        assert(node);

        std::visit([&]<typename T>(T va)
            {
                if constexpr (std::is_same_v<T, PtrNotNull<Branch>>)
                {
                    for (const pugi::xml_node& childXML : xmlNode.children())
                    {
                        INode* child = Deserialize(childXML, serializer, storage);

                        va->AddChild(child->GetNodePtr());
                    }
                }
                else if constexpr (std::is_same_v<T, PtrNotNull<Decorator>>)
                {
                    for (const pugi::xml_node& childXML : xmlNode.children())
                    {
                        INode* child = Deserialize(childXML, serializer, storage);

                        va->SetChild(child->GetNodePtr());
                    }
                }
                else if constexpr (std::is_same_v<T, PtrNotNull<Leaf>>)
                {
                    (void)va;
                }
                else
                {
                    assert(false);
                }

            }, node->GetNodePtr());

        return node;
    }
}
