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
        explicit ZoneExecutionEnvironment(const ServiceLocator& serviceLocator,
            const std::source_location& sourceLocation = std::source_location::current());
        ~ZoneExecutionEnvironment();

    private:
        const ServiceLocator& _serviceLocator;

        static thread_local std::vector<std::source_location> _sourceLocations;
    };
}
