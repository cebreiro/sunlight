#pragma once
#include "sl/emulator/game/contents/job/job_id.h"
#include "sl/emulator/game/system/game_system.h"

namespace sunlight
{
    struct ZoneMessage;

    class GamePlayer;
}

namespace sunlight
{
    class PlayerJobSystem final : public GameSystem
    {
    public:
        explicit PlayerJobSystem(const ServiceLocator& serviceLocator);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        bool Promote(GamePlayer& player, JobId advanced);
        bool AddSkill(GamePlayer& player, JobId jobId, int32_t skillId);

        void GainJobExp(GamePlayer& player, int32_t exp);

    public:
        void OnSkillLevelSet(const ZoneMessage& message);

    private:
        auto GetJobGainSkills(JobId id, int32_t level) const -> std::vector<int32_t>;

    private:
        static bool IsPromotable(JobId novice, JobId advanced);

    private:
        const ServiceLocator& _serviceLocator;
    };
}
