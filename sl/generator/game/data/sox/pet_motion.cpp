#include "pet_motion.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    PetMotion::PetMotion(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        name = reader.ReadString(reader.Read<uint16_t>());
        modelId = reader.Read<int32_t>();
        generateFrame = reader.Read<int32_t>();
        lovelyFrame = reader.Read<int32_t>();
        afflictFrame = reader.Read<int32_t>();
        eatFrame = reader.Read<int32_t>();
        eatTiming = reader.Read<int32_t>();
        hungryFrame = reader.Read<int32_t>();
        idle01Frame = reader.Read<int32_t>();
        idle02Frame = reader.Read<int32_t>();
        idle03Frame = reader.Read<int32_t>();
        idle04Frame = reader.Read<int32_t>();
        idle05Frame = reader.Read<int32_t>();
        attackFrame = reader.Read<int32_t>();
        attackTiming = reader.Read<int32_t>();
        pickupFrame = reader.Read<int32_t>();
        pickupTiming = reader.Read<int32_t>();
    }

    void PetMotionTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "PET_MOTION.sox");
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

    auto PetMotionTable::Find(int32_t index) const -> const PetMotion*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto PetMotionTable::Get() const -> const std::vector<PetMotion>&
    {
        return _vector;
    }
}
