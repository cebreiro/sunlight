#include "lua_npc_talk_box.h"

#include <sol/sol.hpp>

namespace sunlight
{
    LuaNPCTalkBox::LuaNPCTalkBox(int32_t width, int32_t height)
        : _width(width)
        , _height(height)
    {
    }

    void LuaNPCTalkBox::SetContent(int32_t tableIndex)
    {
        _talkBoxItems.emplace_back(NPCTalkBoxContent{
            .tableIndex = tableIndex,
            });
    }

    void LuaNPCTalkBox::SetContentWithInt(int32_t tableIndex, int32_t value)
    {
        _talkBoxItems.emplace_back(NPCTalkBoxContentWithInt{
            .tableIndex = tableIndex,
            .value = value,
            });
    }

    void LuaNPCTalkBox::SetContentWithIntItem(int32_t tableIndex, int32_t itemId)
    {
        _talkBoxItems.emplace_back(NPCTalkBoxContentWithItem{
            .tableIndex = tableIndex,
            .itemId = itemId,
            });
    }

    void LuaNPCTalkBox::AddString(int32_t tableIndex)
    {
        _talkBoxItems.emplace_back(NPCTalkBoxString{
            .tableIndex = tableIndex,
            });
    }

    void LuaNPCTalkBox::AddMenu(int32_t tableIndex, int32_t mouseOver, int32_t index)
    {
        _talkBoxItems.emplace_back(NPCTalkBoxMenu{
            .tableIndexDefault = tableIndex,
            .tableIndexMouseOver = mouseOver,
            .index = index,
            });
    }

    void LuaNPCTalkBox::Bind(sol::state& luaState)
    {
        luaState.new_usertype<LuaNPCTalkBox>("NPCTalkBox",
            sol::constructors<LuaNPCTalkBox(int32_t, int32_t)>(),
            "setContent", &LuaNPCTalkBox::SetContent,
            "setContentWithInt", &LuaNPCTalkBox::SetContentWithInt,
            "setContentWithItem", &LuaNPCTalkBox::SetContentWithIntItem,
            "addString", &LuaNPCTalkBox::AddString,
            "addMenu", &LuaNPCTalkBox::AddMenu
        );
    }

    auto LuaNPCTalkBox::GetWidth() const -> int32_t
    {
        return _width;
    }

    auto LuaNPCTalkBox::GetHeight() const -> int32_t
    {
        return _height;
    }

    auto LuaNPCTalkBox::GetTalkBoxItems() const -> const std::vector<talk_box_item_type>&
    {
        return _talkBoxItems;
    }

    auto LuaNPCTalkBox::GetContent() const -> int32_t
    {
        return _content;
    }
}
