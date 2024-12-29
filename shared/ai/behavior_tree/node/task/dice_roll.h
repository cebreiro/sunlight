#pragma once
#include "shared/ai/behavior_tree/node/bt_node.h"

namespace sunlight::bt::node
{
    class DiceRoll final : public bt::Leaf
    {
    public:
        static constexpr const char* name = "dice_roll";

        auto Run() -> Result override;

        auto GetName() const -> std::string_view override;

        friend void from_xml(DiceRoll& self, const pugi::xml_node& xmlNode);

    private:
        int64_t _weight = 1;
        int64_t _total = 0;
    };
}
