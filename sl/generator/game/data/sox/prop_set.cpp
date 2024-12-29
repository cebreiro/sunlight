#include "prop_set.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    PropSet::PropSet(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        numSubProp = reader.Read<int32_t>();
        subpropID = reader.Read<int32_t>();
    }

    void PropSetTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "PROP_SET.sox");
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

    auto PropSetTable::Find(int32_t index) const -> const PropSet*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto PropSetTable::Get() const -> const std::vector<PropSet>&
    {
        return _vector;
    }
}
