#pragma once
#include "shared/ai/behavior_tree/node/bt_node.h"

namespace sunlight::bt::node
{
    class ForceFailure final : public Decorator
    {
    public:
        static constexpr const char* name = "force_failure";

    public:
        auto Decorate(State state) const -> State override;

        auto GetName() const -> std::string_view override;

        friend void from_xml(ForceFailure&, const pugi::xml_node&);
    };
}