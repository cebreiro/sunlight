#include "itemtable.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    Itemtable::Itemtable(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        name = reader.ReadString(reader.Read<uint16_t>());
        generate = reader.Read<int32_t>();
        category = reader.Read<int32_t>();
        type = reader.Read<int32_t>();
        job = reader.Read<int32_t>();
        explanation = reader.ReadString(reader.Read<uint16_t>());
        detailCate1 = reader.Read<int32_t>();
        detailCate2 = reader.Read<int32_t>();
        modifiable = reader.Read<int32_t>();
        invenWidth = reader.Read<int32_t>();
        invenHeight = reader.Read<int32_t>();
        modelid = reader.Read<int32_t>();
        quickslot = reader.Read<int32_t>();
        materialAmbient = reader.Read<int32_t>();
        materialDiffuse = reader.Read<int32_t>();
        limitArticle1 = reader.Read<int32_t>();
        limitArticle2 = reader.Read<int32_t>();
        limitArticle3 = reader.Read<int32_t>();
        limitArticle4 = reader.Read<int32_t>();
        limitValue1 = reader.Read<int32_t>();
        limitValue2 = reader.Read<int32_t>();
        limitValue3 = reader.Read<int32_t>();
        limitValue4 = reader.Read<int32_t>();
        distanceDefense = reader.Read<int32_t>();
        accuracyEvasion = reader.Read<int32_t>();
        durabilityMaxcount = reader.Read<int32_t>();
        maxInvenCount = reader.Read<int32_t>();
        minimumdamage = reader.Read<int32_t>();
        maximumdamage = reader.Read<int32_t>();
        twohand = reader.Read<int32_t>();
        limitTime = reader.Read<int32_t>();
        effect1 = reader.Read<int32_t>();
        effect2 = reader.Read<int32_t>();
        effect3 = reader.Read<int32_t>();
        effect4 = reader.Read<int32_t>();
        effectvalue1 = reader.Read<float>();
        effectvalue2 = reader.Read<float>();
        effectvalue3 = reader.Read<float>();
        effectvalue4 = reader.Read<float>();
        weaponSound = reader.Read<int32_t>();
        weaponFXH = reader.Read<int32_t>();
        weaponFXM = reader.Read<int32_t>();
        weaponFXL = reader.Read<int32_t>();
        effectID1 = reader.Read<int32_t>();
        effectID2 = reader.Read<int32_t>();
        effectID3 = reader.Read<int32_t>();
        effectID4 = reader.Read<int32_t>();
        rarity = reader.Read<int32_t>();
        sound1 = reader.Read<int32_t>();
        petEP = reader.Read<int32_t>();
        petNP = reader.Read<int32_t>();
        petEXP = reader.Read<int32_t>();
    }

    void ItemtableTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "ITEMTABLE.sox");
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

    auto ItemtableTable::Find(int32_t index) const -> const Itemtable*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ItemtableTable::Get() const -> const std::vector<Itemtable>&
    {
        return _vector;
    }
}
