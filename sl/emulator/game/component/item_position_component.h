#pragma once
#include "sl/emulator/game/component/game_component.h"
#include "sl/emulator/game/contants/item/item_position.h"

namespace sunlight
{
    class ItemPositionComponent final : public GameComponent
    {
    public:
        bool IsInQuickSlot() const;

        auto GetPositionType() const -> ItemPositionType;
        auto GetPage() const -> int8_t;
        auto GetX() const -> int8_t;
        auto GetY() const -> int8_t;

        void SetPositionType(ItemPositionType type);
        void SetPage(int8_t value);
        void SetX(int8_t value);
        void SetY(int8_t value);

    private:
        ItemPositionType _positionType = {};

        int8_t _page = 0;
        int8_t _x = 0;
        int8_t _y = 0;
    };
}
