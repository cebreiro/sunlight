#include "selector.h"

namespace sunlight::bt::node
{
    bool Selector::ShouldContinue(State childState) const
    {
        switch (childState)
        {
        case State::Success:
            return false;
        case State::Failure:
            return true;
        case State::Running:
            assert(false);
        }

        return false;
    }

    auto Selector::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(Selector&, const pugi::xml_node&)
    {
    }
}
