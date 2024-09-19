#include "game_repository_system.h"

#include "sl/emulator/game/entity/game_player.h"
#include "sl/emulator/server/client/game_client.h"
#include "sl/emulator/service/database/database_service.h"

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
