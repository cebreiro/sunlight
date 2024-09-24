#include "npc_talk_box.h"

namespace sunlight
{
    NPCTalkBox::NPCTalkBox(int32_t width, int32_t height)
        : _width(width)
        , _height(height)
    {
    }

    void NPCTalkBox::AddString(int32_t tableIndex)
    {
        _talkBoxItems.emplace_back(NPCTalkBoxString{
            .tableIndex = tableIndex,
            });
    }

    void NPCTalkBox::AddStringWithInt(int32_t tableIndex, int32_t value)
    {
        _talkBoxItems.emplace_back(NPCTalkBoxStringWithInt{
            .tableIndex = tableIndex,
            .value = value,
            });
    }

    void NPCTalkBox::AddStringWithIntItem(int32_t tableIndex, int32_t itemId)
    {
        _talkBoxItems.emplace_back(NPCTalkBoxStringWithItem{
            .tableIndex = tableIndex,
            .itemId = itemId,
            });
    }

    void NPCTalkBox::AddMenu(int32_t tableIndex, int32_t mouseOver, int32_t index)
    {
        _talkBoxItems.emplace_back(NPCTalkBoxMenu{
            .tableIndexDefault = tableIndex,
            .tableIndexMouseOver = mouseOver,
            .index = index,
            });
    }

    auto NPCTalkBox::GetWidth() const -> int32_t
    {
        return _width;
    }

    auto NPCTalkBox::GetHeight() const -> int32_t
    {
        return _height;
    }

    auto NPCTalkBox::GetTalkBoxItems() const -> const std::vector<npc_talk_box_item_type>&
    {
        return _talkBoxItems;
    }
}
