#include "minigame_interface.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    MinigameInterface::MinigameInterface(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        gameName = reader.ReadString(reader.Read<uint16_t>());
        minNumTeams = reader.Read<int32_t>();
        maxNumTeams = reader.Read<int32_t>();
        minTeamSize = reader.Read<int32_t>();
        maxTeamSize = reader.Read<int32_t>();
        maxNumObs = reader.Read<int32_t>();
        teamMinPlayer = reader.Read<int32_t>();
        equalPlayer = reader.Read<int32_t>();
        maxBet = reader.Read<int32_t>();
        param1Name = reader.Read<int32_t>();
        param1Min = reader.Read<int32_t>();
        param1Max = reader.Read<int32_t>();
        param2Name = reader.Read<int32_t>();
        param2Min = reader.Read<int32_t>();
        param2Max = reader.Read<int32_t>();
        param3Name = reader.Read<int32_t>();
        param3Min = reader.Read<int32_t>();
        param3Max = reader.Read<int32_t>();
    }

    void MinigameInterfaceTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "MINIGAME_INTERFACE.sox");
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

    auto MinigameInterfaceTable::Find(int32_t index) const -> const MinigameInterface*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto MinigameInterfaceTable::Get() const -> const std::vector<MinigameInterface>&
    {
        return _vector;
    }
}
