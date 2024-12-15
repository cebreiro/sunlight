#pragma once

namespace sunlight
{
    struct ZoneMessage;

    class Stage;
}

namespace sunlight
{
    class ZoneMessageHooker
    {
    public:
        ZoneMessageHooker(const ZoneMessageHooker& other) = delete;
        ZoneMessageHooker(ZoneMessageHooker&& other) noexcept = delete;
        ZoneMessageHooker& operator=(const ZoneMessageHooker& other) = delete;
        ZoneMessageHooker& operator=(ZoneMessageHooker&& other) noexcept = delete;

    public:
        explicit ZoneMessageHooker(Stage& stage);
        ~ZoneMessageHooker();

        bool ProcessEvent(const ZoneMessage& message);

    private:
        Stage& _stage;
    };
}
