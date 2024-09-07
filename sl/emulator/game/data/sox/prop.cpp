#include "prop.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    Prop::Prop(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        name = reader.ReadString(reader.Read<uint16_t>());
        playerActionCategory = reader.Read<int32_t>();
        minRange = reader.Read<int32_t>();
        posX = reader.Read<int32_t>();
        posY = reader.Read<int32_t>();
        posZ = reader.Read<int32_t>();
    }

    void PropTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "PROP.sox");
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

    auto PropTable::Find(int32_t index) const -> const Prop*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto PropTable::Get() const -> const std::vector<Prop>&
    {
        return _vector;
    }
}
