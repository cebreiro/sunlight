#pragma once

namespace sunlight
{
    struct ZoneMessage;

    class Stage;
}

namespace sunlight
{
    class ZoneMessageRouter
    {
    public:
        ZoneMessageRouter(const ZoneMessageRouter& other) = delete;
        ZoneMessageRouter(ZoneMessageRouter&& other) noexcept = delete;
        ZoneMessageRouter& operator=(const ZoneMessageRouter& other) = delete;
        ZoneMessageRouter& operator=(ZoneMessageRouter&& other) noexcept = delete;

    public:
        explicit ZoneMessageRouter(Stage& stage);
        ~ZoneMessageRouter();

        void Subscribe();

    private:
        void RouteSlV2Message(const ZoneMessage& message);

    private:
        Stage& _stage;
    };
}
