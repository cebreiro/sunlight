#include "game_repository_service.h"

#include <boost/scope/scope_exit.hpp>

#include "sl/generator/game/component/player_profile_component.h"
#include "sl/generator/game/component/player_stat_component.h"
#include "sl/generator/game/entity/game_player.h"
#include "sl/generator/game/zone/zone.h"
#include "sl/generator/game/zone/service/zone_execution_service.h"
#include "sl/generator/server/client/game_client.h"
#include "sl/generator/service/database/database_service.h"

namespace sunlight
{
    GameRepositoryService::GameRepositoryService(Zone& zone)
        : _zone(zone)
        , _serviceLocator(_zone.GetServiceLocator())
    {
    }

    GameRepositoryService::~GameRepositoryService()
    {
    }

    auto GameRepositoryService::GetName() const -> std::string_view
    {
        return "game_repository_service";
    }

    void GameRepositoryService::OnZoneEnter(const GamePlayer& player)
    {
        assert(!_players.contains(player.GetCId()));

        _players[player.GetCId()] = player.GetClient().weak_from_this();
    }

    void GameRepositoryService::OnZoneLeave(const GamePlayer& player)
    {
        [[maybe_unused]]
        const bool erased = _players.erase(player.GetCId());
        assert(erased);
    }

    bool GameRepositoryService::IsPendingSaves(const GamePlayer& player) const
    {
        return _resources.contains(player.GetCId());
    }

    auto GameRepositoryService::WaitForSaveCompletion(const GamePlayer& player) -> Future<void>
    {
        const auto iter = _resources.find(player.GetCId());
        if (iter == _resources.end())
        {
            co_return;
        }

        Resource& resource = iter->second;
        assert(!resource.completionPromise.has_value());

        Promise<void> promise;
        Future<void> future = promise.GetFuture();

        resource.completionPromise = std::move(promise);

        co_await future;

        co_return;
    }

    void GameRepositoryService::LoadAccountStorage(const GamePlayer& player, const std::function<void(const db::dto::AccountStorage&)>& callback)
    {
        auto task = std::make_shared<Task>();
        task->resources.emplace_back(player.GetCId());
        task->operation = [this, aid = player.GetAId(), cid = player.GetCId(), callback]()
            {
                Promise<bool> promise;
                Future<bool> future = promise.GetFuture();

                _serviceLocator.Get<DatabaseService>().GetAccountStorage(aid)
                    .Then(_zone.GetStrand(), [weak = _zone.weak_from_this(), this, cid, callback, p = std::move(promise)](const std::optional<db::dto::AccountStorage>& result) mutable
                        {
                            boost::scope::scope_exit exit([&p, result = result.has_value()]()
                                {
                                    p.Set(result);
                                });

                            const std::shared_ptr<Zone> zone = weak.lock();
                            if (!zone)
                            {
                                return;
                            }

                            if (result.has_value())
                            {
                                _serviceLocator.Get<ZoneExecutionService>().Post([callback, param = *result]()
                                    {
                                        callback(param);
                                    });
                            }
                        });

                return future;
            };

        Schedule(task);
    }

    void GameRepositoryService::LoadProfileIntroduction(const GamePlayer& player, const std::function<void(PlayerProfileIntroduction&)>& callback)
    {
        auto task = std::make_shared<Task>();
        task->resources.emplace_back(player.GetCId());
        task->operation = [this, cid = player.GetCId(), callback]() -> Future<bool>
            {
                Promise<bool> promise;
                Future<bool> future = promise.GetFuture();

                _serviceLocator.Get<DatabaseService>().GetProfileIntroduction(cid)
                    .Then(_zone.GetStrand(), [weak = _zone.weak_from_this(), this, cid, callback, p = std::move(promise)](std::pair<bool, std::optional<db::dto::ProfileIntroduction>> result) mutable
                        {
                            boost::scope::scope_exit exit([&]()
                                {
                                    p.Set(result.first);
                                });

                            const std::shared_ptr<Zone> zone = weak.lock();
                            if (!zone)
                            {
                                return;
                            }

                            PlayerProfileIntroduction param;

                            if (std::optional<db::dto::ProfileIntroduction>& dto = result.second;
                                dto.has_value())
                            {
                                param.age = std::move(dto->age);
                                param.sex = std::move(dto->sex);
                                param.mail = std::move(dto->mail);
                                param.message = std::move(dto->message);
                            }

                            _serviceLocator.Get<ZoneExecutionService>().Post([callback, param]() mutable 
                                {
                                    callback(param);
                                });
                        });

                return future;
            };

        Schedule(task);
    }

    void GameRepositoryService::Save(const GamePlayer& player, db::ItemTransaction transaction)
    {
        auto task = std::make_shared<Task>();
        task->resources.emplace_back(player.GetCId());
        task->operation = [this, transaction = std::move(transaction)]() mutable
            {
                return _serviceLocator.Get<DatabaseService>().StartTransaction(std::move(transaction));
            };

        Schedule(task);
    }

    void GameRepositoryService::Save(const GamePlayer& host, const GamePlayer& guest, db::ItemTransaction transaction)
    {
        auto task = std::make_shared<Task>();
        task->resources.emplace_back(host.GetCId());
        task->resources.emplace_back(guest.GetCId());
        task->operation = [this, transaction = std::move(transaction)]() mutable
            {
                return _serviceLocator.Get<DatabaseService>().StartTransaction(std::move(transaction));
            };

        Schedule(task);
    }

    void GameRepositoryService::SaveCharacterExp(const GamePlayer& player, int32_t exp)
    {
        auto task = std::make_shared<Task>();
        task->resources.emplace_back(player.GetCId());
        task->operation = [this, cid = player.GetCId(), exp]()
            {
                return _serviceLocator.Get<DatabaseService>().SetCharacterExp(cid, exp);
            };

        Schedule(task);
    }

    void GameRepositoryService::SaveCharacterLevel(const GamePlayer& player, int32_t level, int32_t statPoint)
    {
        auto task = std::make_shared<Task>();
        task->resources.emplace_back(player.GetCId());
        task->operation = [this, cid = player.GetCId(), level, statPoint]() -> Future<bool>
            {
                return _serviceLocator.Get<DatabaseService>().SetCharacterLevel(cid, level, statPoint);
            };

        Schedule(task);
    }

    void GameRepositoryService::SaveJobExp(const GamePlayer& player, int32_t job, int32_t exp)
    {
        auto task = std::make_shared<Task>();
        task->resources.emplace_back(player.GetCId());
        task->operation = [this, cid = player.GetCId(), job, exp]() -> Future<bool>
            {
                return _serviceLocator.Get<DatabaseService>().SetJobExp(cid, job, exp);
            };

        Schedule(task);
    }

    void GameRepositoryService::SaveJobLevel(const GamePlayer& player, int32_t job, int32_t level, int32_t skillPoint, std::vector<req::SkillCreate> skills)
    {
        auto task = std::make_shared<Task>();
        task->resources.emplace_back(player.GetCId());
        task->operation = [this, cid = player.GetCId(), skills = std::move(skills), job, level, skillPoint]() mutable -> Future<bool>
            {
                return _serviceLocator.Get<DatabaseService>().SetJobLevel(cid, job, level, skillPoint, std::move(skills));
            };

        Schedule(task);
    }

    void GameRepositoryService::SaveNewJob(const GamePlayer& player, int32_t job, int32_t jobType, int32_t level, int32_t skillPoint, std::vector<req::SkillCreate> skills)
    {
        auto task = std::make_shared<Task>();
        task->resources.emplace_back(player.GetCId());
        task->operation = [this, cid = player.GetCId(), skills = std::move(skills), job, jobType, level, skillPoint]() mutable -> Future<bool>
            {
                return _serviceLocator.Get<DatabaseService>().AddNewJob(cid, job, jobType, level, skillPoint, std::move(skills));
            };

        Schedule(task);
    }

    void GameRepositoryService::SaveNewSkill(const GamePlayer& player, int32_t job, int32_t skillId, int32_t skillLevel)
    {
        auto task = std::make_shared<Task>();
        task->resources.emplace_back(player.GetCId());
        task->operation = [this, cid = player.GetCId(), job, skillId, skillLevel]() -> Future<bool>
            {
                return _serviceLocator.Get<DatabaseService>().AddSkill(cid, job, skillId, skillLevel);
            };

        Schedule(task);
    }

    void GameRepositoryService::SaveSkillLevel(const GamePlayer& player, int32_t job, int32_t skillPoint, int32_t skillId, int32_t skillLevel)
    {
        auto task = std::make_shared<Task>();
        task->resources.emplace_back(player.GetCId());
        task->operation = [this, cid = player.GetCId(), job, skillPoint, skillId, skillLevel]() -> Future<bool>
            {
                return _serviceLocator.Get<DatabaseService>().SetSkillLevel(cid, job, skillPoint, skillId, skillLevel);
            };

        Schedule(task);
    }

    void GameRepositoryService::SaveMixSkillExp(const GamePlayer& player, int32_t skillId, int32_t skillLevel, int32_t exp)
    {
        auto task = std::make_shared<Task>();
        task->resources.emplace_back(player.GetCId());
        task->operation = [this, cid = player.GetCId(), skillId, skillLevel, exp]() -> Future<bool>
            {
                return _serviceLocator.Get<DatabaseService>().SetMixSkillExp(cid, skillId, skillLevel, exp);
            };

        Schedule(task);
    }

    void GameRepositoryService::SaveSkillPosition(const GamePlayer& player, int32_t skillId, int8_t page, int8_t x, int8_t y)
    {
        auto task = std::make_shared<Task>();
        task->resources.emplace_back(player.GetCId());
        task->operation = [this, cid = player.GetCId(), skillId, page, x, y]() -> Future<bool>
            {
                return _serviceLocator.Get<DatabaseService>().SetSkillPosition(cid, skillId, page, x, y);
            };

        Schedule(task);
    }

    void GameRepositoryService::SaveNewQuest(const GamePlayer& player, int32_t questId, int32_t state, std::string flags, std::string data)
    {
        auto task = std::make_shared<Task>();
        task->resources.emplace_back(player.GetCId());
        task->operation = [this, cid = player.GetCId(), flags = std::move(flags), data = std::move(data), questId, state]() mutable -> Future<bool>
            {
                return _serviceLocator.Get<DatabaseService>().AddQuest(cid, questId, state, std::move(flags), std::move(data));
            };

        Schedule(task);
    }

    void GameRepositoryService::SaveQuestChange(const GamePlayer& player, int32_t questId, int32_t state, std::string flags, std::string data)
    {
        auto task = std::make_shared<Task>();
        task->resources.emplace_back(player.GetCId());
        task->operation = [this, cid = player.GetCId(), flags = std::move(flags), data = std::move(data), questId, state]() mutable -> Future<bool>
            {
                return _serviceLocator.Get<DatabaseService>().SetQuest(cid, questId, state, std::move(flags), std::move(data));
            };

        Schedule(task);
    }

    void GameRepositoryService::SaveHair(const GamePlayer& player, int32_t hair)
    {
        auto task = std::make_shared<Task>();
        task->resources.emplace_back(player.GetCId());
        task->operation = [this, cid = player.GetCId(), hair]() -> Future<bool>
            {
                return _serviceLocator.Get<DatabaseService>().SetHair(cid, hair);
            };

        Schedule(task);
    }

    void GameRepositoryService::SaveHairColor(const GamePlayer& player, int32_t hairColor)
    {
        auto task = std::make_shared<Task>();
        task->resources.emplace_back(player.GetCId());
        task->operation = [this, cid = player.GetCId(), hairColor]() -> Future<bool>
            {
                return _serviceLocator.Get<DatabaseService>().SetHairColor(cid, hairColor);
            };

        Schedule(task);
    }

    void GameRepositoryService::SaveProfile(const GamePlayer& player)
    {
        const PlayerProfileComponent& profileComponent = player.GetProfileComponent();

        const int8_t refusePartyInvite = profileComponent.IsConfigured(PlayerProfileSetting::RefusePartyInvite) ? 1 : 0;
        const int8_t refuseChannelInvite = profileComponent.IsConfigured(PlayerProfileSetting::RefuseChannelInvite) ? 1 : 0;
        const int8_t refuseGuildInvite = profileComponent.IsConfigured(PlayerProfileSetting::RefuseGuildInvite) ? 1 : 0;
        const int8_t privateProfile = profileComponent.IsConfigured(PlayerProfileSetting::Private) ? 1 : 0;
        const std::optional<PlayerProfileIntroduction> introduction = profileComponent.GetIntroduction();

        auto task = std::make_shared<Task>();
        task->resources.emplace_back(player.GetCId());
        task->operation = [this, cid = player.GetCId(), refusePartyInvite, refuseChannelInvite, refuseGuildInvite, privateProfile, introduction]() -> Future<bool>
            {
                return _serviceLocator.Get<DatabaseService>().SetProfile(cid,
                    refusePartyInvite, refuseChannelInvite, refuseGuildInvite, privateProfile,
                    introduction.has_value() ? introduction->age : "",
                    introduction.has_value() ? introduction->sex : "",
                    introduction.has_value() ? introduction->mail : "",
                    introduction.has_value() ? introduction->message : "");
            };

        Schedule(task);
    }

    void GameRepositoryService::SaveStat(const GamePlayer& player, int32_t statPoint, int32_t str, int32_t dex, int32_t accr, int32_t health, int32_t intell, int32_t wis, int32_t will)
    {
        auto task = std::make_shared<Task>();
        task->resources.emplace_back(player.GetCId());
        task->operation = [this, cid = player.GetCId(), statPoint, str, dex, accr, health, intell, wis, will]() -> Future<bool>
            {
                return _serviceLocator.Get<DatabaseService>().SetStat(cid, statPoint, str, dex, accr, health, intell, wis, will);
            };

        Schedule(task);
    }

    void GameRepositoryService::SaveState(const GamePlayer& player, int32_t zone, int32_t stage, float x, float y, float yaw)
    {
        const PlayerStatComponent& statComponent = player.GetStatComponent();
        const int32_t hp = statComponent.GetFinalStat(RecoveryStatType::HP).As<int32_t>();
        const int32_t sp = statComponent.GetFinalStat(RecoveryStatType::SP).As<int32_t>();
        const int8_t isArmed = player.IsArmed() ? 1 : 0;
        const int8_t isRunning = player.IsRunning() ? 1 : 0;
        const int8_t isDead = player.IsDead() ? 1 : 0;

        auto task = std::make_shared<Task>();
        task->resources.emplace_back(player.GetCId());
        task->operation = [this, cid = player.GetCId(), zone, stage, x, y, yaw, isArmed, isRunning, isDead, hp, sp]() -> Future<bool>
            {
                return _serviceLocator.Get<DatabaseService>().SaveState(cid, zone, stage, x, y, yaw, isArmed, isRunning, isDead, hp, sp);
            };

        Schedule(task);
    }

    void GameRepositoryService::Schedule(const SharedPtrNotNull<Task>& task)
    {
        assert(!task->resources.empty());
        assert(task->operation.operator bool());
        assert(task->created != game_time_point_type{});

        if (CanRun(*task))
        {
            Run(task);
        }
        else
        {
            for (int64_t cid : task->resources)
            {
                _resources[cid].waiters.insert(task);
            }
        }
    }

    bool GameRepositoryService::CanRun(const Task& task) const
    {
        return std::ranges::all_of(task.resources, [this, &task](int64_t cid)
            {
                const auto iter = _resources.find(cid);
                if (iter == _resources.end())
                {
                    return true;
                }

                const Resource& resource = iter->second;

                if (resource.owner)
                {
                    return false;
                }

                if (!resource.waiters.empty())
                {
                    const SharedPtrNotNull<Task>& waiter = *resource.waiters.begin();

                    if (waiter.get() != &task && *waiter < task)
                    {
                        return false;
                    }
                }

                return true;
            });
    }

    void GameRepositoryService::Run(const SharedPtrNotNull<Task>& task)
    {
        assert(CanRun(*task));

        for (int64_t cid : task->resources)
        {
            assert(!_resources.contains(cid) || _resources[cid].owner == nullptr);

            _resources[cid].owner = task.get();
        }

        task->operation().Then(_zone.GetStrand(),
            [weak = _zone.weak_from_this(), this, task](bool success)
            {
                const std::shared_ptr<Zone> zone = weak.lock();
                if (!zone)
                {
                    return;
                }

                if (!success)
                {
                    for (int64_t cid : task->resources)
                    {
                        if (const auto iter = _players.find(cid);
                            iter != _players.end())
                        {
                            if (const std::shared_ptr<GameClient> client = iter->second.lock();
                                client)
                            {
                                client->Disconnect();
                            }
                        }
                    }
                }

                OnComplete(task);
            });
    }

    void GameRepositoryService::OnComplete(const SharedPtrNotNull<Task>& task)
    {
        _resourceBuffer.clear();

        for (int64_t cid : task->resources)
        {
            assert(_resources.contains(cid));
            assert(_resources[cid].owner == task.get());

            Resource& resource = _resources[cid];
            resource.owner = nullptr;

            if (resource.waiters.empty())
            {
                if (resource.completionPromise.has_value())
                {
                    resource.completionPromise->Set();
                }

                _resources.erase(cid);
            }
            else
            {
                _resourceBuffer.emplace_back(&resource);
            }
        }

        std::ranges::sort(_resourceBuffer, [](const Resource* lhs, const Resource* rhs)
            {
                const SharedPtrNotNull<Task>& lValue = *lhs->waiters.begin();
                const SharedPtrNotNull<Task>& rValue = *rhs->waiters.begin();

                return *lValue < *rValue;
            });

        for (int64_t i = 0; i < std::ssize(_resourceBuffer); ++i)
        {
            Resource& resource = *_resourceBuffer[i];

            const SharedPtrNotNull<Task> waiter = *resource.waiters.begin();

            if (CanRun(*waiter))
            {
                for (int64_t cid : waiter->resources)
                {
                    _resources[cid].waiters.erase(waiter);
                }

                Run(waiter);
            }
        }
    }

    bool GameRepositoryService::Task::operator==(const Task& other) const
    {
        return this == &other;
    }

    bool GameRepositoryService::Task::operator<(const Task& other) const
    {
        if (created == other.created)
        {
            return reinterpret_cast<size_t>(this) < reinterpret_cast<size_t>(&other);
        }

        return created < other.created;
    }

    bool GameRepositoryService::TaskComparer::operator()(const SharedPtrNotNull<Task>& lhs, const SharedPtrNotNull<Task>& rhs) const
    {
        assert(lhs);
        assert(rhs);

        return *lhs < *rhs;
    }
}
