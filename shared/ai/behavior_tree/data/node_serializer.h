#pragma once
#include <unordered_map>
#include <functional>
#include "shared/ai/behavior_tree/node/bt_node_concept.h"
#include "shared/ai/behavior_tree/data/node_serializer_interface.h"

namespace sunlight::bt
{
    class NodeSerializer : public INodeSerializer
    {
    public:
        NodeSerializer();

        auto Deserialize(const pugi::xml_node& xmlNode) const -> UniquePtrNotNull<INode> override;

        template <node_concept T>
        void RegisterXML();

    private:
        std::unordered_map<std::string, std::function<UniquePtrNotNull<INode>(const pugi::xml_node&)>> _xml;
    };

    template <node_concept T>
    void NodeSerializer::RegisterXML()
    {
        [[maybe_unused]]
        bool inserted = _xml.try_emplace(T::name, [](const pugi::xml_node& xml) -> UniquePtrNotNull<INode>
            {
                auto instance = std::make_unique<T>();
                from_xml(*instance, xml);

                return instance;
            }).second;
        assert(inserted);
    }
}
