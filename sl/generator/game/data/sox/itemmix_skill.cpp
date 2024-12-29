#include "itemmix_skill.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    ItemmixSkill::ItemmixSkill(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        iD = reader.Read<int32_t>();
        name = reader.ReadString(reader.Read<uint16_t>());
        type = reader.Read<int32_t>();
        difficultyType = reader.Read<int32_t>();
        requiredJobID = reader.Read<int32_t>();
        requiredJobLevel = reader.Read<int32_t>();
        requiredSkillID = reader.Read<int32_t>();
        requiredSkillLevel = reader.Read<int32_t>();
        mixGroupID1 = reader.Read<int32_t>();
        mixGroupID1Level = reader.Read<int32_t>();
        mixGroupID2 = reader.Read<int32_t>();
        mixGroupID2Level = reader.Read<int32_t>();
        mixGroupID3 = reader.Read<int32_t>();
        mixGroupID3Level = reader.Read<int32_t>();
        mixGroupID4 = reader.Read<int32_t>();
        mixGroupID4Level = reader.Read<int32_t>();
        mixGroupID5 = reader.Read<int32_t>();
        mixGroupID5Level = reader.Read<int32_t>();
        mixGroupID6 = reader.Read<int32_t>();
        mixGroupID6Level = reader.Read<int32_t>();
        skillDescription = reader.ReadString(reader.Read<uint16_t>());
        maxDepth = reader.Read<int32_t>();
        maxDepthWide1 = reader.Read<int32_t>();
        depthName11 = reader.ReadString(reader.Read<uint16_t>());
        depthID11 = reader.Read<int32_t>();
        categoryDes11 = reader.ReadString(reader.Read<uint16_t>());
        depthTxID11 = reader.Read<int32_t>();
        depthTxNum11 = reader.Read<int32_t>();
        depthName12 = reader.ReadString(reader.Read<uint16_t>());
        depthID12 = reader.Read<int32_t>();
        categoryDes12 = reader.ReadString(reader.Read<uint16_t>());
        depthTxID12 = reader.Read<int32_t>();
        depthTxNum12 = reader.Read<int32_t>();
        depthName13 = reader.ReadString(reader.Read<uint16_t>());
        depthID13 = reader.Read<int32_t>();
        categoryDes13 = reader.ReadString(reader.Read<uint16_t>());
        depthTxID13 = reader.Read<int32_t>();
        depthTxNum13 = reader.Read<int32_t>();
        depthName14 = reader.ReadString(reader.Read<uint16_t>());
        depthID14 = reader.Read<int32_t>();
        categoryDes14 = reader.ReadString(reader.Read<uint16_t>());
        depthTxID14 = reader.Read<int32_t>();
        depthTxNum14 = reader.Read<int32_t>();
        depthName15 = reader.ReadString(reader.Read<uint16_t>());
        depthID15 = reader.Read<int32_t>();
        categoryDes15 = reader.ReadString(reader.Read<uint16_t>());
        depthTxID15 = reader.Read<int32_t>();
        depthTxNum15 = reader.Read<int32_t>();
        depthName16 = reader.ReadString(reader.Read<uint16_t>());
        depthID16 = reader.Read<int32_t>();
        categoryDes16 = reader.ReadString(reader.Read<uint16_t>());
        depthTxID16 = reader.Read<int32_t>();
        depthTxNum16 = reader.Read<int32_t>();
        maxDepthWide2 = reader.Read<int32_t>();
        depthName21 = reader.ReadString(reader.Read<uint16_t>());
        depthID21 = reader.Read<int32_t>();
        categoryDes21 = reader.ReadString(reader.Read<uint16_t>());
        depthTxID21 = reader.Read<int32_t>();
        depthTxNum21 = reader.Read<int32_t>();
        depthName22 = reader.ReadString(reader.Read<uint16_t>());
        depthID22 = reader.Read<int32_t>();
        categoryDes22 = reader.ReadString(reader.Read<uint16_t>());
        depthTxID22 = reader.Read<int32_t>();
        depthTxNum22 = reader.Read<int32_t>();
        depthName23 = reader.ReadString(reader.Read<uint16_t>());
        depthID23 = reader.Read<int32_t>();
        categoryDes23 = reader.ReadString(reader.Read<uint16_t>());
        depthTxID23 = reader.Read<int32_t>();
        depthTxNum23 = reader.Read<int32_t>();
        depthName24 = reader.ReadString(reader.Read<uint16_t>());
        depthID24 = reader.Read<int32_t>();
        categoryDes24 = reader.ReadString(reader.Read<uint16_t>());
        depthTxID24 = reader.Read<int32_t>();
        depthTxNum24 = reader.Read<int32_t>();
        maxDepthWide3 = reader.Read<int32_t>();
        depthName31 = reader.ReadString(reader.Read<uint16_t>());
        depthID31 = reader.Read<int32_t>();
        categoryDes31 = reader.ReadString(reader.Read<uint16_t>());
        depthTxID31 = reader.Read<int32_t>();
        depthTxNum31 = reader.Read<int32_t>();
        depthName32 = reader.ReadString(reader.Read<uint16_t>());
        depthID32 = reader.Read<int32_t>();
        categoryDes32 = reader.ReadString(reader.Read<uint16_t>());
        depthTxID32 = reader.Read<int32_t>();
        depthTxNum32 = reader.Read<int32_t>();
        depthName33 = reader.ReadString(reader.Read<uint16_t>());
        depthID33 = reader.Read<int32_t>();
        categoryDes33 = reader.ReadString(reader.Read<uint16_t>());
        depthTxID33 = reader.Read<int32_t>();
        depthTxNum33 = reader.Read<int32_t>();
        depthName34 = reader.ReadString(reader.Read<uint16_t>());
        depthID34 = reader.Read<int32_t>();
        categoryDes34 = reader.ReadString(reader.Read<uint16_t>());
        depthTxID34 = reader.Read<int32_t>();
        depthTxNum34 = reader.Read<int32_t>();
    }

    void ItemmixSkillTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "ITEMMIX_SKILL.sox");
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

    auto ItemmixSkillTable::Find(int32_t index) const -> const ItemmixSkill*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto ItemmixSkillTable::Get() const -> const std::vector<ItemmixSkill>&
    {
        return _vector;
    }
}
