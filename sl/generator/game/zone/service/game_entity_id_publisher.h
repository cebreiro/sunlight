#pragma once
#include "sl/generator/game/entity/game_entity_id_type.h"
#include "sl/generator/game/entity/game_entity_type.h"

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
        static bool IsDynamicEntity(GameEntityType type);
        static bool IsStaticEntity(GameEntityType type);

    private:
        std::string _name;

        game_entity_id_type::value_type _nextValue = {};
        std::queue<game_entity_id_type> _recycleQueue;

        int32_t _nextDynamicEntityId = 0x10000000;
        int32_t _nextStaticEntityId = 0x20000000;
        int32_t _nextUnknownEntityId = 0x30000000;

        std::queue<int32_t> _recycleIdQueueDynamicEntity;
        std::queue<int32_t> _recycleIdQueueStaticEntity;
    };
}
