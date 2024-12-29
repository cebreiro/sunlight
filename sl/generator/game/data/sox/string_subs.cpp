#include "string_subs.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    StringSubs::StringSubs(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        org = reader.ReadString(reader.Read<uint16_t>());
        subs = reader.ReadString(reader.Read<uint16_t>());
    }

    void StringSubsTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "STRING_SUBS.sox");
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

    auto StringSubsTable::Find(int32_t index) const -> const StringSubs*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto StringSubsTable::Get() const -> const std::vector<StringSubs>&
    {
        return _vector;
    }
}
