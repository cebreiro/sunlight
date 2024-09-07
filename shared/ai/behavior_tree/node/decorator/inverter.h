#pragma once
#include "shared/ai/behavior_tree/node/bt_node.h"

namespace sunlight::bt::node
{
    class Inverter final : public Decorator
    {
    public:
        static constexpr const char* name = "inverter";

    public:
        auto Decorate(State state) const -> State override;

        auto GetName() const -> std::string_view override;

        friend void from_xml(Inverter&, const pugi::xml_node&);
    };
}
