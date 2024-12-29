#include "lvup_db.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    LvupDb::LvupDb(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        lvupVg = reader.Read<int32_t>();
        lvupG = reader.Read<int32_t>();
        lvupL = reader.Read<int32_t>();
        lvupP = reader.Read<int32_t>();
    }

    void LvupDbTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "LVUP_DB.sox");
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

    auto LvupDbTable::Find(int32_t index) const -> const LvupDb*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto LvupDbTable::Get() const -> const std::vector<LvupDb>&
    {
        return _vector;
    }
}
