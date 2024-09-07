#include "levelup_ability.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    LevelupAbility::LevelupAbility(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        w1Ability = reader.Read<int32_t>();
        w1Quest = reader.Read<int32_t>();
        w1Title = reader.ReadString(reader.Read<uint16_t>());
        w2Ability = reader.Read<int32_t>();
        w2Quest = reader.Read<int32_t>();
        w2Title = reader.ReadString(reader.Read<uint16_t>());
        w3Ability = reader.Read<int32_t>();
        w3Quest = reader.Read<int32_t>();
        w3Title = reader.ReadString(reader.Read<uint16_t>());
        c1Ability = reader.Read<int32_t>();
        c1Quest = reader.Read<int32_t>();
        c1Title = reader.ReadString(reader.Read<uint16_t>());
        c2Ability = reader.Read<int32_t>();
        c2Quest = reader.Read<int32_t>();
        c2Title = reader.ReadString(reader.Read<uint16_t>());
        c3Ability = reader.Read<int32_t>();
        c3Quest = reader.Read<int32_t>();
        c3Title = reader.ReadString(reader.Read<uint16_t>());
        d1Ability = reader.Read<int32_t>();
        d1Quest = reader.Read<int32_t>();
        d1Title = reader.ReadString(reader.Read<uint16_t>());
        d2Ability = reader.Read<int32_t>();
        d2Quest = reader.Read<int32_t>();
        d2Title = reader.ReadString(reader.Read<uint16_t>());
        d3Ability = reader.Read<int32_t>();
        d3Quest = reader.Read<int32_t>();
        d3Title = reader.ReadString(reader.Read<uint16_t>());
        e1Ability = reader.Read<int32_t>();
        e1Quest = reader.Read<int32_t>();
        e1Title = reader.ReadString(reader.Read<uint16_t>());
        e2Ability = reader.Read<int32_t>();
        e2Quest = reader.Read<int32_t>();
        e2Title = reader.ReadString(reader.Read<uint16_t>());
        e3Ability = reader.Read<int32_t>();
        e3Quest = reader.Read<int32_t>();
        e3Title = reader.ReadString(reader.Read<uint16_t>());
        a1Ability = reader.Read<int32_t>();
        a1Quest = reader.Read<int32_t>();
        a1Title = reader.ReadString(reader.Read<uint16_t>());
        a2Ability = reader.Read<int32_t>();
        a2Quest = reader.Read<int32_t>();
        a2Title = reader.ReadString(reader.Read<uint16_t>());
        a3Ability = reader.Read<int32_t>();
        a3Quest = reader.Read<int32_t>();
        a3Title = reader.ReadString(reader.Read<uint16_t>());
    }

    void LevelupAbilityTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "LEVELUP_ABILITY.sox");
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

    auto LevelupAbilityTable::Find(int32_t index) const -> const LevelupAbility*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto LevelupAbilityTable::Get() const -> const std::vector<LevelupAbility>&
    {
        return _vector;
    }
}
