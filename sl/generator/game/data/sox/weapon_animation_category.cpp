#include "weapon_animation_category.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    WeaponAnimationCategory::WeaponAnimationCategory(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        _default = reader.Read<int32_t>();
        walk = reader.Read<int32_t>();
        run = reader.Read<int32_t>();
        idle = reader.Read<int32_t>();
        disarm = reader.Read<int32_t>();
        rearm = reader.Read<int32_t>();
        attacka = reader.Read<int32_t>();
        attackb = reader.Read<int32_t>();
        attackc = reader.Read<int32_t>();
        attackd = reader.Read<int32_t>();
        reload = reader.Read<int32_t>();
        attackaTime = reader.Read<int32_t>();
        attackbTime = reader.Read<int32_t>();
        attackcTime = reader.Read<int32_t>();
        attackdTime = reader.Read<int32_t>();
    }

    void WeaponAnimationCategoryTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "WEAPON_ANIMATION_CATEGORY.sox");
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

    auto WeaponAnimationCategoryTable::Find(int32_t index) const -> const WeaponAnimationCategory*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto WeaponAnimationCategoryTable::Get() const -> const std::vector<WeaponAnimationCategory>&
    {
        return _vector;
    }
}
