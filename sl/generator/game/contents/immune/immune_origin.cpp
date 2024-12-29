#include "immune_origin.h"

namespace sunlight
{
    ImmuneOrigin::ImmuneOrigin(ImmuneType immuneType, const ImmuneOriginSkill& param)
        : _immuneType(immuneType)
        , _variant(Skill{
            .skillId = param.skillId
        })
    {
    }

    auto ImmuneOrigin::GetImmuneType() const -> ImmuneType
    {
        return _immuneType;
    }

    bool ImmuneOrigin::operator==(const ImmuneOrigin& other) const
    {
        if (_immuneType != other._immuneType)
        {
            return false;
        }

        if (_variant.index() != other._variant.index())
        {
            return false;
        }

        return std::visit([&other]<typename T>(const T & item) -> bool
        {
            return item == std::get<T>(other._variant);

        }, _variant);
    }

    bool ImmuneOrigin::operator!=(const ImmuneOrigin& other) const
    {
        return !operator==(other);
    }

    bool ImmuneOrigin::Skill::operator==(const Skill& other) const
    {
        return skillId == other.skillId;
    }
}
