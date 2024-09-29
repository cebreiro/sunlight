#include "event_script.h"

namespace sunlight
{
    void EventScript::AddString(int32_t tableIndex)
    {
        _items.emplace_back(EventScriptString{
            .tableIndex = tableIndex,
            });
    }

    void EventScript::AddStringWithInt(int32_t tableIndex, int32_t value)
    {
        _items.emplace_back(EventScriptStringWithInt{
            .tableIndex = tableIndex,
            .value = value,
            });
    }

    void EventScript::AddStringWithIntItem(int32_t tableIndex, int32_t itemId)
    {
        _items.emplace_back(EventScriptStringWithItem{
            .tableIndex = tableIndex,
            .itemId = itemId,
            });
    }

    auto EventScript::GetItems() const -> const std::vector<event_script_item_type>&
    {
        return _items;
    }
}
