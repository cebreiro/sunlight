#include "minigame_ufo.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    MinigameUfo::MinigameUfo(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        ufoname = reader.ReadString(reader.Read<uint16_t>());
        cost = reader.Read<int32_t>();
        velocity = reader.Read<int32_t>();
        accel = reader.Read<int32_t>();
        radius = reader.Read<int32_t>();
        mingaugespeed = reader.Read<int32_t>();
        maxgaugespeed = reader.Read<int32_t>();
        mingaugemove = reader.Read<int32_t>();
        maxgaugemove = reader.Read<int32_t>();
        maxRarity = reader.Read<int32_t>();
        respawnTime = reader.Read<int32_t>();
        item1 = reader.Read<int32_t>();
        item2 = reader.Read<int32_t>();
        item3 = reader.Read<int32_t>();
        item4 = reader.Read<int32_t>();
        item5 = reader.Read<int32_t>();
        item6 = reader.Read<int32_t>();
    }

    void MinigameUfoTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "MINIGAME_UFO.sox");
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

    auto MinigameUfoTable::Find(int32_t index) const -> const MinigameUfo*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto MinigameUfoTable::Get() const -> const std::vector<MinigameUfo>&
    {
        return _vector;
    }
}
