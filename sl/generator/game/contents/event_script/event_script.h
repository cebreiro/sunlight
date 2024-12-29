#pragma once

namespace sunlight
{
    struct EventScriptString
    {
        int32_t tableIndex = 0;
    };

    struct EventScriptStringWithInt
    {
        int32_t tableIndex = 0;
        int32_t value = 0;
    };

    struct EventScriptStringWithItem
    {
        int32_t tableIndex = 0;
        int32_t itemId = 0;
    };

    using event_script_item_type = std::variant<EventScriptString, EventScriptStringWithInt, EventScriptStringWithItem>;

    class EventScript
    {
    public:
        EventScript(const EventScript& other) = delete;
        EventScript(EventScript&& other) noexcept = default;
        EventScript& operator=(const EventScript& other) = delete;
        EventScript& operator=(EventScript&& other) noexcept = default;

    public:
        EventScript() = default;
        ~EventScript() = default;

        // func_1602
        void AddString(int32_t tableIndex);

        // func_1603
        void AddStringWithInt(int32_t tableIndex, int32_t value);
        void AddStringWithIntItem(int32_t tableIndex, int32_t itemId);

        auto GetItems() const -> const std::vector<event_script_item_type>&;

    private:
        std::vector<event_script_item_type> _items;
    };
}
