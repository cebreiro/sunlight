#include "interior_furniture.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    InteriorFurniture::InteriorFurniture(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        furnitureId = reader.Read<int32_t>();
        furnitureType = reader.Read<int32_t>();
        furnitureLv = reader.Read<int32_t>();
        furnitureCost = reader.Read<int32_t>();
        furnitureDesc = reader.Read<int32_t>();
        furniturePreview = reader.Read<int32_t>();
        furnitureAnitype = reader.Read<int32_t>();
    }

    void InteriorFurnitureTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "INTERIOR_FURNITURE.sox");
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

    auto InteriorFurnitureTable::Find(int32_t index) const -> const InteriorFurniture*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto InteriorFurnitureTable::Get() const -> const std::vector<InteriorFurniture>&
    {
        return _vector;
    }
}
