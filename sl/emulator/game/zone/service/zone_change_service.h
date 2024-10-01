#pragma once

namespace sunlight
{
    class Zone;

    class GamePlayer;
}

namespace sunlight
{
    class ZoneChangeService final : public IService
    {
    public:
        explicit ZoneChangeService(Zone& zone);

        auto GetName() const -> std::string_view override;

    public:
        bool StartStageChange(GamePlayer& player, int32_t destStageId, int32_t destX, int32_t destY);

    private:
        Zone& _zone;
    };
}
