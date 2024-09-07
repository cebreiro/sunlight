#include "job_reference.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    JobReference::JobReference(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        maxJobLevel = reader.Read<int32_t>();
        commonJobMod = reader.Read<int32_t>();
        mHPFactor = reader.Read<float>();
        mSPFactor = reader.Read<float>();
        pHRatFactor = reader.Read<float>();
        defenseFactor = reader.Read<float>();
        hPRecoveryFactor = reader.Read<float>();
        sPRecoveryFactor = reader.Read<float>();
        avatarSpeedFactor = reader.Read<float>();
        comment = reader.ReadString(reader.Read<uint16_t>());
    }

    void JobReferenceTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "JOB_REFERENCE.sox");
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

    auto JobReferenceTable::Find(int32_t index) const -> const JobReference*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto JobReferenceTable::Get() const -> const std::vector<JobReference>&
    {
        return _vector;
    }
}
