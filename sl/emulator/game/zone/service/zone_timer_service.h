#pragma once

namespace sunlight
{
    class Zone;

    class GamePlayer;
}
namespace sunlight
{
    class ZoneTimerService final : public IService
    {
    public:
        explicit ZoneTimerService(Zone& zone);

        auto GetName() const -> std::string_view override;

    public:
        void AddTimer(std::chrono::milliseconds delay, int64_t playerId, int32_t stageId, const std::function<void(GamePlayer&)>& function);

    private:
        Zone& _zone;
    };
}
