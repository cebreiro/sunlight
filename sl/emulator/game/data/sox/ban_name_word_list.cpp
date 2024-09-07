#include "ban_name_word_list.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    BanNameWordList::BanNameWordList(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        banStr = reader.ReadString(reader.Read<uint16_t>());
        banType = reader.Read<int32_t>();
    }

    void BanNameWordListTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "BAN_NAME_WORD_LIST.sox");
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

    auto BanNameWordListTable::Find(int32_t index) const -> const BanNameWordList*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto BanNameWordListTable::Get() const -> const std::vector<BanNameWordList>&
    {
        return _vector;
    }
}
