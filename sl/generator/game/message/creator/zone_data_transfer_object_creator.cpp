#include "zone_data_transfer_object_creator.h"

#include "sl/generator/game/component/entity_status_effect_component.h"
#include "sl/generator/game/component/player_appearance_component.h"
#include "sl/generator/game/component/player_job_component.h"
#include "sl/generator/game/component/player_profile_component.h"
#include "sl/generator/game/component/player_skill_component.h"
#include "sl/generator/game/component/player_stat_component.h"
#include "sl/generator/game/contents/status_effect/status_effect.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/service/gamedata/skill/player_skill_data.h"

namespace sunlight
{
    auto ZoneDataTransferObjectCreator::CreatePlayerGroupState(const GamePlayer& player) -> Buffer
    {
        (void)player;

        PacketWriter writer;

        const std::string str("unk_0x00482A50");
        const int32_t groupSize = 0;
        const int32_t buddySize = 0;
        const int32_t unk3Size = static_cast<int32_t>(std::ssize(str));

        writer.Write<int32_t>(groupSize);
        writer.Write<int32_t>(buddySize);
        writer.Write<int32_t>(unk3Size);

        for (int32_t i = 0; i < groupSize; ++i)
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

        for (int32_t i = 0; i < buddySize; ++i)
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
            writer.Write<int32_t>(1);
        }

        writer.WriteBuffer(std::span(str.data(), unk3Size));
        writer.Write<int32_t>(player.GetProfileComponent().GetBitmask());

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

        writer.Write<int8_t>(0);

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
            (noviceJob ? static_cast<int32_t>(noviceJob->GetId()) : 0));
        writer.Write<int32_t>(subJob ? static_cast<int32_t>(subJob->GetId()) : 0);

        writer.Write<int32_t>(noviceJob ? static_cast<int32_t>(noviceJob->GetId()) : 0);
        writer.Write<int32_t>(advancedJob ? static_cast<int32_t>(advancedJob->GetId()) : 0);
        writer.Write<int32_t>(mixSkillJob ? static_cast<int32_t>(mixSkillJob->GetId()) : 0);
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(0);

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
            const bool isMixSkill = skill.GetData().abilityType == 5;

            writer.Write<int32_t>(static_cast<int32_t>(skill.GetJobId()));
            writer.Write<int32_t>(skill.GetId());
            writer.Write<int32_t>(skill.GetBaseLevel());
            writer.Write<int32_t>(skill.GetAdditionalLevel());
            writer.Write<int32_t>(skill.GetEXP());
            writer.Write<int32_t>(skill.GetCooldown());
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(isMixSkill ? 1 : 0);
            writer.Write<int32_t>(1);
            writer.Write<int32_t>(skill.GetPage());
            writer.Write<int32_t>(skill.GetX());
            writer.Write<int32_t>(skill.GetY());
        }

        return writer.Flush();
    }

    auto ZoneDataTransferObjectCreator::CreatePlayerStatusEffect(const GamePlayer& player) -> Buffer
    {
        const EntityStatusEffectComponent& statusEffectComponent = player.GetStatusEffectComponent();
        const int32_t count = static_cast<int32_t>(statusEffectComponent.GetCount());

        PacketWriter writer;
        writer.Write<int32_t>(count);

        if (count > 0)
        {
            for (const StatusEffect& statusEffect : statusEffectComponent.GetRange())
            {
                writer.WriteBuffer(CreateStatusEffect(player, statusEffect));
            }
        }

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

    auto ZoneDataTransferObjectCreator::CreatePlayerWeaponMotion(const GamePlayer& player) -> Buffer
    {
        PacketWriter writer;

        writer.Write<int32_t>(player.GetJobComponent().GetMainJob().GetId() == JobId::MartialArtist ? 5500 : 1700);

        writer.Write<int32_t>(0);
        writer.Write<int32_t>(0);
        writer.Write<int8_t>(0);
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(0);

        int32_t v13 = 0;
        writer.Write<int32_t>(v13);
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(player.GetAppearanceComponent().GetWeaponMotionCategory());
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(0);

        if (v13 <= 10)
        {
            for (int32_t i = 0; i < v13; ++i)
            {
                writer.Write<int32_t>(0);
            }
        }

        return writer.Flush();
    }

    auto ZoneDataTransferObjectCreator::CreateStatusEffect(const GameEntity& owner, const StatusEffect& statusEffect) -> boost::container::static_vector<char, 56>
    {
        boost::container::static_vector<char, 56> result = {};
        StreamWriter writer(result);

       writer.Write<int32_t>(static_cast<int32_t>(owner.GetId().Unwrap()));
       writer.Write<int32_t>(static_cast<int32_t>(owner.GetType()));
       writer.Write<int32_t>(statusEffect.IsHidden() ? 1 : 0);
       writer.Write<int32_t>(0);

       writer.Write<int32_t>(statusEffect.GetSkillId());
       writer.Write<int32_t>(1700);
       writer.Write<int32_t>(static_cast<int32_t>(statusEffect.GetType()));
       writer.Write<int32_t>(0);
       writer.Write<int32_t>(0);
       writer.Write<int32_t>(statusEffect.GetId());
       writer.Write<int32_t>(statusEffect.GetStatValue());
       writer.Write<int32_t>(statusEffect.GetStatType());
       writer.Write<float>(static_cast<float>(statusEffect.GetStatPercentageValue()) / 100.f);
       writer.Write<int32_t>(statusEffect.GetType() == StatusEffectType::DyingResistance ? 777 : 0);

        return result;
    }
}
