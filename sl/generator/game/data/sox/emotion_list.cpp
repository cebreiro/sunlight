#include "emotion_list.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    EmotionList::EmotionList(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        name = reader.ReadString(reader.Read<uint16_t>());
        group = reader.Read<int32_t>();
        mmotionId = reader.Read<int32_t>();
        fmotionId = reader.Read<int32_t>();
        mfxId = reader.Read<int32_t>();
        ffxId = reader.Read<int32_t>();
        page = reader.Read<int32_t>();
        levelRequire = reader.Read<int32_t>();
        itemRequire = reader.Read<int32_t>();
        itemConsume = reader.Read<int32_t>();
        emotionIndicate = reader.Read<int32_t>();
    }

    void EmotionListTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "EMOTION_LIST.sox");
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

    auto EmotionListTable::Find(int32_t index) const -> const EmotionList*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto EmotionListTable::Get() const -> const std::vector<EmotionList>&
    {
        return _vector;
    }
}
