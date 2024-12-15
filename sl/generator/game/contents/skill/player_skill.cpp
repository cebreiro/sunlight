#include "player_skill.h"

#include "sl/generator/game/contents/passive/passive.h"
#include "sl/generator/service/gamedata/skill/player_skill_data.h"

namespace sunlight
{
    PlayerSkill::PlayerSkill(JobId id, PtrNotNull<const PlayerSkillData> data)
        : _jobId(id)
        , _data(data)
    {
        assert(_data);
    }

    PlayerSkill::~PlayerSkill()
    {
    }

    bool PlayerSkill::HasPassive() const
    {
        return _passive.operator bool();
    }

    auto PlayerSkill::GetId() const -> int32_t
    {
        return GetData().index;
    }

    auto PlayerSkill::GetJobId() const -> JobId
    {
        return _jobId;
    }

    auto PlayerSkill::GetBaseLevel() const -> int32_t
    {
        return _baseLevel;
    }

    auto PlayerSkill::GetAdditionalLevel() const -> int32_t
    {
        return _additionalLevel;
    }

    auto PlayerSkill::GetLevel() const -> int32_t
    {
        return _baseLevel + _additionalLevel;
    }

    auto PlayerSkill::GetCooldown() const -> int32_t
    {
        return _cooldown;
    }

    auto PlayerSkill::GetPage() const -> int8_t
    {
        return _page;
    }

    auto PlayerSkill::GetX() const -> int8_t
    {
        return _x;
    }

    auto PlayerSkill::GetY() const -> int8_t
    {
        return _y;
    }

    auto PlayerSkill::GetEXP() const -> int32_t
    {
        return _exp;
    }

    auto PlayerSkill::GetData() const -> const PlayerSkillData&
    {
        assert(_data);

        return *_data;
    }

    auto PlayerSkill::GetPassive() -> Passive&
    {
        assert(HasPassive());

        return *_passive;
    }

    auto PlayerSkill::GetPassive() const -> const Passive&
    {
        assert(HasPassive());

        return *_passive;
    }

    void PlayerSkill::SetBaseLevel(int32_t value)
    {
        _baseLevel = value;
    }

    void PlayerSkill::SetAdditionalLevel(int32_t value)
    {
        _additionalLevel = value;
    }

    void PlayerSkill::SetCooldown(int32_t value)
    {
        _cooldown = value;
    }

    void PlayerSkill::SetPage(int8_t value)
    {
        _page = value;
    }

    void PlayerSkill::SetX(int8_t value)
    {
        _x = value;
    }

    void PlayerSkill::SetY(int8_t value)
    {
        _y = value;
    }

    void PlayerSkill::SetEXP(int32_t value)
    {
        _exp = value;
    }

    void PlayerSkill::SetPassive(UniquePtrNotNull<Passive> passive)
    {
        _passive = std::move(passive);
    }
}
