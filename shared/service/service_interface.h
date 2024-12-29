#pragma once
#include <string_view>

namespace sunlight
{
    class ServiceLocator;

    class IService
    {
    public:
        IService(const IService& other) = delete;
        IService(IService&& other) noexcept = delete;
        IService& operator=(const IService& other) = delete;
        IService& operator=(IService&& other) noexcept = delete;

        IService() = default;
        virtual ~IService() = default;

        virtual auto GetName() const -> std::string_view = 0;
    };
}
