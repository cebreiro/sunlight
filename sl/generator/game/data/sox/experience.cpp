#include "experience.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    Experience::Experience(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        cLvExp = reader.Read<int32_t>();
        jLvExp = reader.Read<int32_t>();
    }

    void ExperienceTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "EXPERIENCE.sox");
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

    auto ExperienceTable::Find(int32_t index) const -> const Experience*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ExperienceTable::Get() const -> const std::vector<Experience>&
    {
        return _vector;
    }
}
