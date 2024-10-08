#include "game_repository_system.h"

#include "sl/emulator/game/component/player_profile_component.h"
#include "sl/emulator/game/component/player_stat_component.h"
#include "sl/emulator/game/contants/quest/quest.h"
#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/server/client/game_client.h"
#include "sl/emulator/service/database/database_service.h"
#include "sl/emulator/service/database/dto/profile_introduction.h"

namespace sunlight
{
    GameRepositorySystem::GameRepositorySystem(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    void GameRepositorySystem::InitializeSubSystem(Stage& stage)
    {
        (void)stage;
    }

    bool GameRepositorySystem::Subscribe(Stage& stage)
    {
        (void)stage;

        return true;
    }

    auto GameRepositorySystem::GetName() const -> std::string_view
    {
        return "game_repository_system";
    }

    auto GameRepositorySystem::GetClassId() const -> game_system_id_type
    {
        return GameSystem::GetClassId<GameRepositorySystem>();
    }

    bool GameRepositorySystem::IsPending() const
    {
        return !_pending.empty();
    }

    bool GameRepositorySystem::IsPendingSaves(const GamePlayer& player) const
    {
        return _pending.contains(player.GetCId());
    }

    auto GameRepositorySystem::WaitForSaveCompletion(const GamePlayer& player) -> Future<void>
    {
        const int64_t cid = player.GetCId();

        if (!_pending.contains(cid))
        {
            co_return;
        }

        assert(!_saveCompletionPromise.contains(cid));

        Promise<void> promise;
        Future<void> future = promise.GetFuture();

        _saveCompletionPromise[cid] = std::move(promise);

        co_await future;

        co_return;
    }

    void GameRepositorySystem::LoadAccountStorage(const GamePlayer& player, const std::function<void(const db::dto::AccountStorage&)>& callback)
    {
        ++_pending[player.GetCId()].first;

        const auto handler = [this, cid = player.GetCId(), callback](const std::optional<db::dto::AccountStorage>& result)
            {
                if (result.has_value())
                {
                    OnComplete(cid);

                    callback(*result);
                }
                else
                {
                    OnError(cid);
                }
            };

        _serviceLocator.Get<DatabaseService>().GetAccountStorage(player.GetAId())
            .Then(*ExecutionContext::GetExecutor(), handler);
    }

    void GameRepositorySystem::LoadProfileIntroduction(const GamePlayer& player, const std::function<void(PlayerProfileIntroduction&)>& callback)
    {
        (void)player;

        ++_pending[player.GetCId()].first;

        const auto handler = [this, cid = player.GetCId(), callback](std::pair<bool, std::optional<db::dto::ProfileIntroduction>> result)
            {
                if (result.first)
                {
                    OnComplete(cid);

                    std::optional<db::dto::ProfileIntroduction>& dto = result.second;

                    PlayerProfileIntroduction param;

                    if (dto.has_value())
                    {
                        param.age = std::move(dto->age);
                        param.sex = std::move(dto->sex);
                        param.mail = std::move(dto->mail);
                        param.message = std::move(dto->message);
                    }

                    callback(param);
                }
                else
                {
                    OnError(cid);
                }
            };

        _serviceLocator.Get<DatabaseService>().GetProfileIntroduction(player.GetCId())
            .Then(*ExecutionContext::GetExecutor(), handler);
    }

    void GameRepositorySystem::Save(const GamePlayer& player, db::ItemTransaction transaction)
    {
        ++_pending[player.GetCId()].first;

        _serviceLocator.Get<DatabaseService>().StartTransaction(std::move(transaction))
            .Then(*ExecutionContext::GetExecutor(), [this, cid = player.GetCId()](bool success)
                {
                    if (success)
                    {
                        OnComplete(cid);
                    }
                    else
                    {
                        OnError(cid);
                    }
                });
    }

    void GameRepositorySystem::Save(const GamePlayer& host, const GamePlayer& guest, db::ItemTransaction transaction)
    {
        ++_pending[host.GetCId()].first;
        ++_pending[guest.GetCId()].first;

        _serviceLocator.Get<DatabaseService>().StartTransaction(std::move(transaction))
            .Then(*ExecutionContext::GetExecutor(), [this, hostId = host.GetCId(), guestId = guest.GetCId()](bool success)
                {
                    if (success)
                    {
                        OnComplete(hostId);
                        OnComplete(guestId);
                    }
                    else
                    {
                        OnError(hostId);
                        OnError(guestId);
                    }
                });
    }

    void GameRepositorySystem::SaveCharacterExp(const GamePlayer& player, int32_t exp)
    {
        ++_pending[player.GetCId()].first;

        _serviceLocator.Get<DatabaseService>().SetCharacterExp(player.GetCId(), exp)
            .Then(*ExecutionContext::GetExecutor(), [this, cid = player.GetCId()](bool success)
                {
                    if (success)
                    {
                        OnComplete(cid);
                    }
                    else
                    {
                        OnError(cid);
                    }
                });
    }

    void GameRepositorySystem::SaveCharacterLevel(const GamePlayer& player, int32_t level, int32_t statPoint)
    {
        ++_pending[player.GetCId()].first;

        _serviceLocator.Get<DatabaseService>().SetCharacterLevel(player.GetCId(), level, statPoint)
            .Then(*ExecutionContext::GetExecutor(), [this, cid = player.GetCId()](bool success)
                {
                    if (success)
                    {
                        OnComplete(cid);
                    }
                    else
                    {
                        OnError(cid);
                    }
                });
    }

    void GameRepositorySystem::SaveJobExp(const GamePlayer& player, int32_t job, int32_t exp)
    {
        ++_pending[player.GetCId()].first;

        _serviceLocator.Get<DatabaseService>().SetJobExp(player.GetCId(), job, exp)
            .Then(*ExecutionContext::GetExecutor(), [this, cid = player.GetCId()](bool success)
                {
                    if (success)
                    {
                        OnComplete(cid);
                    }
                    else
                    {
                        OnError(cid);
                    }
                });
    }

    void GameRepositorySystem::SaveJobLevel(const GamePlayer& player, int32_t job, int32_t level, int32_t skillPoint, std::vector<req::SkillCreate> skills)
    {
        ++_pending[player.GetCId()].first;

        _serviceLocator.Get<DatabaseService>().SetJobLevel(player.GetCId(), job, level, skillPoint, std::move(skills))
            .Then(*ExecutionContext::GetExecutor(), [this, cid = player.GetCId()](bool success)
                {
                    if (success)
                    {
                        OnComplete(cid);
                    }
                    else
                    {
                        OnError(cid);
                    }
                });
    }

    void GameRepositorySystem::SaveNewJob(const GamePlayer& player, int32_t job, int32_t jobType, int32_t level, int32_t skillPoint, std::vector<req::SkillCreate> skills)
    {
        ++_pending[player.GetCId()].first;

        _serviceLocator.Get<DatabaseService>().AddNewJob(player.GetCId(), job, jobType, level, skillPoint, std::move(skills))
            .Then(*ExecutionContext::GetExecutor(), [this, cid = player.GetCId()](bool success)
                {
                    if (success)
                    {
                        OnComplete(cid);
                    }
                    else
                    {
                        OnError(cid);
                    }
                });
    }

    void GameRepositorySystem::SaveNewSkill(const GamePlayer& player, int32_t job, int32_t skillId, int32_t skillLevel)
    {
        ++_pending[player.GetCId()].first;

        _serviceLocator.Get<DatabaseService>().AddSkill(player.GetCId(), job, skillId, skillLevel)
            .Then(*ExecutionContext::GetExecutor(), [this, cid = player.GetCId()](bool success)
                {
                    if (success)
                    {
                        OnComplete(cid);
                    }
                    else
                    {
                        OnError(cid);
                    }
                });
    }

    void GameRepositorySystem::SaveSkillLevel(const GamePlayer& player, int32_t job, int32_t skillPoint,
        int32_t skillId, int32_t skillLevel)
    {
        ++_pending[player.GetCId()].first;

        _serviceLocator.Get<DatabaseService>().SetSkillLevel(player.GetCId(), job, skillPoint, skillId, skillLevel)
            .Then(*ExecutionContext::GetExecutor(), [this, cid = player.GetCId()](bool success)
                {
                    if (success)
                    {
                        OnComplete(cid);
                    }
                    else
                    {
                        OnError(cid);
                    }
                });
    }

    void GameRepositorySystem::SaveMixSkillExp(const GamePlayer& player, int32_t skillId, int32_t skillLevel, int32_t exp)
    {
        ++_pending[player.GetCId()].first;

        _serviceLocator.Get<DatabaseService>().SetMixSkillExp(player.GetCId(), skillId, skillLevel, exp)
            .Then(*ExecutionContext::GetExecutor(), [this, cid = player.GetCId()](bool success)
                {
                    if (success)
                    {
                        OnComplete(cid);
                    }
                    else
                    {
                        OnError(cid);
                    }
                });
    }

    void GameRepositorySystem::SaveNewQuest(const GamePlayer& player, int32_t questId, int32_t state, std::string flags, std::string data)
    {
        ++_pending[player.GetCId()].first;

        _serviceLocator.Get<DatabaseService>().AddQuest(player.GetCId(), questId, state, std::move(flags), std::move(data))
            .Then(*ExecutionContext::GetExecutor(), [this, cid = player.GetCId()](bool success)
                {
                    if (success)
                    {
                        OnComplete(cid);
                    }
                    else
                    {
                        OnError(cid);
                    }
                });
    }

    void GameRepositorySystem::SaveQuestChange(const GamePlayer& player, int32_t questId, int32_t state, std::string flags, std::string data)
    {
        ++_pending[player.GetCId()].first;

        _serviceLocator.Get<DatabaseService>().SetQuest(player.GetCId(), questId, state, std::move(flags), std::move(data))
            .Then(*ExecutionContext::GetExecutor(), [this, cid = player.GetCId()](bool success)
                {
                    if (success)
                    {
                        OnComplete(cid);
                    }
                    else
                    {
                        OnError(cid);
                    }
                });
    }

    void GameRepositorySystem::SaveHair(const GamePlayer& player, int32_t hair)
    {
        ++_pending[player.GetCId()].first;

        _serviceLocator.Get<DatabaseService>().SetHair(player.GetCId(), hair)
            .Then(*ExecutionContext::GetExecutor(), [this, cid = player.GetCId()](bool success)
                {
                    if (success)
                    {
                        OnComplete(cid);
                    }
                    else
                    {
                        OnError(cid);
                    }
                });
    }

    void GameRepositorySystem::SaveHairColor(const GamePlayer& player, int32_t hairColor)
    {
        ++_pending[player.GetCId()].first;

        _serviceLocator.Get<DatabaseService>().SetHairColor(player.GetCId(), hairColor)
            .Then(*ExecutionContext::GetExecutor(), [this, cid = player.GetCId()](bool success)
                {
                    if (success)
                    {
                        OnComplete(cid);
                    }
                    else
                    {
                        OnError(cid);
                    }
                });
    }

    void GameRepositorySystem::SaveProfile(const GamePlayer& player)
    {
        ++_pending[player.GetCId()].first;

        const PlayerProfileComponent& profileComponent = player.GetProfileComponent();

        const int8_t refusePartyInvite = profileComponent.IsConfigured(PlayerProfileSetting::RefusePartyInvite) ? 1 : 0;
        const int8_t refuseChannelInvite = profileComponent.IsConfigured(PlayerProfileSetting::RefuseChannelInvite) ? 1 : 0;
        const int8_t refuseGuildInvite = profileComponent.IsConfigured(PlayerProfileSetting::RefuseGuildInvite) ? 1 : 0;
        const int8_t privateProfile = profileComponent.IsConfigured(PlayerProfileSetting::Private) ? 1 : 0;
        const std::optional<PlayerProfileIntroduction>& introduction = profileComponent.GetIntroduction();

        Future<bool> future = _serviceLocator.Get<DatabaseService>().SetProfile(player.GetCId(),
            refusePartyInvite, refuseChannelInvite, refuseGuildInvite, privateProfile,
            introduction.has_value() ? introduction->age : "",
            introduction.has_value() ? introduction->sex : "",
            introduction.has_value() ? introduction->mail : "",
            introduction.has_value() ? introduction->message : "");

        future.Then(*ExecutionContext::GetExecutor(), [this, cid = player.GetCId()](bool success)
            {
                if (success)
                {
                    OnComplete(cid);
                }
                else
                {
                    OnError(cid);
                }
            });
    }

    void GameRepositorySystem::SaveStat(const GamePlayer& player, int32_t statPoint, int32_t str, int32_t dex,
        int32_t accr, int32_t health, int32_t intell, int32_t wis, int32_t will)
    {
        ++_pending[player.GetCId()].first;

        _serviceLocator.Get<DatabaseService>().SetStat(player.GetCId(), statPoint, str, dex, accr, health, intell, wis, will)
            .Then(*ExecutionContext::GetExecutor(), [this, cid = player.GetCId()](bool success)
                {
                    if (success)
                    {
                        OnComplete(cid);
                    }
                    else
                    {
                        OnError(cid);
                    }
                });
    }

    void GameRepositorySystem::SaveState(const GamePlayer& player, int32_t zone, int32_t stage, float x, float y, float yaw)
    {
        ++_pending[player.GetCId()].first;

        const PlayerStatComponent& statComponent = player.GetStatComponent();
        const int32_t hp = statComponent.GetFinalStat(RecoveryStatType::HP).As<int32_t>();
        const int32_t sp = statComponent.GetFinalStat(RecoveryStatType::SP).As<int32_t>();

        _serviceLocator.Get<DatabaseService>().SaveState(player.GetCId(), zone, stage, x, y, yaw, player.IsArmed(), player.IsRunning(), hp, sp)
            .Then(*ExecutionContext::GetExecutor(), [this, cid = player.GetCId()](bool success)
                {
                    if (success)
                    {
                        OnComplete(cid);
                    }
                    else
                    {
                        OnError(cid);
                    }
                });
    }

    void GameRepositorySystem::OnComplete(int64_t cid)
    {
        auto iter = _pending.find(cid);
        if (iter == _pending.end())
        {
            assert(false);

            return;
        }

        if (const int32_t count = --iter->second.first; count == 0)
        {
            _pending.erase(iter);

            const auto iter2 = _saveCompletionPromise.find(cid);
            if (iter2 != _saveCompletionPromise.end())
            {
                Promise<void>& promise = iter2->second;

                promise.Set();

                _saveCompletionPromise.erase(iter2);
            }
        }
    }

    void GameRepositorySystem::OnError(int64_t cid)
    {
        auto iter = _pending.find(cid);
        if (iter == _pending.end())
        {
            assert(false);

            return;
        }

        auto& [pendingCount, weak] = iter->second;
        std::shared_ptr<GameClient> client = weak.lock();

        if (const int32_t count = --pendingCount; count == 0)
        {
            _pending.erase(iter);

            const auto iter2 = _saveCompletionPromise.find(cid);
            if (iter2 != _saveCompletionPromise.end())
            {
                Promise<void>& promise = iter2->second;

                promise.Set();

                _saveCompletionPromise.erase(iter2);
            }
        }

        SUNLIGHT_LOG_CRITICAL(_serviceLocator,
            fmt::format("[{}] database error occur. cid: {}",
                GetName(), cid));

        if (client)
        {
            client->Disconnect();
        }
    }
}
