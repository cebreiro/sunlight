#include "game_client_storage.h"

#include "sl/generator/server/client/game_client.h"

namespace sunlight
{
    bool GameClientStorage::Add(SharedPtrNotNull<GameClient> client)
    {
        const game_client_id_type id = client->GetId();

        std::unique_lock lock(_mutex);

        return _clients.try_emplace(id, std::move(client)).second;
    }

    bool GameClientStorage::Remove(game_client_id_type id)
    {
        std::unique_lock lock(_mutex);

        return _clients.erase(id);
    }

    auto GameClientStorage::Find(game_client_id_type id) -> std::shared_ptr<GameClient>
    {
        std::shared_lock lock(_mutex);

        const auto iter = _clients.find(id);

        return iter != _clients.end() ? iter->second : nullptr;
    }

    auto GameClientStorage::Extract(game_client_id_type id) -> std::shared_ptr<GameClient>
    {
        std::unique_lock lock(_mutex);

        std::shared_ptr<GameClient> result;

        auto iter = _clients.find(id);
        if (iter == _clients.end())
        {
            return result;
        }

        std::swap(result, iter->second);
        _clients.erase(iter);

        return result;
    }

    auto GameClientStorage::GetName() const -> std::string_view
    {
        return "game_client_storage";
    }
}
