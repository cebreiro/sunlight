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

        // client 0x482A50
        // client has no reference to 'SLBUDDYLIST.WND'(global asset id: 0x700030DB) and even that file is not exists.
        // player can't open 'friend' window and other player profile's 'friend add' button has no action (client 0x43715F)
        // I think it is nature that all bytes is 0.

        PacketWriter writer;

        const std::string str("unk_0x00482A50");
        const int32_t groupSize = 0;
        const int32_t buddySize = 0;
        const int32_t unk3Size = static_cast<int32_t>(std::ssize(str));

        writer.Write<int32_t>(groupSize);
        writer.Write<int32_t>(buddySize);
        writer.Write<int32_t>(unk3Size);

        for (int32_t i = 0; i < groupSize; ++i) // 72 byte 
        {
            writer.Write<int32_t>(0); // index 0~
            writer.Write<int32_t>(0); // group names start
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
            writer.Write<int32_t>(0); // v3[17] requires greater then 0 -> client 0x4C3020
        }

        for (int32_t i = 0; i < buddySize; ++i) // 40 byte
        {
            writer.Write<int32_t>(0); // index 0~
            writer.Write<int32_t>(0); // friend names start
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0);
            writer.Write<int32_t>(0); // player name end
            writer.Write<int32_t>(1); // *(v3 + 36) requires greater then 0 -> client 0x4C2E5F
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

        // client 0x484D60
        // client has no reference to 'SLPET.WND'(global asset id: 0x700030F9) and even that file is not exists.
        // so player can't open 'pet summon' window.
        // I think it is nature that all bytes is 0.

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
            writer.Write<int32_t>(skill.GetCooldown()); // client 0x4B1F70
            writer.Write<int32_t>(0); // delay? -> client set on 0x0047D0CF
            writer.Write<int32_t>(isMixSkill ? 1 : 0); // choose whether displayed on skill window or mix skill window
            writer.Write<int32_t>(1); // can use skill? -> client 0x48D200
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

        // client 0x4A9212
        // maybe base weapon motion category (I think it is 1700(other) or 5500(novice knuckle man).)
        // if client receives incorrect value then client can't see remote player's skill charge animation
        // because client basically interprets asset id as party of 'novice knuckle man'
        // e.g. magician skill charge requires COMMON_MEDICALBAG0USERMAGIC0READY04.bnx but when invalid value is given, COMMON_KNUCKLE0NOVICE0COMBO02.bnx is returned
        writer.Write<int32_t>(player.GetJobComponent().GetMainJob().GetId() == JobId::MartialArtist ? 5500 : 1700);

        writer.Write<int32_t>(0);
        writer.Write<int32_t>(0);
        writer.Write<int8_t>(0);
        writer.Write<int32_t>(0);
        writer.Write<int32_t>(0); // client get this at 'update character stat(0x487220)' but is immediately discarded, client 0x47D810
        writer.Write<int32_t>(0); // client get this at 'update character stat(0x487220)' but is immediately discarded, client 0x47D820

        int32_t v13 = 0;
        writer.Write<int32_t>(v13);
        writer.Write<int32_t>(0); // 0x4A3F98 - EAX
        writer.Write<int32_t>(player.GetAppearanceComponent().GetWeaponMotionCategory());
        writer.Write<int32_t>(0); // 0x49643D attacked monster id?
        writer.Write<int32_t>(0); // 0x4A4F50
        writer.Write<int32_t>(0); // 0x4A4F50

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

        /* a3 + 00 */writer.Write<int32_t>(static_cast<int32_t>(owner.GetId().Unwrap()));
        /* a3 + 04 */writer.Write<int32_t>(static_cast<int32_t>(owner.GetType()));
        /* a3 + 08 */writer.Write<int32_t>(statusEffect.IsHidden() ? 1 : 0);
        /* a3 + 12 */writer.Write<int32_t>(0);

        // client 0x4B443F
        /* a3 + 16 */writer.Write<int32_t>(statusEffect.GetSkillId()); // CONDITION_ACTION.sox -> index
        /* a3 + 20 */writer.Write<int32_t>(1700); // client 0x4787E0. param 1700 works
        /* a3 + 24 */writer.Write<int32_t>(static_cast<int32_t>(statusEffect.GetType()));
        /* a3 + 28 */writer.Write<int32_t>(0);
        /* a3 + 32 */writer.Write<int32_t>(0);
        /* a3 + 36 */writer.Write<int32_t>(statusEffect.GetId());
        /* a3 + 40 */writer.Write<int32_t>(statusEffect.GetStatValue());
        /* a3 + 44 */writer.Write<int32_t>(statusEffect.GetStatType());
        /* a3 + 48 */writer.Write<float>(static_cast<float>(statusEffect.GetStatPercentageValue()) / 100.f);
        /* a3 + 52 */writer.Write<int32_t>(statusEffect.GetType() == StatusEffectType::DyingResistance ? 777 : 0); // client 0x4B49F0

        return result;
    }
}
