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

    void ItemPositionComponent::SetPosition(int8_t page, int8_t x, int8_t y)
    {
        _page = page;
        _x = x;
        _y = y;
    }

    void ItemPositionComponent::ResetPosition()
    {
        _page = -1;
        _x = -1;
        _y = -1;
    }

    void ItemPositionComponent::SwapPosition(ItemPositionComponent& other)
    {
        std::swap(_positionType, other._positionType);
        std::swap(_page, other._page);
        std::swap(_x, other._x);
        std::swap(_y, other._y);
    }
}
