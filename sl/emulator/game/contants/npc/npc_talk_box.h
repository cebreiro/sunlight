#pragma once

namespace sunlight
{
    struct NPCTalkBoxContent
    {
        int32_t tableIndex = 0;
    };

    struct NPCTalkBoxAddContent
    {
        int32_t tableIndex = 0;
    };

    struct NPCTalkBoxContentWithInt
    {
        int32_t tableIndex = 0;
        int32_t value = 0;
    };

    struct NPCTalkBoxContentWithItem
    {
        int32_t tableIndex = 0;
        int32_t itemId = 0;
    };

    struct NPCTalkBoxString
    {
        int32_t tableIndex = 0;
    };

    struct NPCTalkBoxMenu
    {
        int32_t tableIndexDefault = 0;
        int32_t tableIndexMouseOver = 0;
        int32_t index = 0;
    };

    using npc_talk_box_item_type = std::variant<NPCTalkBoxContent, NPCTalkBoxContentWithInt, NPCTalkBoxContentWithItem, NPCTalkBoxString, NPCTalkBoxMenu>;

    class NPCTalkBox
    {
    public:
        NPCTalkBox(const NPCTalkBox& other) = delete;
        NPCTalkBox(NPCTalkBox&& other) noexcept = delete;
        NPCTalkBox& operator=(const NPCTalkBox& other) = delete;
        NPCTalkBox& operator=(NPCTalkBox&& other) noexcept = delete;

    public:
        NPCTalkBox(int32_t width, int32_t height);

        void SetContent(int32_t tableIndex);
        void SetContentWithInt(int32_t tableIndex, int32_t value);
        void SetContentWithIntItem(int32_t tableIndex, int32_t itemId);

        void AddString(int32_t tableIndex);
        void AddMenu(int32_t tableIndexDefault, int32_t tableIndexMouseOver, int32_t index);

    public:
        auto GetWidth() const -> int32_t;
        auto GetHeight() const -> int32_t;
        auto GetTalkBoxItems() const -> const std::vector<npc_talk_box_item_type>&;

    private:
        int32_t _width = 0;
        int32_t _height = 0;
        int32_t _content = 0;

        std::vector<npc_talk_box_item_type> _talkBoxItems;
    };
}
