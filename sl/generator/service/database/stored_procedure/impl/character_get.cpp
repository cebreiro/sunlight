#include "character_get.h"

#include <boost/locale.hpp>

#include "sl/generator/server/server_constant.h"

namespace sunlight::db::sp
{
    CharacterGet::CharacterGet(ConnectionPool::Borrowed& conn, int64_t cid)
        : StoredProcedure(conn)
        , _cid(cid)
    {
    }

    auto CharacterGet::Release() -> dto::Character
    {
        assert(_result.has_value());

        return std::move(_result.value());
    }

    auto CharacterGet::GetSQL() const -> std::string_view
    {
        return "CALL character_get(?)";
    }

    auto CharacterGet::GetInput() const -> boost::container::small_vector<boost::mysql::field, 16>
    {
        boost::container::small_vector<boost::mysql::field, 16> inputs;
        inputs.emplace_back(_cid);

        return inputs;
    }

    void CharacterGet::SetOutput(const boost::mysql::results& result)
    {
        if (result.size() < 7)
        {
            return;
        }

        const boost::mysql::resultset_view& characterSet = result.at(0);
        const boost::mysql::resultset_view& characterStatSet = result.at(1);
        const boost::mysql::resultset_view& characterJobSet = result.at(2);
        const boost::mysql::resultset_view& characterSkillSet = result.at(3);
        const boost::mysql::resultset_view& characterItemSet = result.at(4);
        const boost::mysql::resultset_view& characterQuestSet = result.at(5);
        const boost::mysql::resultset_view& characterProfileSettingSet = result.at(6);

        if (characterSet.rows().empty())
        {
            return;
        }

        dto::Character& character = _result.emplace();

        {
            const boost::mysql::row_view& row = characterSet.rows().at(0);
            size_t index = 0;

            character.id = row.at(index++).as_int64();;
            character.aid = row.at(index++).as_int64();;

            const std::string name = row.at(index++).as_string();
            character.name = boost::locale::conv::from_utf(name, ServerConstant::TEXT_ENCODING);

            character.hair = static_cast<int32_t>(row.at(index++).as_int64());
            character.hairColor = static_cast<int32_t>(row.at(index++).as_int64());
            character.face = static_cast<int32_t>(row.at(index++).as_int64());
            character.skinColor = static_cast<int32_t>(row.at(index++).as_int64());
            character.arms = static_cast<int8_t>(row.at(index++).as_int64());
            character.running = static_cast<int8_t>(row.at(index++).as_int64());
            character.dead = static_cast<int8_t>(row.at(index++).as_int64());
            character.gold = static_cast<int32_t>(row.at(index++).as_int64());
            character.inventoryPage = static_cast<int8_t>(row.at(index++).as_int64());
            character.zone = static_cast<int32_t>(row.at(index++).as_int64());;
            character.stage = static_cast<int32_t>(row.at(index++).as_int64());;
            character.x = row.at(index++).as_float();
            character.y = row.at(index++).as_float();
            character.yaw = row.at(index++).as_float();
        }

        {
            const boost::mysql::row_view& row = characterStatSet.rows().at(0);
            size_t index = 0;

            character.gender = static_cast<int8_t>(row.at(index++).as_int64());

            boost::mysql::field_view hpField = row.at(index++);
            character.hp = hpField.is_null() ? std::optional<int32_t>() : static_cast<int32_t>(hpField.as_int64());

            boost::mysql::field_view spField = row.at(index++);
            character.sp = spField.is_null() ? std::optional<int32_t>() : static_cast<int32_t>(spField.as_int64());

            character.level = static_cast<int32_t>(row.at(index++).as_int64());
            character.exp = static_cast<int32_t>(row.at(index++).as_int64());
            character.statPoint = static_cast<int32_t>(row.at(index++).as_int64());
            character.str = static_cast<int32_t>(row.at(index++).as_int64());
            character.dex = static_cast<int32_t>(row.at(index++).as_int64());
            character.accr = static_cast<int32_t>(row.at(index++).as_int64());
            character.health = static_cast<int32_t>(row.at(index++).as_int64());
            character.intell = static_cast<int32_t>(row.at(index++).as_int64());
            character.wis = static_cast<int32_t>(row.at(index++).as_int64());
            character.will = static_cast<int32_t>(row.at(index++).as_int64());
            character.water = static_cast<int8_t>(row.at(index++).as_int64());
            character.fire = static_cast<int8_t>(row.at(index++).as_int64());
            character.lightning = static_cast<int8_t>(row.at(index++).as_int64());
        }

        character.jobs.reserve(characterJobSet.rows().size());
        for (const boost::mysql::row_view& row : characterJobSet.rows())
        {
            size_t index = 0;

            dto::Character::Job& job = character.jobs.emplace_back();

            job.id = static_cast<int32_t>(row.at(index++).as_int64());
            job.type = static_cast<int32_t>(row.at(index++).as_int64());
            job.level = static_cast<int32_t>(row.at(index++).as_int64());
            job.exp = static_cast<int32_t>(row.at(index++).as_int64());
            job.skillPoint = static_cast<int32_t>(row.at(index++).as_int64());
        }

        character.skills.reserve(characterSkillSet.rows().size());
        for (const boost::mysql::row_view& row : characterSkillSet.rows())
        {
            size_t index = 0;

            dto::Character::Skill& skill = character.skills.emplace_back();

            skill.id = static_cast<int32_t>(row.at(index++).as_int64());
            skill.job = static_cast<int32_t>(row.at(index++).as_int64());
            skill.level = static_cast<int32_t>(row.at(index++).as_int64());
            skill.cooldown = static_cast<int32_t>(row.at(index++).as_int64());
            skill.page = static_cast<int8_t>(row.at(index++).as_int64());
            skill.x = static_cast<int8_t>(row.at(index++).as_int64());
            skill.y = static_cast<int8_t>(row.at(index++).as_int64());
            skill.exp = static_cast<int32_t>(row.at(index++).as_int64());
        }

        character.items.reserve(characterItemSet.rows().size());
        for (const boost::mysql::row_view& row : characterItemSet.rows())
        {
            size_t index = 0;

            dto::Character::Item& item = character.items.emplace_back();

            item.id = row.at(index++).as_int64();
            item.dataId = static_cast<int32_t>(row.at(index++).as_int64());
            item.quantity = static_cast<int32_t>(row.at(index++).as_int64());
            item.posType = CreateItemPosTypeFrom(row.at(index++).as_string());
            item.page = static_cast<int8_t>(row.at(index++).as_int64());
            item.x = static_cast<int8_t>(row.at(index++).as_int64());
            item.y = static_cast<int8_t>(row.at(index++).as_int64());
        }

        character.quests.reserve(characterQuestSet.rows().size());
        for (const boost::mysql::row_view& row : characterQuestSet.rows())
        {
            size_t index = 0;

            dto::Character::Quest& quest = character.quests.emplace_back();

            quest.id = static_cast<int32_t>(row.at(index++).as_int64());
            quest.state = static_cast<int32_t>(row.at(index++).as_int64());
            quest.flags = row.at(index++).as_string();
            quest.data = row.at(index++).as_string();
        }

        {
            const boost::mysql::row_view& row = characterProfileSettingSet.rows().at(0);

            size_t index = 0;
            character.profileSetting.refusePartyInvite = static_cast<int8_t>(row.at(index++).as_int64());
            character.profileSetting.refuseChannelInvite = static_cast<int8_t>(row.at(index++).as_int64());
            character.profileSetting.refuseGuildInvite = static_cast<int8_t>(row.at(index++).as_int64());
            character.profileSetting.privateProfile = static_cast<int8_t>(row.at(index++).as_int64());
        }
    }
}
