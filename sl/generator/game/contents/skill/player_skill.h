#pragma once
#include "sl/generator/game/contents/job/job_id.h"

namespace sunlight
{
    struct PlayerSkillData;

    class Passive;
}

namespace sunlight
{
    class PlayerSkill
    {
    public:
        PlayerSkill() = default;
        PlayerSkill(PlayerSkill&& other) noexcept = default;
        PlayerSkill& operator=(PlayerSkill&& other) noexcept = default;

        PlayerSkill(const PlayerSkill& other) = delete;
        PlayerSkill& operator=(const PlayerSkill& other) = delete;

    public:
        PlayerSkill(JobId id ,PtrNotNull<const PlayerSkillData> data);
        ~PlayerSkill();

        bool HasPassive() const;

        auto GetId() const -> int32_t;
        auto GetJobId() const -> JobId;
        auto GetBaseLevel() const -> int32_t;
        auto GetAdditionalLevel() const -> int32_t;
        auto GetLevel() const -> int32_t;
        auto GetCooldown() const -> int32_t;
        auto GetPage() const -> int8_t;
        auto GetX() const -> int8_t;
        auto GetY() const -> int8_t;
        auto GetEXP() const -> int32_t;
        auto GetData() const -> const PlayerSkillData&;
        auto GetPassive() -> Passive&;
        auto GetPassive() const -> const Passive&;

        void SetBaseLevel(int32_t value);
        void SetAdditionalLevel(int32_t value);
        void SetCooldown(int32_t value);
        void SetPage(int8_t value);
        void SetX(int8_t value);
        void SetY(int8_t value);
        void SetEXP(int32_t value);
        void SetPassive(UniquePtrNotNull<Passive> passive);

    private:
        JobId _jobId = JobId::None;
        int32_t _baseLevel = 0;
        int32_t _additionalLevel = 0;
        int32_t _cooldown = 0;
        int8_t _page = -1;
        int8_t _x = -1;
        int8_t _y = -1;
        int32_t _exp = 0;

        PtrNotNull<const PlayerSkillData> _data = nullptr;
        UniquePtrNotNull<Passive> _passive;
    };
}
