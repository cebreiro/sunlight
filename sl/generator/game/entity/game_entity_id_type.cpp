#include "game_entity_id_type.h"

namespace sunlight::detail
{
    GameEntityIdType::GameEntityIdType(uint32_t value, uint32_t recycleSequence)
        : _value(value)
        , _recycleSequence(recycleSequence)
    {
    }

    auto GameEntityIdType::Unwrap() const -> uint32_t
    {
        return _value;
    }

    auto GameEntityIdType::GetRecycleSequence() const -> uint32_t
    {
        return _recycleSequence;
    }

    void GameEntityIdType::SetRecycleSequence(uint32_t sequence)
    {
        _recycleSequence = sequence;
    }

    bool operator==(const GameEntityIdType& lhs, const GameEntityIdType& rhs)
    {
        return lhs._value == rhs._value;
    }

    bool operator<(const GameEntityIdType& lhs, const GameEntityIdType& rhs)
    {
        return lhs._value < rhs._value;
    }

    auto hash_value(GameEntityIdType id) -> size_t
    {
        boost::hash<uint32_t> hasher;

        return hasher(id.Unwrap());
    }
}
