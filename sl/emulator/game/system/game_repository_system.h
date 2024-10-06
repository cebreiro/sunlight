#pragma once
#include "sl/emulator/game/system/game_system.h"
#include "sl/emulator/service/database/request/skill_create.h"
#include "sl/emulator/service/database/transaction/item/item_transaction.h"

namespace sunlight::db::dto
{
    struct AccountStorage;
}

namespace sunlight
{
    class Quest;
    struct PlayerProfileIntroduction;

    class GameClient;
    class GamePlayer;
}

namespace sunlight
{
    class GameRepositorySystem final : public GameSystem
    {
    public:
        explicit GameRepositorySystem(const ServiceLocator& serviceLocator);

        void InitializeSubSystem(Stage& stage) override;
        bool Subscribe(Stage& stage) override;
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

        // TODO: Join

    public:
        bool IsPending() const;
        bool IsPendingSaves(const GamePlayer& player) const;

        auto WaitForSaveCompletion(const GamePlayer& player) -> Future<void>;

        void LoadAccountStorage(const GamePlayer& player, const std::function<void(const db::dto::AccountStorage&)>& callback);
        void LoadProfileIntroduction(const GamePlayer& player, const std::function<void(PlayerProfileIntroduction&)>& callback);

        void Save(const GamePlayer& player, db::ItemTransaction transaction);
        void Save(const GamePlayer& host, const GamePlayer& guest, db::ItemTransaction transaction);

        void SaveCharacterExp(const GamePlayer& player, int32_t exp);
        void SaveCharacterLevel(const GamePlayer& player, int32_t level, int32_t statPoint);
        void SaveJobExp(const GamePlayer& player, int32_t job, int32_t exp);
        void SaveJobLevel(const GamePlayer& player, int32_t job, int32_t level, int32_t skillPoint, std::vector<req::SkillCreate> skills);
        void SaveNewJob(const GamePlayer& player, int32_t job, int32_t jobType, int32_t level, int32_t skillPoint, std::vector<req::SkillCreate> skills);
        void SaveNewSkill(const GamePlayer& player, int32_t job, int32_t skillId, int32_t skillLevel);
        void SaveSkillLevel(const GamePlayer& player, int32_t job, int32_t skillPoint, int32_t skillId, int32_t skillLevel);
        void SaveNewQuest(const GamePlayer& player, int32_t questId, int32_t state, std::string flags, std::string data);
        void SaveQuestChange(const GamePlayer& player, int32_t questId, int32_t state, std::string flags, std::string data);

        void SaveHair(const GamePlayer& player, int32_t hair);
        void SaveHairColor(const GamePlayer& player, int32_t hairColor);

        void SaveProfile(const GamePlayer& player);

        void SaveStat(const GamePlayer& player, int32_t statPoint, int32_t str, int32_t dex, int32_t accr,
            int32_t health, int32_t intell, int32_t wis, int32_t will);

        void SaveState(const GamePlayer& player, int32_t zone, int32_t stage, float x, float y, float yaw);

    private:
        void OnComplete(int64_t cid);
        void OnError(int64_t cid);

    private:
        const ServiceLocator& _serviceLocator;

        std::unordered_map<int64_t, std::pair<int32_t, WeakPtrNotNull<GameClient>>> _pending;
        std::unordered_map<int64_t, Promise<void>> _saveCompletionPromise;
    };
}
