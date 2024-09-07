#include "force_failure.h"

namespace sunlight::bt::node
{
    auto ForceFailure::Decorate(State state) const -> State
    {
        (void)state;

        return State::Failure;
    }

    auto ForceFailure::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(ForceFailure&, const pugi::xml_node&)
    {
    }
}
