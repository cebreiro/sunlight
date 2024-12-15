#include "skill_basic.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    SkillBasic::SkillBasic(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        name = reader.ReadString(reader.Read<uint16_t>());
        routineId = reader.Read<int32_t>();
        abilityType = reader.Read<int32_t>();
        job1 = reader.Read<int32_t>();
        job2 = reader.Read<int32_t>();
        job3 = reader.Read<int32_t>();
        degree = reader.Read<int32_t>();
        maxLevel = reader.Read<int32_t>();
        delayId = reader.Read<int32_t>();
        canSubjob = reader.Read<int32_t>();
        levelQualification = reader.Read<int32_t>();
        ability1Qualification = reader.Read<int32_t>();
        ability1Level = reader.Read<int32_t>();
        ability2Qualification = reader.Read<int32_t>();
        ability2Level = reader.Read<int32_t>();
        passive = reader.Read<int32_t>();
        spConsumption = reader.Read<int32_t>();
        spConsumptionDelay = reader.Read<int32_t>();
        furyOnly = reader.Read<int32_t>();
        requireItem = reader.Read<int32_t>();
        requireItemCount = reader.Read<int32_t>();
        motionSpeed = reader.Read<float>();
        needWeapon = reader.Read<int32_t>();
        delayType = reader.Read<int32_t>();
        delayTime = reader.Read<int32_t>();
        useType = reader.Read<int32_t>();
        applyTargetType = reader.Read<int32_t>();
        applyDamageType = reader.Read<int32_t>();
        damageLength = reader.Read<int32_t>();
        damageLength2 = reader.Read<int32_t>();
        damageMaxcount = reader.Read<int32_t>();
        cancelable = reader.Read<int32_t>();
        useRange = reader.Read<int32_t>();
        addWeaponRange = reader.Read<int32_t>();
        applyCasting = reader.Read<int32_t>();
        castingDelay = reader.Read<int32_t>();
        applyCharging = reader.Read<int32_t>();
        chargingDelay = reader.Read<int32_t>();
        damageMotionType = reader.Read<int32_t>();
        effect1 = reader.Read<int32_t>();
        effect1Type = reader.Read<int32_t>();
        effect1Reserved1 = reader.Read<int32_t>();
        effect1Reserved2 = reader.Read<int32_t>();
        effect1Reserved3 = reader.Read<int32_t>();
        effect1Reserved4 = reader.Read<int32_t>();
        effect1Reserved5 = reader.Read<int32_t>();
        effect1Reserved6 = reader.Read<int32_t>();
        effect1Reserved7 = reader.Read<int32_t>();
        effect1Reserved8 = reader.Read<int32_t>();
        effect1Reserved9 = reader.Read<int32_t>();
        effect1Reserved10 = reader.Read<int32_t>();
        effect1Reserved11 = reader.Read<int32_t>();
        effect1Reserved12 = reader.Read<int32_t>();
        effect1HopType = reader.Read<int32_t>();
        effect1HopA = reader.Read<int32_t>();
        effect1HopB = reader.Read<int32_t>();
        effect2 = reader.Read<int32_t>();
        effect2Type = reader.Read<int32_t>();
        effect2Reserved1 = reader.Read<int32_t>();
        effect2Reserved2 = reader.Read<int32_t>();
        effect2Reserved3 = reader.Read<int32_t>();
        effect2Reserved4 = reader.Read<int32_t>();
        effect2Reserved5 = reader.Read<int32_t>();
        effect2Reserved6 = reader.Read<int32_t>();
        effect2Reserved7 = reader.Read<int32_t>();
        effect2Reserved8 = reader.Read<int32_t>();
        effect2Reserved9 = reader.Read<int32_t>();
        effect2Reserved10 = reader.Read<int32_t>();
        effect2Reserved11 = reader.Read<int32_t>();
        effect2Reserved12 = reader.Read<int32_t>();
        effect2HopType = reader.Read<int32_t>();
        effect2HopA = reader.Read<int32_t>();
        effect2HopB = reader.Read<int32_t>();
        effect3 = reader.Read<int32_t>();
        effect3Type = reader.Read<int32_t>();
        effect3Reserved1 = reader.Read<int32_t>();
        effect3Reserved2 = reader.Read<int32_t>();
        effect3Reserved3 = reader.Read<int32_t>();
        effect3Reserved4 = reader.Read<int32_t>();
        effect3Reserved5 = reader.Read<int32_t>();
        effect3Reserved6 = reader.Read<int32_t>();
        effect3Reserved7 = reader.Read<int32_t>();
        effect3Reserved8 = reader.Read<int32_t>();
        effect3Reserved9 = reader.Read<int32_t>();
        effect3Reserved10 = reader.Read<int32_t>();
        effect3Reserved11 = reader.Read<int32_t>();
        effect3Reserved12 = reader.Read<int32_t>();
        effect3HopType = reader.Read<int32_t>();
        effect3HopA = reader.Read<int32_t>();
        effect3HopB = reader.Read<int32_t>();
        effect4 = reader.Read<int32_t>();
        effect4Type = reader.Read<int32_t>();
        effect4Reserved1 = reader.Read<int32_t>();
        effect4Reserved2 = reader.Read<int32_t>();
        effect4Reserved3 = reader.Read<int32_t>();
        effect4Reserved4 = reader.Read<int32_t>();
        effect4Reserved5 = reader.Read<int32_t>();
        effect4Reserved6 = reader.Read<int32_t>();
        effect4Reserved7 = reader.Read<int32_t>();
        effect4Reserved8 = reader.Read<int32_t>();
        effect4Reserved9 = reader.Read<int32_t>();
        effect4Reserved10 = reader.Read<int32_t>();
        effect4Reserved11 = reader.Read<int32_t>();
        effect4Reserved12 = reader.Read<int32_t>();
        effect4HopType = reader.Read<int32_t>();
        effect4HopA = reader.Read<int32_t>();
        effect4HopB = reader.Read<int32_t>();
    }

    void SkillBasicTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "skill_basic.sox");
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

    auto SkillBasicTable::Find(int32_t index) const -> const SkillBasic*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto SkillBasicTable::Get() const -> const std::vector<SkillBasic>&
    {
        return _vector;
    }
}
