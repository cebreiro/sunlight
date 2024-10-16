#include "game_monster.h"

#include "sl/emulator/game/component/entity_status_effect_component.h"
#include "sl/emulator/service/gamedata/monster/monster_data.h"

namespace sunlight
{
    GameMonster::GameMonster(GameEntityIdPublisher& idPublisher, const MonsterData& data, std::optional<game_entity_id_type> spawnerId)
        : GameEntity(idPublisher, TYPE)
        , _data(data)
        , _spawnerId(spawnerId)
    {
        AddComponent(std::make_unique<EntityStatusEffectComponent>());
    }

    bool GameMonster::IsInvisible() const
    {
        return false;
    }

    auto GameMonster::GetDataId() const -> int32_t
    {
        return _data.GetId();
    }

    auto GameMonster::GetData() const -> const MonsterData&
    {
        return _data;
    }

    auto GameMonster::GetSpawnerId() const -> std::optional<game_entity_id_type>
    {
        return _spawnerId;
    }
}
