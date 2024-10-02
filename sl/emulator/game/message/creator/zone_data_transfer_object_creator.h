#pragma once

namespace sunlight
{
    class GamePlayer;
}

namespace sunlight
{
    class ZoneDataTransferObjectCreator
    {
    public:
        ZoneDataTransferObjectCreator() = delete;

        static auto CreatePlayerGroupState(const GamePlayer& player) -> Buffer;
        static auto CreatePlayerAppearance(const GamePlayer& player) -> Buffer;
        static auto CreatePlayerInformation(const GamePlayer& player) -> Buffer;
        static auto CreatePlayerPet(const GamePlayer& player) -> Buffer;
        static auto CreatePlayerSkill(const GamePlayer& player) -> Buffer;
        static auto CreatePlayerStatusEffect(const GamePlayer& player) -> Buffer;

        static auto CreatePlayerStat(const GamePlayer& player) -> Buffer;
        static auto CreatePlayerWeaponMotion(const GamePlayer& player) -> Buffer;
    };
}
