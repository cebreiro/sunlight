#include "pet_ability.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    PetAbility::PetAbility(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        name = reader.ReadString(reader.Read<uint16_t>());
        abilityType = reader.Read<int32_t>();
        autoAbility = reader.Read<int32_t>();
        delay = reader.Read<int32_t>();
        levelRestriction = reader.Read<int32_t>();
    }

    void PetAbilityTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "PET_ABILITY.sox");
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

    auto PetAbilityTable::Find(int32_t index) const -> const PetAbility*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto PetAbilityTable::Get() const -> const std::vector<PetAbility>&
    {
        return _vector;
    }
}
