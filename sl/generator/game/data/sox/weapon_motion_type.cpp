#include "weapon_motion_type.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    WeaponMotionType::WeaponMotionType(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        _default = reader.Read<int32_t>();
        defaultSpeed = reader.Read<float>();
        walk = reader.Read<int32_t>();
        walkSpeed = reader.Read<float>();
        run = reader.Read<int32_t>();
        runSpeed = reader.Read<float>();
        idle = reader.Read<int32_t>();
        idleSpeed = reader.Read<float>();
        disArm = reader.Read<int32_t>();
        disArmSpeed = reader.Read<float>();
        reArm = reader.Read<int32_t>();
        reArmSpeed = reader.Read<float>();
        reLoad = reader.Read<int32_t>();
        reLoadSpeed = reader.Read<float>();
        maxCombo = reader.Read<int32_t>();
        attackMotion1 = reader.Read<int32_t>();
        attackMotion1Speed = reader.Read<float>();
        attackMotion2 = reader.Read<int32_t>();
        attackMotion2Speed = reader.Read<float>();
        attackMotion3 = reader.Read<int32_t>();
        attackMotion3Speed = reader.Read<float>();
        attackMotion4 = reader.Read<int32_t>();
        attackMotion4Speed = reader.Read<float>();
        attackMotion5 = reader.Read<int32_t>();
        attackMotion5Speed = reader.Read<float>();
        attackMotion6 = reader.Read<int32_t>();
        attackMotion6Speed = reader.Read<float>();
        attackMotion7 = reader.Read<int32_t>();
        attackMotion7Speed = reader.Read<float>();
        attackMotion8 = reader.Read<int32_t>();
        attackMotion8Speed = reader.Read<float>();
        attackMotion9 = reader.Read<int32_t>();
        attackMotion9Speed = reader.Read<float>();
        attackMotion10 = reader.Read<int32_t>();
        attackMotion10Speed = reader.Read<float>();
        feedbackMotion1 = reader.Read<int32_t>();
        feedbackMotion1Speed = reader.Read<float>();
        feedbackMotion2 = reader.Read<int32_t>();
        feedbackMotion2Speed = reader.Read<float>();
        feedbackMotion3 = reader.Read<int32_t>();
        feedbackMotion3Speed = reader.Read<float>();
        feedbackMotion4 = reader.Read<int32_t>();
        feedbackMotion4Speed = reader.Read<float>();
        feedbackMotion5 = reader.Read<int32_t>();
        feedbackMotion5Speed = reader.Read<float>();
        feedbackMotion6 = reader.Read<int32_t>();
        feedbackMotion6Speed = reader.Read<float>();
        feedbackMotion7 = reader.Read<int32_t>();
        feedbackMotion7Speed = reader.Read<float>();
        feedbackMotion8 = reader.Read<int32_t>();
        feedbackMotion8Speed = reader.Read<float>();
        feedbackMotion9 = reader.Read<int32_t>();
        feedbackMotion9Speed = reader.Read<float>();
        feedbackMotion10 = reader.Read<int32_t>();
        feedbackMotion10Speed = reader.Read<float>();
    }

    void WeaponMotionTypeTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "weapon_motion_type.sox");
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

    auto WeaponMotionTypeTable::Find(int32_t index) const -> const WeaponMotionType*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto WeaponMotionTypeTable::Get() const -> const std::vector<WeaponMotionType>&
    {
        return _vector;
    }
}
