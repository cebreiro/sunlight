#pragma once
#include "sl/generator/game/contents/immune/immune_type.h"

namespace sunlight
{
    struct ImmuneOriginSkill
    {
        int32_t skillId = 0;
    };

    class ImmuneOrigin
    {
    public:
        ImmuneOrigin(ImmuneType immuneType, const ImmuneOriginSkill& param);

        auto GetImmuneType() const -> ImmuneType;

        bool operator==(const ImmuneOrigin& other) const;
        bool operator!=(const ImmuneOrigin& other) const;

    private:
        ImmuneType _immuneType = {};

        struct Skill
        {
            int32_t skillId = 0;

            bool operator==(const Skill& other) const;
        };

        std::variant<Skill> _variant;
    };
}
