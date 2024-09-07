#include "pet_conversion.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    PetConversion::PetConversion(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        maxSt = reader.Read<int32_t>();
        exp = reader.Read<int32_t>();
        stInc = reader.Read<int32_t>();
        npDec = reader.Read<float>();
        expDecNp0 = reader.Read<int32_t>();
        freqIdleMotion = reader.Read<float>();
        moveSpeed = reader.Read<int32_t>();
        caressCount = reader.Read<float>();
        caressExp = reader.Read<int32_t>();
        annoyCount = reader.Read<float>();
        annoyExpInc = reader.Read<int32_t>();
        annoyExpDec = reader.Read<int32_t>();
        abFetchScanRange = reader.Read<int32_t>();
        abFetchScanFreq = reader.Read<float>();
        abFetchNoItem = reader.Read<int32_t>();
        abAttackAttackPower = reader.Read<int32_t>();
        abAttackScanRange = reader.Read<int32_t>();
        abAttackScanFreq = reader.Read<float>();
    }

    void PetConversionTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "PET_CONVERSION.sox");
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

    auto PetConversionTable::Find(int32_t index) const -> const PetConversion*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto PetConversionTable::Get() const -> const std::vector<PetConversion>&
    {
        return _vector;
    }
}
