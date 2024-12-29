#pragma once
#include "shared/ai/behavior_tree/node/bt_node.h"

namespace pugi
{
    class xml_node;
}

namespace sunlight::bt::node
{
    class Sequence final : public Branch
    {
    public:
        static constexpr const char* name = "sequence";

    public:
        bool ShouldContinue(node::State childState) const override;

        auto GetName() const -> std::string_view override;

        friend void from_xml(Sequence&, const pugi::xml_node&);
    };
}
