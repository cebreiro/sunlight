#include "item_mix_prop_item_mapping.h"

namespace sunlight
{
    const ItemMixPropItemMapping ItemMixPropItemMapping::_instance;

    ItemMixPropItemMapping::ItemMixPropItemMapping()
        : _items({
            std::pair(0xE8B0E2B1, 5072061),
            std::pair(0xDEB1EDB0, 5072091),
            std::pair(0xB9B7B0B9, 5072031),
            std::pair(0xF6BCAFC6, 5072101),
            std::pair(0xF3B6D7B1, 5071081),
            std::pair(0xD3BCDDB1, 5071021),
            std::pair(0xECBAC0BF, 5073051),
            std::pair(0xB1B8D7B1, 5073061),
            std::pair(0xD5C7B9BA, 5074041),
            std::pair(0xB2C6A3BA, 5072051),
            std::pair(0xA7C1CEC0, 5071051),
            std::pair(0xC0BAE7C0, 5072071),
            std::pair(0xE8B0E2B1, 5072081),
            std::pair(0xFCC74FA3, 5071091),
            std::pair(0xD0BEEDB0, 5071041),
            std::pair(0xADBFBBB3, 5071071),
            std::pair(0xCCC0B9B7, 5071101),
            std::pair(0xBBC5E4C5, 5074071),
            std::pair(0xCCC0ADC8, 5073062),
            std::pair(0xF1BADEC4, 5073052),
        })
    {
    }

    auto ItemMixPropItemMapping::FindItemId(int32_t unk1, int32_t unk2) -> int32_t
    {
        const uint32_t value1 = static_cast<uint32_t>(unk1);

        if (value1 == 0xFCC7DFC1)
        {
            if (const uint32_t value2 = static_cast<uint32_t>(unk2);
                value2 == 0xC1CEC020)
            {
                return 5071061;
            }
            else if (value2 == 0xBCDDB120)
            {
                return 5071031;
            }

            return 0;
        }

        const auto iter = _instance._items.find(value1);

        return iter != _instance._items.end() ? iter->second : 0;
    }
}
