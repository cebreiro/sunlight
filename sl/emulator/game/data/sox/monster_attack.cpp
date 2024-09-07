#include "monster_attack.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    MonsterAttack::MonsterAttack(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        attackType = reader.Read<int32_t>();
        attackPercent = reader.Read<int32_t>();
        attackRange = reader.Read<int32_t>();
        attackDamageFactor = reader.Read<float>();
        attackDivDamage = reader.Read<int32_t>();
        attackDivDamageDelay = reader.Read<int32_t>();
        attackEffectRange = reader.Read<int32_t>();
        attackReserved1 = reader.Read<int32_t>();
        attackReserved2 = reader.Read<int32_t>();
        attackReserved3 = reader.Read<int32_t>();
        attackBulletID = reader.Read<int32_t>();
        attackConditionID = reader.Read<int32_t>();
        attackConditionKeepTime = reader.Read<int32_t>();
        attackConditionProbability = reader.Read<int32_t>();
        attackConditionReserved1 = reader.Read<int32_t>();
        attackConditionReserved2 = reader.Read<int32_t>();
        attackAniPartID = reader.Read<int32_t>();
        attackEndFrame = reader.Read<int32_t>();
        attackDelay = reader.Read<int32_t>();
        attackBeatFrame = reader.Read<int32_t>();
        attackFxGroupID = reader.Read<int32_t>();
        attackSoundFileID = reader.Read<int32_t>();
        skill1Type = reader.Read<int32_t>();
        skill1Percent = reader.Read<int32_t>();
        skill1Range = reader.Read<int32_t>();
        skill1CoolTime = reader.Read<int32_t>();
        skill1LV = reader.Read<int32_t>();
        skill1AniPartID = reader.Read<int32_t>();
        skill1EndFrame = reader.Read<int32_t>();
        skill1Delay = reader.Read<int32_t>();
        skill1BeatFrame = reader.Read<int32_t>();
        skill1AbilityID = reader.Read<int32_t>();
        skill2Type = reader.Read<int32_t>();
        skill2Percent = reader.Read<int32_t>();
        skill2Range = reader.Read<int32_t>();
        skill2CoolTime = reader.Read<int32_t>();
        skill2LV = reader.Read<int32_t>();
        skill2AniPartID = reader.Read<int32_t>();
        skill2EndFrame = reader.Read<int32_t>();
        skill2Delay = reader.Read<int32_t>();
        skill2BeatFrame = reader.Read<int32_t>();
        skill2AbilityID = reader.Read<int32_t>();
        skill3Type = reader.Read<int32_t>();
        skill3Percent = reader.Read<int32_t>();
        skill3Range = reader.Read<int32_t>();
        skill3CoolTime = reader.Read<int32_t>();
        skill3LV = reader.Read<int32_t>();
        skill3AniPartID = reader.Read<int32_t>();
        skill3EndFrame = reader.Read<int32_t>();
        skill3Delay = reader.Read<int32_t>();
        skill3BeatFrame = reader.Read<int32_t>();
        skill3AbilityID = reader.Read<int32_t>();
        passive1Percent = reader.Read<int32_t>();
        passive1LV = reader.Read<int32_t>();
        passive1AddExp = reader.Read<int32_t>();
        passive1ID = reader.Read<int32_t>();
        passive1ExtraMoney = reader.Read<int32_t>();
        passive2Percent = reader.Read<int32_t>();
        passive2LV = reader.Read<int32_t>();
        passive2AddExp = reader.Read<int32_t>();
        passive2ID = reader.Read<int32_t>();
        passive2ExtraMoney = reader.Read<int32_t>();
        passive3Percent = reader.Read<int32_t>();
        passive3LV = reader.Read<int32_t>();
        passive3AddExp = reader.Read<int32_t>();
        passive3ID = reader.Read<int32_t>();
        passive3ExtraMoney = reader.Read<int32_t>();
    }

    void MonsterAttackTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "MONSTER_ATTACK.sox");
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

    auto MonsterAttackTable::Find(int32_t index) const -> const MonsterAttack*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto MonsterAttackTable::Get() const -> const std::vector<MonsterAttack>&
    {
        return _vector;
    }
}
