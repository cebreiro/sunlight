#include "itemmix_table.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    ItemmixTable::ItemmixTable(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        groupID = reader.Read<int32_t>();
        name = reader.ReadString(reader.Read<uint16_t>());
        gradeTableType = reader.Read<int32_t>();
        mixTime = reader.Read<int32_t>();
        difficulty = reader.Read<int32_t>();
        resultID = reader.Read<int32_t>();
        resultMax = reader.Read<int32_t>();
        count = reader.Read<int32_t>();
        material1ID = reader.Read<int32_t>();
        material1Name = reader.ReadString(reader.Read<uint16_t>());
        material1NU = reader.Read<int32_t>();
        material2ID = reader.Read<int32_t>();
        material2Name = reader.ReadString(reader.Read<uint16_t>());
        material2NU = reader.Read<int32_t>();
        material3ID = reader.Read<int32_t>();
        material3Name = reader.ReadString(reader.Read<uint16_t>());
        material3NU = reader.Read<int32_t>();
        material4ID = reader.Read<int32_t>();
        material4Name = reader.ReadString(reader.Read<uint16_t>());
        material4NU = reader.Read<int32_t>();
        maxDepth = reader.Read<int32_t>();
        depthID1 = reader.Read<int32_t>();
        depthID2 = reader.Read<int32_t>();
        depthID3 = reader.Read<int32_t>();
        depthID4 = reader.Read<int32_t>();
        expCorrect = reader.Read<int32_t>();
    }

    void ItemmixTableTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "ITEMMIX_TABLE.sox");
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

    auto ItemmixTableTable::Find(int32_t index) const -> const ItemmixTable*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ItemmixTableTable::Get() const -> const std::vector<ItemmixTable>&
    {
        return _vector;
    }
}
