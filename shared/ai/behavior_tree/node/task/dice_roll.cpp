#include "dice_roll.h"

#include <pugixml.hpp>
#include <random>

namespace sunlight::bt::node
{
    auto DiceRoll::Run() -> Result
    {
        thread_local std::mt19937 mt19937(std::random_device{}());

        const int64_t weight = std::max<int64_t>(1, _weight);
        std::uniform_int_distribution<int64_t> dist(1, _total);

        const int64_t rand = dist(mt19937);

        return Result(rand <= weight);
    }

    auto DiceRoll::GetName() const -> std::string_view
    {
        return name;
    }

    void from_xml(DiceRoll& self, const pugi::xml_node& xmlNode)
    {
        if (const auto attr = xmlNode.attribute("total"); attr)
        {
            self._total = attr.as_int();
            assert(self._total > 1);
        }
        else
        {
            assert(false);
        }

        if (const auto attr = xmlNode.attribute("weight"); attr)
        {
            self._weight = attr.as_int();
        }
    }
}
