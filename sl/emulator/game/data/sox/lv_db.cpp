#include "lv_db.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    LvDb::LvDb(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        job = reader.Read<int32_t>();
        jobStatus = reader.Read<int32_t>();
        power = reader.Read<int32_t>();
        speed = reader.Read<int32_t>();
        health = reader.Read<int32_t>();
        _int = reader.Read<int32_t>();
        will = reader.Read<int32_t>();
    }

    void LvDbTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "LV_DB.sox");
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

    auto LvDbTable::Find(int32_t index) const -> const LvDb*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto LvDbTable::Get() const -> const std::vector<LvDb>&
    {
        return _vector;
    }
}
