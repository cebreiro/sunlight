#pragma once
#include <cstdint>
#include <atomic>
#include <boost/system/system_error.hpp>
#include "shared/service/service_locator.h"

namespace sunlight
{
    namespace execution
    {
        class IExecutor;
        class AsioExecutor;
    }

    class AppInstance
    {
    public:
        AppInstance(const AppInstance& other) = delete;
        AppInstance(AppInstance&& other) noexcept = delete;
        AppInstance& operator=(const AppInstance& other) = delete;
        AppInstance& operator=(AppInstance&& other) noexcept = delete;

        AppInstance();
        virtual ~AppInstance();

        auto Run(std::span<char*> args) -> int32_t;
        void Shutdown();

        virtual auto GetName() const -> std::string_view = 0;

        template <std::derived_from<IService> T>
        auto FindService() -> T*;

        template <std::derived_from<IService> T>
        auto FindService() const -> const T*;

        bool IsRunning() const;

        auto GetServiceLocator() -> ServiceLocator&;
        auto GetServiceLocator() const -> const ServiceLocator&;

    private:
        virtual void OnStartUp(std::span<char*> args) = 0;
        virtual void OnShutdown() = 0;
        virtual void OnExit(std::vector<boost::system::error_code>&);

    private:
        std::atomic<bool> _running = false;
        ServiceLocator _serviceLocator;
        std::vector<boost::system::error_code> _errorCodes;
    };

    template <std::derived_from<IService> T>
    auto AppInstance::FindService() -> T*
    {
        return _serviceLocator.Find<T>();
    }

    template <std::derived_from<IService> T>
    auto AppInstance::FindService() const -> const T*
    {
        return _serviceLocator.Find<T>();
    }
}
