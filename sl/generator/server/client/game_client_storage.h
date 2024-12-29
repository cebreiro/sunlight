#pragma once
#include "sl/generator/server/client/game_client_id.h"

namespace sunlight
{
    class GameClient;
}

namespace sunlight
{
    class GameClientStorage final
        : public IService
        , public std::enable_shared_from_this<GameClientStorage>
    {
    public:
        bool Add(SharedPtrNotNull<GameClient> client);
        bool Remove(game_client_id_type id);

        auto Find(game_client_id_type id) -> std::shared_ptr<GameClient>;
        auto Extract(game_client_id_type id) -> std::shared_ptr<GameClient>;

        auto GetName() const -> std::string_view override;

    private:
        mutable std::shared_mutex _mutex;

        std::unordered_map<game_client_id_type, SharedPtrNotNull<GameClient>> _clients;
    };
}
