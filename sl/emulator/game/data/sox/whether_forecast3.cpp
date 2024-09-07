#include "whether_forecast3.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    WhetherForecast3::WhetherForecast3(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        whetherType = reader.Read<int32_t>();
        lightType = reader.Read<int32_t>();
        density = reader.Read<int32_t>();
        cloudType = reader.Read<int32_t>();
        fog = reader.Read<int32_t>();
        fogRate = reader.Read<float>();
        fogColorR = reader.Read<int32_t>();
        fogColorG = reader.Read<int32_t>();
        fogColorB = reader.Read<int32_t>();
        cloudA = reader.Read<int32_t>();
        cloudB = reader.Read<int32_t>();
    }

    void WhetherForecast3Table::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "WHETHER_FORECAST3.sox");
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

    auto WhetherForecast3Table::Find(int32_t index) const -> const WhetherForecast3*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto WhetherForecast3Table::Get() const -> const std::vector<WhetherForecast3>&
    {
        return _vector;
    }
}
