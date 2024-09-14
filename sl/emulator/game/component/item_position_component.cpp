#include "item_position_component.h"

namespace sunlight
{
    bool ItemPositionComponent::IsInQuickSlot() const
    {
        return _positionType == ItemPositionType::QuickSlot;
    }

    auto ItemPositionComponent::GetPositionType() const -> ItemPositionType
    {
        return _positionType;
    }

    auto ItemPositionComponent::GetPage() const -> int8_t
    {
        return _page;
    }

    auto ItemPositionComponent::GetX() const -> int8_t
    {
        return _x;
    }

    auto ItemPositionComponent::GetY() const -> int8_t
    {
        return _y;
    }

    void ItemPositionComponent::SetPositionType(ItemPositionType type)
    {
        _positionType = type;
    }

    void ItemPositionComponent::SetPage(int8_t value)
    {
        _page = value;
    }

    void ItemPositionComponent::SetX(int8_t value)
    {
        _x = value;
    }

    void ItemPositionComponent::SetY(int8_t value)
    {
        _y = value;
    }
}
