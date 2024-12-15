#include "stringtablekorean_tip.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    StringtablekoreanTip::StringtablekoreanTip(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        string = reader.ReadString(reader.Read<uint16_t>());
        desc = reader.ReadString(reader.Read<uint16_t>());
    }

    void StringtablekoreanTipTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "STRINGTABLEKOREAN_TIP.sox");
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

    auto StringtablekoreanTipTable::Find(int32_t index) const -> const StringtablekoreanTip*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto StringtablekoreanTipTable::Get() const -> const std::vector<StringtablekoreanTip>&
    {
        return _vector;
    }
}
