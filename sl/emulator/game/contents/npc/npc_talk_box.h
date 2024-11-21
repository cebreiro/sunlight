#pragma once

namespace sunlight
{
    struct NPCTalkBoxString
    {
        int32_t tableIndex = 0;
    };

    struct NPCTalkBoxStringWithInt
    {
        int32_t tableIndex = 0;
        int32_t value = 0;
    };

    struct NPCTalkBoxStringWithItem
    {
        int32_t tableIndex = 0;
        int32_t itemId = 0;
    };

    struct NPCTalkBoxMenu
    {
        int32_t tableIndexDefault = 0;
        int32_t tableIndexMouseOver = 0;
        int32_t index = 0;
    };

    using npc_talk_box_item_type = std::variant<NPCTalkBoxString, NPCTalkBoxStringWithInt, NPCTalkBoxStringWithItem, NPCTalkBoxMenu>;

    class NPCTalkBox
    {
    public:
        NPCTalkBox(const NPCTalkBox& other) = delete;
        NPCTalkBox(NPCTalkBox&& other) noexcept = default;
        NPCTalkBox& operator=(const NPCTalkBox& other) = delete;
        NPCTalkBox& operator=(NPCTalkBox&& other) noexcept = default;

    public:
        // func_400
        NPCTalkBox(int32_t width, int32_t height);

        // func_403
        void AddString(int32_t tableIndex);

        // func_405
        void AddStringWithInt(int32_t tableIndex, int32_t value);
        void AddStringWithIntItem(int32_t tableIndex, int32_t itemId);

        // func_404
        void AddMenu(int32_t tableIndexDefault, int32_t tableIndexMouseOver, int32_t index);

    public:
        auto GetWidth() const -> int32_t;
        auto GetHeight() const -> int32_t;
        auto GetTalkBoxItems() const -> const std::vector<npc_talk_box_item_type>&;

    private:
        int32_t _width = 0;
        int32_t _height = 0;

        std::vector<npc_talk_box_item_type> _talkBoxItems;
    };
}
