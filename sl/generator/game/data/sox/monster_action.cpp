#include "monster_action.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    MonsterAction::MonsterAction(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        modelID = reader.Read<int32_t>();
        bodySize = reader.Read<int32_t>();
        genFxType = reader.Read<int32_t>();
        sightRange = reader.Read<int32_t>();
        chaseRange = reader.Read<int32_t>();
        canGoOut = reader.Read<int32_t>();
        domain = reader.Read<int32_t>();
        characteristic = reader.Read<int32_t>();
        levelDiffMax = reader.Read<int32_t>();
        maxEnemy = reader.Read<int32_t>();
        scanTime = reader.Read<int32_t>();
        cancelTime = reader.Read<int32_t>();
        moveRangeMin = reader.Read<int32_t>();
        moveRangeMax = reader.Read<int32_t>();
        moveDelayMin = reader.Read<int32_t>();
        moveDelayMax = reader.Read<int32_t>();
        moveDelayChase = reader.Read<int32_t>();
        followship1 = reader.Read<int32_t>();
        followship2 = reader.Read<int32_t>();
        followship3 = reader.Read<int32_t>();
        followship4 = reader.Read<int32_t>();
        guardID = reader.Read<int32_t>();
        childID = reader.Read<int32_t>();
        childSummonDelay = reader.Read<int32_t>();
        childCount = reader.Read<int32_t>();
        childSummonFrame = reader.Read<int32_t>();
        hostileLenFactor = reader.Read<int32_t>();
        hostileHpFactor = reader.Read<int32_t>();
        hostileDefFactor = reader.Read<int32_t>();
        hostileOtherAtt = reader.Read<float>();
        chaseScore = reader.Read<int32_t>();
        memoryTime = reader.Read<int32_t>();
        aerialHeight = reader.Read<int32_t>();
        deadSound = reader.Read<int32_t>();
        moveSound = reader.Read<int32_t>();
    }

    void MonsterActionTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "MONSTER_ACTION.sox");
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

    auto MonsterActionTable::Find(int32_t index) const -> const MonsterAction*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto MonsterActionTable::Get() const -> const std::vector<MonsterAction>&
    {
        return _vector;
    }
}
