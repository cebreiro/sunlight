#include "monster.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    Monster::Monster(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        monstername = reader.ReadString(reader.Read<uint16_t>());
        modelid = reader.Read<int32_t>();
        race = reader.Read<int32_t>();
        exp = reader.Read<int32_t>();
        level = reader.Read<int32_t>();
        str = reader.Read<int32_t>();
        dex = reader.Read<int32_t>();
        con = reader.Read<int32_t>();
        _int = reader.Read<int32_t>();
        wil = reader.Read<int32_t>();
        reserved1 = reader.Read<int32_t>();
        reserved2 = reader.Read<int32_t>();
        reserved3 = reader.Read<int32_t>();
        lifetime = reader.Read<int32_t>();
        damageMotionMinHp = reader.Read<float>();
        resist = reader.Read<int32_t>();
        abilityDamageB = reader.Read<int32_t>();
        abilityDamageC = reader.Read<int32_t>();
        abilityDamageD = reader.Read<int32_t>();
        randomAttackFactor = reader.Read<float>();
        death1delay = reader.Read<int32_t>();
        maxrarity = reader.Read<int32_t>();
        itemgenTimes = reader.Read<int32_t>();
        tresure1 = reader.Read<int32_t>();
        tresure2 = reader.Read<int32_t>();
        tresure3 = reader.Read<int32_t>();
        tresure4 = reader.Read<int32_t>();
        tresure5 = reader.Read<int32_t>();
        tresure6 = reader.Read<int32_t>();
        tresure7 = reader.Read<int32_t>();
        tresure8 = reader.Read<int32_t>();
        moneyRarityMod = reader.Read<int32_t>();
        moneyMinRarity = reader.Read<int32_t>();
        moneyFactor = reader.Read<float>();
        aiType = reader.Read<int32_t>();
        followship = reader.Read<int32_t>();
        generateType = reader.Read<int32_t>();
        rageIncRate = reader.Read<int32_t>();
        rageDecRate = reader.Read<int32_t>();
        rageAttackFactor = reader.Read<float>();
        ragespeed = reader.Read<int32_t>();
        basicspeed = reader.Read<int32_t>();
        sight = reader.Read<int32_t>();
        chase = reader.Read<int32_t>();
        canGoOut = reader.Read<int32_t>();
        domain = reader.Read<int32_t>();
        monsterarea = reader.Read<int32_t>();
        characteristic = reader.Read<int32_t>();
        activationStartTime = reader.Read<int32_t>();
        activationEndTime = reader.Read<int32_t>();
        childId = reader.Read<int32_t>();
        childSummonDelay = reader.Read<int32_t>();
        childNum = reader.Read<int32_t>();
        childSummonFrame = reader.Read<int32_t>();
        searchtime = reader.Read<int32_t>();
        move1 = reader.Read<int32_t>();
        move1range1 = reader.Read<int32_t>();
        move1range2 = reader.Read<int32_t>();
        move1delay1 = reader.Read<int32_t>();
        move1delay2 = reader.Read<int32_t>();
    }

    void MonsterTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "MONSTER.sox");
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

    auto MonsterTable::Find(int32_t index) const -> const Monster*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto MonsterTable::Get() const -> const std::vector<Monster>&
    {
        return _vector;
    }
}
