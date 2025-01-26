#pragma once
#include <boost/unordered/unordered_flat_map.hpp>
#include "sl/generator/game/system/game_system.h"

namespace sunlight
{
    class GamePlayer;
}

namespace sunlight
{
    class PlayerIndexSystem final : public GameSystem
    {
    public:
        auto GetName() const -> std::string_view override;
        auto GetClassId() const -> game_system_id_type override;

    public:
        void Visit(const std::function<void(GamePlayer&)>& function);

        void OnStageEnter(GamePlayer& player);
        void OnStageExit(GamePlayer& player);

        auto FindByCId(int64_t cid) -> GamePlayer*;
        auto FindByCId(int64_t cid) const -> const GamePlayer*;
        auto FindByName(const std::string& name) -> GamePlayer*;
        auto FindByName(const std::string& name) const -> const GamePlayer*;

        auto GetPlayerCount() const -> int64_t;
        inline auto GetPlayerRange() const;

    private:
        boost::unordered::unordered_flat_map<int64_t, PtrNotNull<GamePlayer>> _cidIndex;
        boost::unordered::unordered_flat_map<std::string, PtrNotNull<GamePlayer>> _nameIndex;
    };

    inline auto PlayerIndexSystem::GetPlayerRange() const
    {
        return _cidIndex | std::views::values | notnull::reference;
    }
}
