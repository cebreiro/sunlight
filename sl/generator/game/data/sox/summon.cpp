#include "summon.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    Summon::Summon(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        monstername = reader.ReadString(reader.Read<uint16_t>());
        modelid = reader.Read<int32_t>();
        level = reader.Read<int32_t>();
        hp = reader.Read<int32_t>();
        patk = reader.Read<int32_t>();
        phrat = reader.Read<int32_t>();
        perat = reader.Read<int32_t>();
        pdef = reader.Read<int32_t>();
        resDamage = reader.Read<int32_t>();
        death1delay = reader.Read<int32_t>();
        lifeTime = reader.Read<int32_t>();
        aiType = reader.Read<int32_t>();
        basicspeed = reader.Read<int32_t>();
        sight = reader.Read<int32_t>();
        chase = reader.Read<int32_t>();
        size = reader.Read<int32_t>();
        characteristic = reader.Read<int32_t>();
        searchtime = reader.Read<int32_t>();
        moveType = reader.Read<int32_t>();
        moveRangeMin = reader.Read<int32_t>();
        moveRangeMax = reader.Read<int32_t>();
        moveDelayMin = reader.Read<int32_t>();
        moveDelayMax = reader.Read<int32_t>();
        attack1dmgrfactor = reader.Read<float>();
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
        attack4atkanipartid = reader.Read<int32_t>();
        attack4atkprm = reader.Read<int32_t>();
        attack4delay = reader.Read<int32_t>();
        attack4beatkani = reader.Read<int32_t>();
        attack4FxType = reader.Read<int32_t>();
        attack4FxId = reader.Read<int32_t>();
        attack4Sound = reader.Read<int32_t>();
    }

    void SummonTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "SUMMON.sox");
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

    auto SummonTable::Find(int32_t index) const -> const Summon*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto SummonTable::Get() const -> const std::vector<Summon>&
    {
        return _vector;
    }
}
