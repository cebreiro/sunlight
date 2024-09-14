#pragma once

namespace sunlight
{
    class GameEntity;
}

namespace sunlight
{
    class GameEntityNetworkId
    {
    public:
        GameEntityNetworkId() = default;
        explicit GameEntityNetworkId(const GameEntity& entity);

        auto ToBuffer() -> std::span<const char>;

        static auto Null() -> GameEntityNetworkId;

    private:
        std::array<char, 8> _array = {};
    };
}
