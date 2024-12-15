#include "skill_action.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    SkillAction::SkillAction(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        type = reader.Read<int32_t>();
        reserved1 = reader.Read<int32_t>();
        reserved2 = reader.Read<int32_t>();
        reserved3 = reader.Read<int32_t>();
        charge1FxGid = reader.Read<int32_t>();
        charge1FxPid = reader.Read<int32_t>();
        charge2FxGid = reader.Read<int32_t>();
        charge2FxPid = reader.Read<int32_t>();
        shockFxGid = reader.Read<int32_t>();
        shockFxPid = reader.Read<int32_t>();
        _1handswordCharge1Pid = reader.Read<int32_t>();
        _2handswordCharge1Pid = reader.Read<int32_t>();
        heavyswordCharge1Pid = reader.Read<int32_t>();
        hammerCharge1Pid = reader.Read<int32_t>();
        axeCharge1Pid = reader.Read<int32_t>();
        spearCharge1Pid = reader.Read<int32_t>();
        polearmCharge1Pid = reader.Read<int32_t>();
        knuckleCharge1Pid = reader.Read<int32_t>();
        dartCharge1Pid = reader.Read<int32_t>();
        gunCharge1Pid = reader.Read<int32_t>();
        doublegunCharge1Pid = reader.Read<int32_t>();
        shotgunCharge1Pid = reader.Read<int32_t>();
        machinegunCharge1Pid = reader.Read<int32_t>();
        bowCharge1Pid = reader.Read<int32_t>();
        crossbowCharge1Pid = reader.Read<int32_t>();
        daggerCharge1Pid = reader.Read<int32_t>();
        rodCharge1Pid = reader.Read<int32_t>();
        _1handswordCharge2Pid = reader.Read<int32_t>();
        _2handswordCharge2Pid = reader.Read<int32_t>();
        heavyswordCharge2Pid = reader.Read<int32_t>();
        hammerCharge2Pid = reader.Read<int32_t>();
        axeCharge2Pid = reader.Read<int32_t>();
        spearCharge2Pid = reader.Read<int32_t>();
        polearmCharge2Pid = reader.Read<int32_t>();
        knuckleCharge2Pid = reader.Read<int32_t>();
        dartCharge2Pid = reader.Read<int32_t>();
        gunCharge2Pid = reader.Read<int32_t>();
        doublegunCharge2Pid = reader.Read<int32_t>();
        shotgunCharge2Pid = reader.Read<int32_t>();
        machinegunCharge2Pid = reader.Read<int32_t>();
        bowCharge2Pid = reader.Read<int32_t>();
        crossbowCharge2Pid = reader.Read<int32_t>();
        daggerCharge2Pid = reader.Read<int32_t>();
        rodCharge2Pid = reader.Read<int32_t>();
        _1handswordPid = reader.Read<int32_t>();
        _2handswordPid = reader.Read<int32_t>();
        heavyswordPid = reader.Read<int32_t>();
        hammerPid = reader.Read<int32_t>();
        axePid = reader.Read<int32_t>();
        spearPid = reader.Read<int32_t>();
        polearmPid = reader.Read<int32_t>();
        knucklePid = reader.Read<int32_t>();
        dartPid = reader.Read<int32_t>();
        gunPid = reader.Read<int32_t>();
        doublegunPid = reader.Read<int32_t>();
        shotgunPid = reader.Read<int32_t>();
        machinegunPid = reader.Read<int32_t>();
        bowPid = reader.Read<int32_t>();
        crossbowPid = reader.Read<int32_t>();
        daggerPid = reader.Read<int32_t>();
        rodPid = reader.Read<int32_t>();
        _1handswordAttackedTiming = reader.Read<int32_t>();
        _2handswordAttackedTiming = reader.Read<int32_t>();
        heavyswordAttackedTiming = reader.Read<int32_t>();
        hammerAttackedTiming = reader.Read<int32_t>();
        axeAttackedTiming = reader.Read<int32_t>();
        spearAttackedTiming = reader.Read<int32_t>();
        polearmAttackedTiming = reader.Read<int32_t>();
        knuckleAttackedTiming = reader.Read<int32_t>();
        dartAttackedTiming = reader.Read<int32_t>();
        gunAttackedTiming = reader.Read<int32_t>();
        doublegunAttackedTiming = reader.Read<int32_t>();
        shotgunAttackedTiming = reader.Read<int32_t>();
        machinegunAttackedTiming = reader.Read<int32_t>();
        bowAttackedTiming = reader.Read<int32_t>();
        crossbowAttackedTiming = reader.Read<int32_t>();
        daggerAttackedTiming = reader.Read<int32_t>();
        rodAttackedTiming = reader.Read<int32_t>();
        _1handswordTiming = reader.Read<int32_t>();
        _2handswordTiming = reader.Read<int32_t>();
        heavyswordTiming = reader.Read<int32_t>();
        hammerTiming = reader.Read<int32_t>();
        axeTiming = reader.Read<int32_t>();
        spearTiming = reader.Read<int32_t>();
        polearmTiming = reader.Read<int32_t>();
        knuckleTiming = reader.Read<int32_t>();
        dartTiming = reader.Read<int32_t>();
        gunTiming = reader.Read<int32_t>();
        doublegunTiming = reader.Read<int32_t>();
        shotgunTiming = reader.Read<int32_t>();
        machinegunTiming = reader.Read<int32_t>();
        bowTiming = reader.Read<int32_t>();
        crossbowTiming = reader.Read<int32_t>();
        daggerTiming = reader.Read<int32_t>();
        rodTiming = reader.Read<int32_t>();
        _1handswordFulltime = reader.Read<int32_t>();
        _2handswordFulltime = reader.Read<int32_t>();
        heavyswordFulltime = reader.Read<int32_t>();
        hammerFulltime = reader.Read<int32_t>();
        axeFulltime = reader.Read<int32_t>();
        spearFulltime = reader.Read<int32_t>();
        polearmFulltime = reader.Read<int32_t>();
        knuckleFulltime = reader.Read<int32_t>();
        dartFulltime = reader.Read<int32_t>();
        gunFulltime = reader.Read<int32_t>();
        doublegunFulltime = reader.Read<int32_t>();
        shotgunFulltime = reader.Read<int32_t>();
        machinegunFulltime = reader.Read<int32_t>();
        bowFulltime = reader.Read<int32_t>();
        crossbowFulltime = reader.Read<int32_t>();
        daggerFulltime = reader.Read<int32_t>();
        rodFulltime = reader.Read<int32_t>();
        xxx = reader.ReadString(reader.Read<uint16_t>());
    }

    void SkillActionTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "SKILL_ACTION.sox");
        StreamReader<std::vector<char>::const_iterator> reader(fileData.gameData.begin(), fileData.gameData.end());

        _vector.reserve(fileData.rowCount);
        for (int64_t i = 0; i < fileData.rowCount; ++i)
        {
            _vector.emplace_back(reader);
        }

        for (auto& data : _vector)
        {
            _umap[data.index] = &data;
        }
    }

    auto SkillActionTable::Find(int32_t index) const -> const SkillAction*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto SkillActionTable::Get() const -> const std::vector<SkillAction>&
    {
        return _vector;
    }
}
