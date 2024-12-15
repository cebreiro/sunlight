#pragma once
#include "sl/generator/game/contents/job/job_id.h"
#include "sl/generator/game/system/game_system.h"

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
        PlayerJobSystem(const ServiceLocator& serviceLocator, int32_t stageId);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        bool Promote(GamePlayer& player, JobId advanced);
        bool AddSkill(GamePlayer& player, JobId jobId, int32_t skillId);

        void GainJobExp(GamePlayer& player, int32_t exp);

    public:
        void OnLocalActivate(GamePlayer& player);
        void OnSkillLevelSet(const ZoneMessage& message);
        void OnSkillQuickSlotPositionSet(const ZoneMessage& message);

    private:
        auto GetJobGainSkills(JobId id, int32_t level) const -> std::vector<int32_t>;

    private:
        static bool IsNovice(JobId job);
        static bool IsPromotable(JobId novice, JobId advanced);

    private:
        const ServiceLocator& _serviceLocator;
        int32_t _stageId = 0;
    };
}
