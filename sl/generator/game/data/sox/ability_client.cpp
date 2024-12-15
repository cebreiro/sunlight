#include "ability_client.h"

#include "sl/data/sox/sox_file.h"

namespace sunlight::sox
{
    AbilityClient::AbilityClient(StreamReader<std::vector<char>::const_iterator>& reader)
    {
        index = reader.Read<int32_t>();
        abilityName = reader.ReadString(reader.Read<uint16_t>());
        abilityRoutine = reader.Read<int32_t>();
        totalTime = reader.Read<int32_t>();
        _1Hand00Pid = reader.Read<int32_t>();
        _2Hand00Pid = reader.Read<int32_t>();
        spearPid = reader.Read<int32_t>();
        polearmPid = reader.Read<int32_t>();
        glovePid = reader.Read<int32_t>();
        bowPid = reader.Read<int32_t>();
        handgunPid = reader.Read<int32_t>();
        doublegunPid = reader.Read<int32_t>();
        shotgunPid = reader.Read<int32_t>();
        throwPid = reader.Read<int32_t>();
        _1Hand00StartDamageFrame = reader.Read<int32_t>();
        _2Hand00StartDamageFrame = reader.Read<int32_t>();
        spearStartDamageFrame = reader.Read<int32_t>();
        polearmDamageFrame = reader.Read<int32_t>();
        gloveStartDamageFrame = reader.Read<int32_t>();
        bowStartDamageFrame = reader.Read<int32_t>();
        handgunStartDamageFrame = reader.Read<int32_t>();
        doublegunStartDamageFrame = reader.Read<int32_t>();
        shotgunStartDamageFrame = reader.Read<int32_t>();
        throwStartDamageFrame = reader.Read<int32_t>();
        _1Hand00Timing = reader.Read<int32_t>();
        _2Hand00Timing = reader.Read<int32_t>();
        spearTiming = reader.Read<int32_t>();
        polearmTiming = reader.Read<int32_t>();
        gloveTiming = reader.Read<int32_t>();
        bowTiming = reader.Read<int32_t>();
        handgunTiming = reader.Read<int32_t>();
        doublegunTiming = reader.Read<int32_t>();
        shotgunTiming = reader.Read<int32_t>();
        throwTiming = reader.Read<int32_t>();
        _1Hand00FullTime = reader.Read<int32_t>();
        _2Hand00FullTime = reader.Read<int32_t>();
        spearFullTime = reader.Read<int32_t>();
        polearmFullTime = reader.Read<int32_t>();
        gloveFullTime = reader.Read<int32_t>();
        bowFullTime = reader.Read<int32_t>();
        handgunFullTime = reader.Read<int32_t>();
        doublegunFullTime = reader.Read<int32_t>();
        shotgunFullTime = reader.Read<int32_t>();
        throwFullTime = reader.Read<int32_t>();
        isUniqueItemAbility = reader.Read<int32_t>();
        uniqueItemAbilityPid = reader.Read<int32_t>();
        chargeFXGid = reader.Read<int32_t>();
        chargeFXPid = reader.Read<int32_t>();
        defaultShockFX = reader.Read<int32_t>();
        shockFXGid = reader.Read<int32_t>();
        shockFXPid = reader.Read<int32_t>();
        abilityDelay = reader.Read<int32_t>();
        locationTab = reader.Read<int32_t>();
        locationX = reader.Read<int32_t>();
        locationY = reader.Read<int32_t>();
        detail = reader.ReadString(reader.Read<uint16_t>());
        iconTextureId = reader.Read<int32_t>();
        iconLocation = reader.Read<int32_t>();
        iconTexture2Id = reader.Read<int32_t>();
        iconLocation2 = reader.Read<int32_t>();
        reserved1 = reader.Read<int32_t>();
        reserved2 = reader.Read<int32_t>();
        dmgPenalty = reader.Read<float>();
        category = reader.Read<int32_t>();
        cancelable = reader.Read<int32_t>();
        five = reader.Read<int32_t>();
        spendSP = reader.Read<int32_t>();
        length = reader.Read<int32_t>();
        keepTime = reader.Read<int32_t>();
        damageLength = reader.Read<int32_t>();
        usage = reader.Read<int32_t>();
        applyTarget = reader.Read<int32_t>();
        addWeaponRange = reader.Read<int32_t>();
        damageStyle = reader.Read<int32_t>();
        damageReserved1 = reader.Read<int32_t>();
        damageReserved2 = reader.Read<int32_t>();
        damageMotionType = reader.Read<int32_t>();
        sideEffect1 = reader.Read<int32_t>();
        sideEffect2 = reader.Read<int32_t>();
        statusValType1 = reader.Read<int32_t>();
        statusValType2 = reader.Read<int32_t>();
        statusProb = reader.Read<int32_t>();
        statusReserved11 = reader.Read<int32_t>();
        statusReserved12 = reader.Read<int32_t>();
        statusReserved21 = reader.Read<int32_t>();
        statusReserved22 = reader.Read<int32_t>();
        job = reader.Read<int32_t>();
        exp = reader.Read<int32_t>();
        speed = reader.Read<float>();
        requireItem = reader.Read<int32_t>();
        spendItem = reader.Read<int32_t>();
        spendBullet = reader.Read<int32_t>();
        increaseHit = reader.Read<int32_t>();
        needWeapon = reader.Read<int32_t>();
    }

    void AbilityClientTable::LoadFromFile(const std::filesystem::path& path)
    {
        SoxFile fileData = SoxFile::CreateFrom(path / "ABILITY_CLIENT.sox");
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

    auto AbilityClientTable::Find(int32_t index) const -> const AbilityClient*
    {
        auto iter = _umap.find(index);

        return iter != _umap.end() ? iter->second : nullptr;
    }

    auto AbilityClientTable::Get() const -> const std::vector<AbilityClient>&
    {
        return _vector;
    }
}
