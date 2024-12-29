#include "quest_info.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    QuestInfo::QuestInfo(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        descFuncId = reader.Read<int32_t>();
        questCancelable = reader.Read<int32_t>();
    }

    void QuestInfoTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "QuestInfo.sox");
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

    auto QuestInfoTable::Find(int32_t index) const -> const QuestInfo*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto QuestInfoTable::Get() const -> const std::vector<QuestInfo>&
    {
        return _vector;
    }
}
