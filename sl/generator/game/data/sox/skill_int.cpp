#include "skill_int.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    SkillInt::SkillInt(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        description = reader.ReadString(reader.Read<uint16_t>());
        textureid = reader.Read<int32_t>();
        count = reader.Read<int32_t>();
    }

    void SkillIntTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "skill_int.sox");
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

    auto SkillIntTable::Find(int32_t index) const -> const SkillInt*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto SkillIntTable::Get() const -> const std::vector<SkillInt>&
    {
        return _vector;
    }
}
