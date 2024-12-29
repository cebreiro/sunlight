#include "lv_ext_db.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    LvExtDb::LvExtDb(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        ext = reader.Read<int32_t>();
    }

    void LvExtDbTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "LV_EXT_DB.sox");
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

    auto LvExtDbTable::Find(int32_t index) const -> const LvExtDb*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto LvExtDbTable::Get() const -> const std::vector<LvExtDb>&
    {
        return _vector;
    }
}
