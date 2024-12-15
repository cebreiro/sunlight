#include "minigame_typing.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    MinigameTyping::MinigameTyping(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        monsterId = reader.Read<int32_t>();
        monsterName = reader.ReadString(reader.Read<uint16_t>());
        monsterModelId = reader.Read<int32_t>();
        monsterSpeed = reader.Read<int32_t>();
        monsterAdd = reader.Read<int32_t>();
        minWordLength = reader.Read<int32_t>();
        maxWordLength = reader.Read<int32_t>();
    }

    void MinigameTypingTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "MINIGAME_TYPING.sox");
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

    auto MinigameTypingTable::Find(int32_t index) const -> const MinigameTyping*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto MinigameTypingTable::Get() const -> const std::vector<MinigameTyping>&
    {
        return _vector;
    }
}
