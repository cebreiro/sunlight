#pragma once
#include "sl/emulator/game/entity/game_entity_id_type.h"
#include "sl/emulator/game/entity/game_entity_type.h"

namespace sunlight
{
    class GameEntityIdPublisher final : public IService
    {
    public:
        static constexpr game_entity_id_type::value_type NULL_ID = 0;

    public:
        explicit GameEntityIdPublisher(int32_t zoneId);

        auto GetName() const -> std::string_view override;

    public:
        auto Publish(GameEntityType type) -> game_entity_id_type;
        void Return(GameEntityType type, game_entity_id_type id);

        auto PublishSceneObjectId(GameEntityType type) -> int32_t;
        void ReturnSceneObjectId(GameEntityType type, int32_t id);

    private:
        static bool IsEntityType(int32_t value);

    private:
        std::string _name;

        game_entity_id_type::value_type _nextValue = {};
        std::queue<game_entity_id_type> _recycleQueue;

        int32_t _nextSceneObjectId = 1;
        std::queue<int32_t> _recycleQueueSceneObjectId;
    };
}
