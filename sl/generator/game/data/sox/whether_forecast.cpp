#include "whether_forecast.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    WhetherForecast::WhetherForecast(StreamReader<std::vector<char>::const_iterator>& reader)
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

    void WhetherForecastTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "WHETHER_FORECAST.sox");
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

    auto WhetherForecastTable::Find(int32_t index) const -> const WhetherForecast*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto WhetherForecastTable::Get() const -> const std::vector<WhetherForecast>&
    {
        return _vector;
    }
}
