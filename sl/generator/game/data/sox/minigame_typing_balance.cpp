#include "minigame_typing_balance.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    MinigameTypingBalance::MinigameTypingBalance(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        playerSpeed = reader.Read<int32_t>();
        monsterSpeed = reader.Read<int32_t>();
        monsterAccel = reader.Read<int32_t>();
        monsterAttack = reader.Read<int32_t>();
        itemTerm = reader.Read<int32_t>();
        itemDuration = reader.Read<int32_t>();
        item1Duration = reader.Read<int32_t>();
        item1Effect = reader.Read<int32_t>();
        item2Duration = reader.Read<int32_t>();
        item2Effect = reader.Read<int32_t>();
        item3Duration = reader.Read<int32_t>();
        item3Effect = reader.Read<int32_t>();
        item4Duration = reader.Read<int32_t>();
        item4Effect = reader.Read<int32_t>();
        item5Duration = reader.Read<int32_t>();
        item5Effect = reader.Read<int32_t>();
    }

    void MinigameTypingBalanceTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "MINIGAME_TYPING_BALANCE.sox");
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

    auto MinigameTypingBalanceTable::Find(int32_t index) const -> const MinigameTypingBalance*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto MinigameTypingBalanceTable::Get() const -> const std::vector<MinigameTypingBalance>&
    {
        return _vector;
    }
}
