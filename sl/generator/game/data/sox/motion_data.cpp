#include "motion_data.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    MotionData::MotionData(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        aniTime = reader.Read<int32_t>();
        hitTime = reader.Read<int32_t>();
        bladeStart = reader.Read<int32_t>();
        bladeEnd = reader.Read<int32_t>();
        attackSound = reader.Read<int32_t>();
        attackDirection = reader.Read<int32_t>();
        damagePercent = reader.Read<float>();
        damageType = reader.Read<int32_t>();
        damageTypePercent = reader.Read<int32_t>();
        targetblowFX = reader.Read<int32_t>();
    }

    void MotionDataTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "MOTION_DATA.sox");
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

    auto MotionDataTable::Find(int32_t index) const -> const MotionData*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto MotionDataTable::Get() const -> const std::vector<MotionData>&
    {
        return _vector;
    }
}
