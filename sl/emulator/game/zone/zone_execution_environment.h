#pragma once

namespace sunlight
{
    class Zone;
}

namespace sunlight
{
    class ZoneExecutionEnvironment
    {
    public:
        ZoneExecutionEnvironment() = delete;
        ZoneExecutionEnvironment(const ZoneExecutionEnvironment& other) = delete;
        ZoneExecutionEnvironment(ZoneExecutionEnvironment&& other) noexcept = delete;
        ZoneExecutionEnvironment& operator=(const ZoneExecutionEnvironment& other) = delete;
        ZoneExecutionEnvironment& operator=(ZoneExecutionEnvironment&& other) noexcept = delete;

    public:
        explicit ZoneExecutionEnvironment(const ServiceLocator& serviceLocator);
        ~ZoneExecutionEnvironment();

    private:
        const ServiceLocator& _serviceLocator;
    };
}
