#include "minigame_typing_word.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    MinigameTypingWord::MinigameTypingWord(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        typingWord = reader.ReadString(reader.Read<uint16_t>());
    }

    void MinigameTypingWordTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "MINIGAME_TYPING_WORD.sox");
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

    auto MinigameTypingWordTable::Find(int32_t index) const -> const MinigameTypingWord*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto MinigameTypingWordTable::Get() const -> const std::vector<MinigameTypingWord>&
    {
        return _vector;
    }
}
