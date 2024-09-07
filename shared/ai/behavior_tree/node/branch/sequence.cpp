#include "sequence.h"

namespace sunlight::bt::node
{
    bool Sequence::ShouldContinue(State childState) const
    {
        switch (childState)
        {
        case State::Success:
            return true;
        case State::Failure:
            return false;
        case State::Running:
            assert(false);
        }

        return false;
    }

    auto Sequence::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(Sequence&, const pugi::xml_node&)
    {
    }
}
