#pragma once
#include <boost/container/small_vector.hpp>
#include <boost/container/flat_set.hpp>
#include "sl/generator/game/time/game_time.h"
#include "sl/generator/service/database/request/skill_create.h"
#include "sl/generator/service/database/transaction/item/item_transaction.h"

namespace sunlight::db::dto
{
    struct AccountStorage;
}

namespace sunlight
{
    struct PlayerProfileIntroduction;

    class Zone;

    class GamePlayer;
    class GameClient;
}

namespace sunlight
{
    class GameRepositoryService final : public IService
    {
    public:
        explicit GameRepositoryService(Zone& zone);
        ~GameRepositoryService();

        auto GetName() const -> std::string_view override;

    public:
        void OnZoneEnter(const GamePlayer& player);
        void OnZoneLeave(const GamePlayer& player);

    public:
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
        void SaveMixSkillExp(const GamePlayer& player, int32_t skillId, int32_t skillLevel, int32_t exp);
        void SaveSkillPosition(const GamePlayer& player, int32_t skillId, int8_t page, int8_t x, int8_t y);
        void SaveNewQuest(const GamePlayer& player, int32_t questId, int32_t state, std::string flags, std::string data);
        void SaveQuestChange(const GamePlayer& player, int32_t questId, int32_t state, std::string flags, std::string data);

        void SaveHair(const GamePlayer& player, int32_t hair);
        void SaveHairColor(const GamePlayer& player, int32_t hairColor);

        void SaveProfile(const GamePlayer& player);

        void SaveStat(const GamePlayer& player, int32_t statPoint, int32_t str, int32_t dex, int32_t accr,
            int32_t health, int32_t intell, int32_t wis, int32_t will);

        void SaveState(const GamePlayer& player, int32_t zone, int32_t stage, float x, float y, float yaw);

    private:
        struct Task
        {
            const game_time_point_type created = game_clock_type::now();
            boost::container::small_vector<int64_t, 4> resources;

            std::function<Future<bool>()> operation;

            bool operator==(const Task& other) const;
            bool operator<(const Task& other) const;
        };

        struct TaskComparer
        {
            bool operator()(const SharedPtrNotNull<Task>& lhs, const SharedPtrNotNull<Task>& rhs) const;
        };

        struct Resource
        {
            const Task* owner = nullptr;
            boost::container::small_flat_set<SharedPtrNotNull<Task>, 4, TaskComparer> waiters;

            std::optional<Promise<void>> completionPromise = std::nullopt;
        };

    private:
        void Schedule(const SharedPtrNotNull<Task>& task);

    private:
        bool CanRun(const Task& task) const;
        void Run(const SharedPtrNotNull<Task>& task);
        void OnComplete(const SharedPtrNotNull<Task>& task);

    private:
        Zone& _zone;
        const ServiceLocator& _serviceLocator;

        std::unordered_map<int64_t, std::weak_ptr<GameClient>> _players;

        std::unordered_map<int64_t, Resource> _resources;
        std::vector<PtrNotNull<Resource>> _resourceBuffer;
    };
}
