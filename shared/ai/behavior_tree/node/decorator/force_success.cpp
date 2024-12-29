#include "force_success.h"

namespace sunlight::bt::node
{
    auto ForceSuccess::Decorate(State state) const -> State
    {
        (void)state;

        return State::Success;
    }

    auto ForceSuccess::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(ForceSuccess&, const pugi::xml_node&)
    {
    }
}
