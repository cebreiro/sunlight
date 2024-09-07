#include "minigame_rank.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    MinigameRank::MinigameRank(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        rankValue = reader.ReadString(reader.Read<uint16_t>());
        rankDesc = reader.ReadString(reader.Read<uint16_t>());
    }

    void MinigameRankTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "MINIGAME_RANK.sox");
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

    auto MinigameRankTable::Find(int32_t index) const -> const MinigameRank*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto MinigameRankTable::Get() const -> const std::vector<MinigameRank>&
    {
        return _vector;
    }
}
