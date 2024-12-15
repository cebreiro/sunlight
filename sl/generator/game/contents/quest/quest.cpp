#include "quest.h"

#include <boost/lexical_cast.hpp>

#include "sl/generator/game/time/game_time_service.h"

namespace sunlight
{
    Quest::Quest()
    {
    }

    Quest::Quest(int32_t id)
        : _id(id)
    {
    }

    bool Quest::IsExpired() const
    {
        if (_timeLimit.has_value())
        {
            const int64_t count = GameTimeService::Now().time_since_epoch().count();

            return count > _timeLimit->endTimePoint;
        }

        return true;
    }

    bool Quest::HasFlag(int32_t index) const
    {
        return _flags.contains(index);
    }

    bool Quest::HasTimeLimit() const
    {
        return _timeLimit.has_value();
    }

    bool Quest::HasFlagValue(int32_t value) const
    {
        for (int32_t flagValue : _flags | std::views::values)
        {
            if (flagValue == value)
            {
                return true;
            }
        }

        return false;
    }

    auto Quest::GetId() const -> int32_t
    {
        return _id;
    }

    auto Quest::GetState() const -> int32_t
    {
        return _state;
    }

    auto Quest::GetFlags() const -> const std::map<int32_t, int32_t>&
    {
        return _flags;
    }

    auto Quest::GetFlag(int32_t index) const -> int32_t
    {
        const auto iter = _flags.find(index);
        if (iter == _flags.end())
        {
            return -1;
        }

        return iter->second;
    }

    auto Quest::GetTimeLimit() const -> const QuestTimeLimit&
    {
        assert(_timeLimit);

        return *_timeLimit;
    }

    void Quest::SetState(int32_t state)
    {
        _state = state;
    }

    void Quest::SetFlag(int32_t index, int32_t value)
    {
        _flags[index] = value;
    }

    void Quest::SetTimeLimit(const std::optional<QuestTimeLimit>& limit)
    {
        _timeLimit = limit;
    }

    auto Quest::GetFlagString() const -> std::string
    {
        nlohmann::json j;

        for (const auto [key, value] : _flags)
        {
            j[std::to_string(key)] = value;
        }

        return j.dump();
    }

    auto Quest::GetData() const -> std::string
    {
        nlohmann::json j;

        if (_timeLimit.has_value())
        {
            to_json(j[QuestTimeLimit::NAME], *_timeLimit);
        }

        return j.dump();
    }

    auto Quest::CreateFrom(int32_t id, int32_t state, const std::string& flags, const std::string& data)
        -> std::optional<Quest>
    {
        try
        {
            Quest result;
            result._id = id;
            result._state = state;

            if (!flags.empty())
            {
                std::map<std::string, int32_t> temp;
                from_json(nlohmann::json::parse(flags), temp);

                for (const auto& [key, value] : temp)
                {
                    result._flags[boost::lexical_cast<int32_t>(key)] = value;
                }
            }

            if (!data.empty())
            {
                nlohmann::json json = nlohmann::json::parse(data);

                if (const auto iter = json.find(QuestTimeLimit::NAME); iter != json.end())
                {
                    from_json(*iter, result._timeLimit.emplace());
                }
            }

            return result;
        }
        catch (...)
        {
        }

        return std::nullopt;
    }
}
