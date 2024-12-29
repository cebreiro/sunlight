#include "item_etc.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    ItemEtc::ItemEtc(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        name = reader.ReadString(reader.Read<uint16_t>());
        generate = reader.Read<int32_t>();
        explanation = reader.ReadString(reader.Read<uint16_t>());
        invenWidth = reader.Read<int32_t>();
        invenHeight = reader.Read<int32_t>();
        maxOverlapCount = reader.Read<int32_t>();
        rarity = reader.Read<int32_t>();
        price = reader.Read<int32_t>();
        modelID = reader.Read<int32_t>();
        modelColor = reader.Read<int32_t>();
        dropSound = reader.Read<int32_t>();
        useQuickSlot = reader.Read<int32_t>();
        oneMoreItem = reader.Read<int32_t>();
        equipPos = reader.Read<int32_t>();
        money = reader.Read<int32_t>();
        bulletType = reader.Read<int32_t>();
        ableToSell = reader.Read<int32_t>();
        ableToTrade = reader.Read<int32_t>();
        ableToDrop = reader.Read<int32_t>();
        ableToDestroy = reader.Read<int32_t>();
        ableToStorage = reader.Read<int32_t>();
        skillID = reader.Read<int32_t>();
        grade = reader.Read<int32_t>();
        isTool = reader.Read<int32_t>();
        mixDistance = reader.Read<int32_t>();
        interfaceAniID = reader.Read<int32_t>();
        toolMotionID = reader.Read<int32_t>();
        charMotionLoopID = reader.Read<int32_t>();
        charMotionStartID = reader.Read<int32_t>();
        toolSoundID = reader.Read<int32_t>();
        toolEventSoundID = reader.Read<int32_t>();
    }

    void ItemEtcTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "item_etc.sox");
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

    auto ItemEtcTable::Find(int32_t index) const -> const ItemEtc*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ItemEtcTable::Get() const -> const std::vector<ItemEtc>&
    {
        return _vector;
    }
}
