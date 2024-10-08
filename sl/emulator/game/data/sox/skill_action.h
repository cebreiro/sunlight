#pragma once
/**********************************************************
*        AUTO GENERATED FILE BY sox_code_generator        *
*                  DO NOT EDIT THIS FILE                  *
**********************************************************/

#include "sl/data/sox/sox_table_interface.h"

namespace sunlight::sox
{
    struct SkillAction
    {
        explicit SkillAction(StreamReader<std::vector<char>::const_iterator>& reader);

        int32_t index = {};
        int32_t type = {};
        int32_t reserved1 = {};
        int32_t reserved2 = {};
        int32_t reserved3 = {};
        int32_t charge1FxGid = {};
        int32_t charge1FxPid = {};
        int32_t charge2FxGid = {};
        int32_t charge2FxPid = {};
        int32_t shockFxGid = {};
        int32_t shockFxPid = {};
        int32_t _1handswordCharge1Pid = {};
        int32_t _2handswordCharge1Pid = {};
        int32_t heavyswordCharge1Pid = {};
        int32_t hammerCharge1Pid = {};
        int32_t axeCharge1Pid = {};
        int32_t spearCharge1Pid = {};
        int32_t polearmCharge1Pid = {};
        int32_t knuckleCharge1Pid = {};
        int32_t dartCharge1Pid = {};
        int32_t gunCharge1Pid = {};
        int32_t doublegunCharge1Pid = {};
        int32_t shotgunCharge1Pid = {};
        int32_t machinegunCharge1Pid = {};
        int32_t bowCharge1Pid = {};
        int32_t crossbowCharge1Pid = {};
        int32_t daggerCharge1Pid = {};
        int32_t rodCharge1Pid = {};
        int32_t _1handswordCharge2Pid = {};
        int32_t _2handswordCharge2Pid = {};
        int32_t heavyswordCharge2Pid = {};
        int32_t hammerCharge2Pid = {};
        int32_t axeCharge2Pid = {};
        int32_t spearCharge2Pid = {};
        int32_t polearmCharge2Pid = {};
        int32_t knuckleCharge2Pid = {};
        int32_t dartCharge2Pid = {};
        int32_t gunCharge2Pid = {};
        int32_t doublegunCharge2Pid = {};
        int32_t shotgunCharge2Pid = {};
        int32_t machinegunCharge2Pid = {};
        int32_t bowCharge2Pid = {};
        int32_t crossbowCharge2Pid = {};
        int32_t daggerCharge2Pid = {};
        int32_t rodCharge2Pid = {};
        int32_t _1handswordPid = {};
        int32_t _2handswordPid = {};
        int32_t heavyswordPid = {};
        int32_t hammerPid = {};
        int32_t axePid = {};
        int32_t spearPid = {};
        int32_t polearmPid = {};
        int32_t knucklePid = {};
        int32_t dartPid = {};
        int32_t gunPid = {};
        int32_t doublegunPid = {};
        int32_t shotgunPid = {};
        int32_t machinegunPid = {};
        int32_t bowPid = {};
        int32_t crossbowPid = {};
        int32_t daggerPid = {};
        int32_t rodPid = {};
        int32_t _1handswordAttackedTiming = {};
        int32_t _2handswordAttackedTiming = {};
        int32_t heavyswordAttackedTiming = {};
        int32_t hammerAttackedTiming = {};
        int32_t axeAttackedTiming = {};
        int32_t spearAttackedTiming = {};
        int32_t polearmAttackedTiming = {};
        int32_t knuckleAttackedTiming = {};
        int32_t dartAttackedTiming = {};
        int32_t gunAttackedTiming = {};
        int32_t doublegunAttackedTiming = {};
        int32_t shotgunAttackedTiming = {};
        int32_t machinegunAttackedTiming = {};
        int32_t bowAttackedTiming = {};
        int32_t crossbowAttackedTiming = {};
        int32_t daggerAttackedTiming = {};
        int32_t rodAttackedTiming = {};
        int32_t _1handswordTiming = {};
        int32_t _2handswordTiming = {};
        int32_t heavyswordTiming = {};
        int32_t hammerTiming = {};
        int32_t axeTiming = {};
        int32_t spearTiming = {};
        int32_t polearmTiming = {};
        int32_t knuckleTiming = {};
        int32_t dartTiming = {};
        int32_t gunTiming = {};
        int32_t doublegunTiming = {};
        int32_t shotgunTiming = {};
        int32_t machinegunTiming = {};
        int32_t bowTiming = {};
        int32_t crossbowTiming = {};
        int32_t daggerTiming = {};
        int32_t rodTiming = {};
        int32_t _1handswordFulltime = {};
        int32_t _2handswordFulltime = {};
        int32_t heavyswordFulltime = {};
        int32_t hammerFulltime = {};
        int32_t axeFulltime = {};
        int32_t spearFulltime = {};
        int32_t polearmFulltime = {};
        int32_t knuckleFulltime = {};
        int32_t dartFulltime = {};
        int32_t gunFulltime = {};
        int32_t doublegunFulltime = {};
        int32_t shotgunFulltime = {};
        int32_t machinegunFulltime = {};
        int32_t bowFulltime = {};
        int32_t crossbowFulltime = {};
        int32_t daggerFulltime = {};
        int32_t rodFulltime = {};
        std::string xxx = {};
    };

    class SkillActionTable final : public ISoxTable, public std::enable_shared_from_this<SkillActionTable>
    {
    public:
        void LoadFromFile(const std::filesystem::path& path) override;

        auto Find(int32_t index) const -> const SkillAction*;
        auto Get() const -> const std::vector<SkillAction>&;

    private:
        std::unordered_map<int32_t, SkillAction*> _umap;
        std::vector<SkillAction> _vector;

    };
}
