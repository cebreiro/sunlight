#pragma once
#include "sl/generator/game/entity/game_entity_id_type.h"

namespace sunlight
{
    class Zone;

    class GameEntity;
    class GamePlayer;
    class GameMonster;
}

namespace sunlight
{
    class ZoneExecutionService final : public IService
    {
    public:
        explicit ZoneExecutionService(Zone& zone);

        auto GetName() const -> std::string_view override;

    public:
        void Post(const std::function<void()>& function,
            const std::source_location& current = std::source_location::current());
        void Post(game_entity_id_type id, int32_t stageId, const std::function<void(GameMonster&)>& function,
            const std::source_location& current = std::source_location::current());
        void Post(int64_t playerId, int32_t stageId, const std::function<void(GamePlayer&)>& function,
            const std::source_location& current = std::source_location::current());

        void AddTimer(std::chrono::milliseconds delay, const std::function<void()>& function,
            const std::source_location& current = std::source_location::current());
        void AddTimer(std::chrono::milliseconds delay, const GameEntity& entity, int32_t stageId, const std::function<void(GameEntity&)>& function,
            const std::source_location& current = std::source_location::current());
        void AddTimer(std::chrono::milliseconds delay, int64_t playerId, int32_t stageId, const std::function<void(GamePlayer&)>& function,
            const std::source_location& current = std::source_location::current());

    private:
        Zone& _zone;
    };
}
