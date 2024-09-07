#include "inverter.h"

namespace sunlight::bt::node
{
    auto Inverter::Decorate(State state) const -> State
    {
        assert(state != State::Running);

        return state == State::Success ? State::Failure : State::Success;
    }

    auto Inverter::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(Inverter&, const pugi::xml_node&)
    {
    }
}
