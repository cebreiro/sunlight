#include "monster_part2.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    MonsterPart2::MonsterPart2(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        attack1type = reader.Read<int32_t>();
        attack1percent = reader.Read<float>();
        attack1range = reader.Read<int32_t>();
        attack1dmgfactor = reader.Read<float>();
        attack1effectrange = reader.Read<int32_t>();
        attack1additional = reader.Read<int32_t>();
        attack1reserved1 = reader.Read<int32_t>();
        attack1reserved2 = reader.Read<int32_t>();
        attack1reserved3 = reader.Read<int32_t>();
        attack1fx = reader.Read<int32_t>();
        attack1abnormality = reader.Read<int32_t>();
        attack1abnormalityDuration = reader.Read<int32_t>();
        attack1abnormalityProbmod = reader.Read<int32_t>();
        attack1abnormalityReserved1 = reader.Read<int32_t>();
        attack1abnormalityReserved2 = reader.Read<int32_t>();
        attack1atkanipartid = reader.Read<int32_t>();
        attack1atkprm = reader.Read<int32_t>();
        attack1delay = reader.Read<int32_t>();
        attack1beatkani = reader.Read<int32_t>();
        attack1FxType = reader.Read<int32_t>();
        attack1FxId = reader.Read<int32_t>();
        attack1Sound = reader.Read<int32_t>();
        attack2type = reader.Read<int32_t>();
        attack2percent = reader.Read<float>();
        attack2range = reader.Read<int32_t>();
        attack2dmgfactor = reader.Read<float>();
        attack2effectrange = reader.Read<int32_t>();
        attack2additional = reader.Read<int32_t>();
        attack2reserved1 = reader.Read<int32_t>();
        attack2reserved2 = reader.Read<int32_t>();
        attack2reserved3 = reader.Read<int32_t>();
        attack2fx = reader.Read<int32_t>();
        attack2abnormality = reader.Read<int32_t>();
        attack2abnormalityDuration = reader.Read<int32_t>();
        attack2abnormalityProbmod = reader.Read<int32_t>();
        attack2abnormalityReserved1 = reader.Read<int32_t>();
        attack2abnormalityReserved2 = reader.Read<int32_t>();
        attack2atkanipartid = reader.Read<int32_t>();
        attack2atkprm = reader.Read<int32_t>();
        attack2delay = reader.Read<int32_t>();
        attack2beatkani = reader.Read<int32_t>();
        attack2FxType = reader.Read<int32_t>();
        attack2FxId = reader.Read<int32_t>();
        attack2Sound = reader.Read<int32_t>();
        attack3type = reader.Read<int32_t>();
        attack3percent = reader.Read<float>();
        attack3range = reader.Read<int32_t>();
        attack3dmgfactor = reader.Read<float>();
        attack3effectrange = reader.Read<int32_t>();
        attack3additional = reader.Read<int32_t>();
        attack3reserved1 = reader.Read<int32_t>();
        attack3reserved2 = reader.Read<int32_t>();
        attack3reserved3 = reader.Read<int32_t>();
        attack3fx = reader.Read<int32_t>();
        attack3abnormality = reader.Read<int32_t>();
        attack3abnormalityDuration = reader.Read<int32_t>();
        attack3abnormalityProbmod = reader.Read<int32_t>();
        attack3abnormalityReserved1 = reader.Read<int32_t>();
        attack3abnormalityReserved3 = reader.Read<int32_t>();
        attack3atkanipartid = reader.Read<int32_t>();
        attack3atkprm = reader.Read<int32_t>();
        attack3delay = reader.Read<int32_t>();
        attack3beatkani = reader.Read<int32_t>();
        attack3FxType = reader.Read<int32_t>();
        attack3FxId = reader.Read<int32_t>();
        attack3Sound = reader.Read<int32_t>();
        attack4type = reader.Read<int32_t>();
        attack4percent = reader.Read<float>();
        attack4range = reader.Read<int32_t>();
        attack4dmgfactor = reader.Read<float>();
        attack4effectrange = reader.Read<int32_t>();
        attack4additional = reader.Read<int32_t>();
        attack4reserved1 = reader.Read<int32_t>();
        attack4reserved2 = reader.Read<int32_t>();
        attack4reserved3 = reader.Read<int32_t>();
        attack4fx = reader.Read<int32_t>();
        attack4abnormality = reader.Read<int32_t>();
        attack4abnormalityDuration = reader.Read<int32_t>();
        attack4abnormalityProbmod = reader.Read<int32_t>();
        attack4abnormalityReserved1 = reader.Read<int32_t>();
        attack4abnormalityReserved4 = reader.Read<int32_t>();
        attack4atkanipartid = reader.Read<int32_t>();
        attack4atkprm = reader.Read<int32_t>();
        attack4delay = reader.Read<int32_t>();
        attack4beatkani = reader.Read<int32_t>();
        attack4FxType = reader.Read<int32_t>();
        attack4FxId = reader.Read<int32_t>();
        attack4Sound = reader.Read<int32_t>();
    }

    void MonsterPart2Table::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "MONSTER_PART2.sox");
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

    auto MonsterPart2Table::Find(int32_t index) const -> const MonsterPart2*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto MonsterPart2Table::Get() const -> const std::vector<MonsterPart2>&
    {
        return _vector;
    }
}
