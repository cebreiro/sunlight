#include "cafefood.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    Cafefood::Cafefood(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        foodName = reader.ReadString(reader.Read<uint16_t>());
        foodModelid = reader.Read<int32_t>();
        foodEatType = reader.Read<int32_t>();
        foodEatCount = reader.Read<int32_t>();
        foodCount = reader.Read<int32_t>();
        foodPrice = reader.Read<int32_t>();
        foodDesc = reader.ReadString(reader.Read<uint16_t>());
        foodEffect = reader.Read<int32_t>();
        foodEffectValue = reader.Read<int32_t>();
    }

    void CafefoodTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "CAFEFOOD.sox");
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

    auto CafefoodTable::Find(int32_t index) const -> const Cafefood*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto CafefoodTable::Get() const -> const std::vector<Cafefood>&
    {
        return _vector;
    }
}
