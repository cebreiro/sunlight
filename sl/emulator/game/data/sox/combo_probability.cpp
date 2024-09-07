#include "combo_probability.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    ComboProbability::ComboProbability(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        enableCombo2 = reader.Read<float>();
        enableCombo3 = reader.Read<float>();
        enableCombo4 = reader.Read<float>();
        enableCombo5 = reader.Read<float>();
        enableCombo6 = reader.Read<float>();
        enableCombo7 = reader.Read<float>();
        enableCombo8 = reader.Read<float>();
        enableCombo9 = reader.Read<float>();
        enableCombo10 = reader.Read<float>();
    }

    void ComboProbabilityTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "COMBO_PROBABILITY.sox");
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

    auto ComboProbabilityTable::Find(int32_t index) const -> const ComboProbability*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ComboProbabilityTable::Get() const -> const std::vector<ComboProbability>&
    {
        return _vector;
    }
}
