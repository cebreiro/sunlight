#include "zone_data_transfer_object_creator.h"

#include "sl/emulator/game/component/player_appearance_component.h"
#include "sl/emulator/game/component/player_job_component.h"
#include "sl/emulator/game/component/player_skill_component.h"
#include "sl/emulator/game/component/player_stat_component.h"
#include "sl/emulator/game/entity/game_player.h"

namespace sunlight
{
    auto ZoneDataTransferObjectCreator::CreatePlayerUnkState(const GamePlayer& player) -> Buffer
    {
        (void)player;

        // 0x00482A50

        PacketWriter writer;

        const std::string str("unk_0x00482A50");
        const int32_t unk1Size = 0;
        const int32_t unk2Size = 0;
        const int32_t unk3Size = static_cast<int32_t>(std::ssize(str));

        writer.Write<int32_t>(unk1Size);
        writer.Write<int32_t>(unk2Size);
        writer.Write<int32_t>(unk3Size);

        for (int32_t i = 0; i < unk1Size; ++i) // 72 byte 
        {
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
        }

        for (int32_t i = 0; i < unk2Size; ++i) // 40 byte
        {
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
        }

        writer.WriteBuffer(std::span(str.data(), unk3Size));
        writer.Write<int32_t>(0); // unk

        return writer.Flush();
    }

    auto ZoneDataTransferObjectCreator::CreatePlayerAppearance(const GamePlayer& player) -> Buffer
    {
        const PlayerAppearanceComponent& appearanceComponent = player.GetAppearanceComponent();

        PacketWriter writer;

        const int32_t hatModel = appearanceComponent.GetHatModelId();
        const int32_t hairModel = appearanceComponent.GetHair();

        writer.Write<int32_t>(hatModel != 0 ? hairModel : 0);
        writer.Write<int32_t>(appearanceComponent.GetHairColor());
        writer.Write<int32_t>(appearanceComponent.GetSkinColor());

        writer.Write<int32_t>(appearanceComponent.GetFace());
        writer.Write<int32_t>(hatModel != 0 ? hatModel : hairModel);
        writer.Write<int32_t>(appearanceComponent.GetJacketModelId());
        writer.Write<int32_t>(appearanceComponent.GetGlovesModelId());
        writer.Write<int32_t>(appearanceComponent.GetPantsModelId());
        writer.Write<int32_t>(appearanceComponent.GetShoesModelId());
        writer.Write<int32_t>(appearanceComponent.GetWeaponModelId());

        writer.Write<int32_t>(0);
        writer.Write<int32_t>(appearanceComponent.GetHatModelColor());
        writer.Write<int32_t>(appearanceComponent.GetJacketModelColor());
        writer.Write<int32_t>(appearanceComponent.GetGlovesModelColor());
        writer.Write<int32_t>(appearanceComponent.GetPantsModelColor());
        writer.Write<int32_t>(appearanceComponent.GetShoesModelColor());
        writer.Write<int32_t>(appearanceComponent.GetWeaponModelColor());

        writer.Write<int8_t>(0); // 0x48411D

        return writer.Flush();
    }

    auto ZoneDataTransferObjectCreator::CreatePlayerInformation(const GamePlayer& player) -> Buffer
    {
        const PlayerStatComponent& statComponent = player.GetStatComponent();
        const PlayerJobComponent& jobComponent = player.GetJobComponent();

        PacketWriter writer;

        {
            std::array<char, 32> name = {};
            const int64_t count = std::min(std::ssize(player.GetName()), std::ssize(name));

            std::copy_n(player.GetName().begin(), count, name.begin());

            writer.WriteBuffer(name);
        }

        writer.WriteZeroBytes(32);

        {
            std::array<char, 32> zone = {};
            const std::string str = std::to_string(player.GetZoneId());

            const int64_t count = std::min(std::ssize(str), std::ssize(zone));

            std::copy_n(str.begin(), count, zone.begin());

            writer.WriteBuffer(zone);
        }

        writer.WriteZeroBytes(32);

        const Job* noviceJob = jobComponent.Find(JobType::Novice);
        const Job* advancedJob = jobComponent.Find(JobType::Advanced);
        const Job* mixSkillJob = jobComponent.Find(JobType::MixSkill);
        const Job* subJob = jobComponent.Find(JobType::Sub);

        writer.Write<int32_t>(noviceJob ? noviceJob->GetSkillPoint() : 0);
        writer.Write<int32_t>(advancedJob ? advancedJob->GetSkillPoint() : 0);
        writer.Write<int32_t>(mixSkillJob ? mixSkillJob->GetSkillPoint() : 0);
        writer.Write<int32_t>(subJob ? subJob->GetSkillPoint() : 0);
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(statComponent.GetStatPoint());
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(advancedJob ? static_cast<int32_t>(advancedJob->GetId())
            : (noviceJob ? static_cast<int32_t>(noviceJob->GetId()) : 0));

        writer.Write<int8_t>(player.IsArmed());
        writer.Write<int32_t>(0);
        writer.Write<int8_t>(0);
        writer.Write<int32_t>(player.GetGmLevel());
        writer.Write<int8_t>(0);
        writer.Write<int8_t>(0);
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(advancedJob ? static_cast<int32_t>(advancedJob->GetId()) :
            (noviceJob ? static_cast<int32_t>(noviceJob->GetId()) : 0)); // profile job *(_DWORD *)(a2 + 176);
        writer.Write<int32_t>(subJob ? static_cast<int32_t>(subJob->GetId()) : 0);

        // skill window (A button)
        //   -> novice advanced
        //   -> sub    mix_skill
        // qmemcpy(v2 + 332, (const void *)(a2 + 184), 20u);
        writer.Write<int32_t>(noviceJob ? static_cast<int32_t>(noviceJob->GetId()) : 0);
        writer.Write<int32_t>(advancedJob ? static_cast<int32_t>(advancedJob->GetId()) : 0);
        writer.Write<int32_t>(mixSkillJob ? static_cast<int32_t>(mixSkillJob->GetId()) : 0);
        writer.Write<int32_t>(0); // not accessed
        writer.Write<int32_t>(0); // not accessed

        // qmemcpy(v2 + 352, (const void *)(a2 + 204), 20u);
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(subJob ? static_cast<int32_t>(subJob->GetId()) : 0);
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(0);

        writer.Write<int32_t>(0);
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(statComponent.GetExp());

        // qmemcpy(v2 + 396, (const void *)(a2 + 248), 60u);
        writer.Write<int32_t>(statComponent.GetGender());
        writer.Write<int32_t>(statComponent.GetLevel());
        writer.Write<int32_t>(statComponent.GetFinalStat(RecoveryStatType::HP).As<int32_t>());
        writer.Write<int32_t>(statComponent.GetFinalStat(RecoveryStatType::SP).As<int32_t>());
        writer.Write<int32_t>(statComponent.Get(PlayerStatType::Str).Get(StatOriginType::Base).As<int32_t>());
        writer.Write<int32_t>(statComponent.Get(PlayerStatType::Dex).Get(StatOriginType::Base).As<int32_t>());
        writer.Write<int32_t>(statComponent.Get(PlayerStatType::Accr).Get(StatOriginType::Base).As<int32_t>());
        writer.Write<int32_t>(statComponent.Get(PlayerStatType::Health).Get(StatOriginType::Base).As<int32_t>());
        writer.Write<int32_t>(statComponent.Get(PlayerStatType::Intell).Get(StatOriginType::Base).As<int32_t>());
        writer.Write<int32_t>(statComponent.Get(PlayerStatType::Wisdom).Get(StatOriginType::Base).As<int32_t>());
        writer.Write<int32_t>(statComponent.Get(PlayerStatType::Will).Get(StatOriginType::Base).As<int32_t>());
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(statComponent.Get(PlayerStatType::Fire).Get(StatOriginType::Base).As<int32_t>());
        writer.Write<int32_t>(statComponent.Get(PlayerStatType::Water).Get(StatOriginType::Base).As<int32_t>());
        writer.Write<int32_t>(statComponent.Get(PlayerStatType::Lightning).Get(StatOriginType::Base).As<int32_t>());

        writer.Write<int8_t>(!player.IsRunning());
        writer.WriteZeroBytes(512);

        std::vector jobs{ noviceJob, advancedJob, mixSkillJob, subJob };
        const int32_t jobCount = static_cast<int32_t>(std::ranges::count_if(jobs, [](const auto& job)
            {
                return job != nullptr;
            }));

        writer.Write<int32_t>(jobCount);
        writer.Write<int32_t>(jobCount * 12);

        for (int32_t i = 0; i < jobCount; ++i)
        {
            writer.Write<int32_t>(12);
        }

        for (int32_t i = 0; i < jobCount; ++i)
        {
            const Job* job = jobs[i];
            if (!job)
            {
                continue;
            }

            writer.Write<int32_t>(static_cast<int32_t>(job->GetId()));
            writer.Write<int32_t>(job->GetLevel());
            writer.Write<int32_t>(job->GetExp());
        }

        return writer.Flush();
    }

    auto ZoneDataTransferObjectCreator::CreatePlayerPet(const GamePlayer& player) -> Buffer
    {
        (void)player;

        // 0x00484D60

        PacketWriter writer;
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(0);

        return writer.Flush();
    }

    auto ZoneDataTransferObjectCreator::CreatePlayerSkill(const GamePlayer& player) -> Buffer
    {
        const PlayerSkillComponent& skillComponent = player.GetSkillComponent();

        PacketWriter writer;
        writer.Write<int32_t>(static_cast<int32_t>(skillComponent.GetSkillCount()));

        for (const PlayerSkill& skill : skillComponent.GetSkills())
        {
            writer.Write<int32_t>(static_cast<int32_t>(skill.GetJobId()));
            writer.Write<int32_t>(skill.GetId());
            writer.Write<int32_t>(skill.GetBaseLevel());
            writer.Write<int32_t>(skill.GetAdditionalLevel());
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(skill.GetCooldown());
            writer.Write<int32_t>(0); // delay? -> client set on 0x0047D0CF
            writer.Write<int32_t>(0); // hidden? -> client cmp 1 on 00422E76
            writer.Write<int32_t>(0); // can use skill? -> client 0x48D200
            writer.Write<int32_t>(skill.GetPage());
            writer.Write<int32_t>(skill.GetX());
            writer.Write<int32_t>(skill.GetY());
        }

        return writer.Flush();
    }

    auto ZoneDataTransferObjectCreator::CreatePlayerStatusEffect(const GamePlayer& player) -> Buffer
    {
        (void)player;

        PacketWriter writer;

        constexpr int32_t statusEffectCount = 0;
        writer.Write<int32_t>(statusEffectCount);

        return writer.Flush();
    }

    auto ZoneDataTransferObjectCreator::CreatePlayerStat(const GamePlayer& player) -> Buffer
    {
        const PlayerStatComponent& statComponent = player.GetStatComponent();
        const Job& mainJob = player.GetJobComponent().GetMainJob();

        PacketWriter writer;
        writer.Write<int16_t>(statComponent.GetFinalStat(RecoveryStatType::HP).As<int16_t>());
        writer.Write<int16_t>(statComponent.GetFinalStat(RecoveryStatType::SP).As<int16_t>());
        writer.Write<int16_t>(statComponent.Get(PlayerStatType::Dex).Get(StatOriginType::Base).As<int16_t>());
        writer.Write<int16_t>(statComponent.Get(PlayerStatType::Health).Get(StatOriginType::Base).As<int16_t>());
        writer.Write<int16_t>(statComponent.Get(PlayerStatType::Will).Get(StatOriginType::Base).As<int16_t>());
        writer.Write<int16_t>(static_cast<int16_t>(statComponent.GetLevel()));
        writer.Write<int16_t>(statComponent.Get(PlayerStatType::Intell).Get(StatOriginType::Base).As<int16_t>());
        writer.Write<int16_t>(statComponent.Get(PlayerStatType::Wisdom).Get(StatOriginType::Base).As<int16_t>());
        writer.Write<int16_t>(statComponent.Get(PlayerStatType::Accr).Get(StatOriginType::Base).As<int16_t>());
        writer.Write<int16_t>(statComponent.Get(PlayerStatType::Str).Get(StatOriginType::Base).As<int16_t>());
        writer.Write<int16_t>(0); // add max hp_1
        writer.Write<int16_t>(0); // add max hp_2
        writer.Write<int16_t>(static_cast<int16_t>(mainJob.GetId()));
        writer.Write<int16_t>(static_cast<int16_t>(mainJob.GetLevel()));
        writer.Write<int32_t>(statComponent.GetStatPoint());
        writer.Write<int32_t>(0); // anger ? 1 : 0

        return writer.Flush();
    }
}
