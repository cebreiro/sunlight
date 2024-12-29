#include "monster_attack_type.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    MonsterAttackType::MonsterAttackType(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        name = reader.ReadString(reader.Read<uint16_t>());
        attackdamage = reader.Read<float>();
    }

    void MonsterAttackTypeTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "MONSTER_ATTACK_TYPE.sox");
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

    auto MonsterAttackTypeTable::Find(int32_t index) const -> const MonsterAttackType*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto MonsterAttackTypeTable::Get() const -> const std::vector<MonsterAttackType>&
    {
        return _vector;
    }
}
