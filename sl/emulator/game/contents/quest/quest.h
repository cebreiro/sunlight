#pragma once
#include "sl/emulator/game/contents/quest/quest_item_gain.h"
#include "sl/emulator/game/contents/quest/quest_time_limit.h"

namespace sunlight
{
    class Quest
    {
    public:
        Quest();
        explicit Quest(int32_t id);

        bool IsExpired() const;
        bool HasFlag(int32_t index) const;
        bool HasTimeLimit() const;
        bool HasItemGain() const;

        void ConfigureItemGain(int32_t monsterId, int32_t itemId, int32_t probability, int32_t maxItemQuantity, int32_t minKillCount);

        auto GetId() const -> int32_t;

        // func_1202
        auto GetState() const -> int32_t;
        auto GetFlags() const -> const std::map<int32_t, int32_t>&;

        // func_1204
        auto GetFlag(int32_t index) const -> int32_t;
        auto GetTimeLimit() const -> const QuestTimeLimit&;
        auto GetItemGain() -> QuestItemGain&;
        auto GetItemGain() const -> const QuestItemGain&;

        // func_1203
        void SetState(int32_t state);

        // func_1205
        void SetFlag(int32_t index, int32_t value);
        void SetTimeLimit(const std::optional<QuestTimeLimit>& limit);
        void SetItemGain(const std::optional<QuestItemGain>& itemGain);

        auto GetFlagString() const -> std::string;
        auto GetData() const -> std::string;

        static auto CreateFrom(int32_t id, int32_t state, const std::string& flags, const std::string& data)
            -> std::optional<Quest>;

    private:
        int32_t _id = 0;
        int32_t _state = 0;
        std::map<int32_t, int32_t> _flags;

        std::optional<QuestTimeLimit> _timeLimit;
        std::optional<QuestItemGain> _itemGain;
    };
}
