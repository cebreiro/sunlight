#pragma once

namespace sunlight
{
    class Zone;

    class GameEntity;
    class GamePlayer;
}

namespace sunlight
{
    class ZoneExecutionService final : public IService
    {
    public:
        explicit ZoneExecutionService(Zone& zone);

        auto GetName() const -> std::string_view override;

    public:
        void AddTimer(std::chrono::milliseconds delay, const std::function<void()>& function);
        void AddTimer(std::chrono::milliseconds delay, const GameEntity& entity, int32_t stageId, const std::function<void(GameEntity&)>& function);
        void AddTimer(std::chrono::milliseconds delay, int64_t playerId, int32_t stageId, const std::function<void(GamePlayer&)>& function);

    private:
        Zone& _zone;
    };
}
