#include "monster_move_type.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    MonsterMoveType::MonsterMoveType(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        movename = reader.ReadString(reader.Read<uint16_t>());
        movespeed = reader.Read<float>();
        gyrationangle1 = reader.Read<int32_t>();
        gyrationangle2 = reader.Read<int32_t>();
    }

    void MonsterMoveTypeTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "MONSTER_MOVE_TYPE.sox");
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

    auto MonsterMoveTypeTable::Find(int32_t index) const -> const MonsterMoveType*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto MonsterMoveTypeTable::Get() const -> const std::vector<MonsterMoveType>&
    {
        return _vector;
    }
}
