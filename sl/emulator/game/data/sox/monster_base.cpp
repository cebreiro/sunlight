#include "monster_base.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    MonsterBase::MonsterBase(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        name = reader.ReadString(reader.Read<uint16_t>());
        level = reader.Read<int32_t>();
        exp = reader.Read<int32_t>();
        partyExp = reader.Read<int32_t>();
        race = reader.Read<int32_t>();
        hp = reader.Read<int32_t>();
        patkMin = reader.Read<int32_t>();
        patkMax = reader.Read<int32_t>();
        phrat = reader.Read<int32_t>();
        perat = reader.Read<int32_t>();
        pdefSlash = reader.Read<int32_t>();
        pdefBash = reader.Read<int32_t>();
        pdefPierce = reader.Read<int32_t>();
        matk = reader.Read<int32_t>();
        mres = reader.Read<int32_t>();
        elemF = reader.Read<int32_t>();
        elemW = reader.Read<int32_t>();
        elemL = reader.Read<int32_t>();
        speed = reader.Read<int32_t>();
        speedChase = reader.Read<int32_t>();
        debuffRes = reader.Read<int32_t>();
        lifetime = reader.Read<int32_t>();
        activationStartTime = reader.Read<int32_t>();
        activationEndTime = reader.Read<int32_t>();
        material = reader.Read<int32_t>();
    }

    void MonsterBaseTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "MONSTER_BASE.sox");
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

    auto MonsterBaseTable::Find(int32_t index) const -> const MonsterBase*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto MonsterBaseTable::Get() const -> const std::vector<MonsterBase>&
    {
        return _vector;
    }
}
