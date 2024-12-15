#include "minigame_slmon.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    MinigameSlmon::MinigameSlmon(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        unitname = reader.ReadString(reader.Read<uint16_t>());
        modelid = reader.Read<int32_t>();
        unithp = reader.Read<int32_t>();
        boss = reader.Read<int32_t>();
        flying = reader.Read<int32_t>();
        moverule = reader.Read<int32_t>();
        team = reader.Read<int32_t>();
        startpoint1 = reader.ReadString(reader.Read<uint16_t>());
        startpoint2 = reader.ReadString(reader.Read<uint16_t>());
        startpoint3 = reader.ReadString(reader.Read<uint16_t>());
        startpoint4 = reader.ReadString(reader.Read<uint16_t>());
        startpoint5 = reader.ReadString(reader.Read<uint16_t>());
        attackbeatani = reader.Read<int32_t>();
        attackFxId = reader.Read<int32_t>();
        attackSound = reader.Read<int32_t>();
    }

    void MinigameSlmonTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "MINIGAME_SLMON.sox");
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

    auto MinigameSlmonTable::Find(int32_t index) const -> const MinigameSlmon*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto MinigameSlmonTable::Get() const -> const std::vector<MinigameSlmon>&
    {
        return _vector;
    }
}
