#pragma once
#include <atomic>
#include "shared/service/service_locator.h"

namespace sunlight
{
    class AppInstance;

    namespace execution
    {
        class IExecutor;
    }

    class App
    {
    public:
        App() = delete;

        static void SetInstance(AppInstance* instance);
        static void Shutdown();

        static auto GetServiceLocator() -> ServiceLocator&;

        template <std::derived_from<IService> T>
        static auto Find() -> T*;

    private:
        static auto LoadInstance() -> AppInstance*;

    private:
        static std::atomic<AppInstance*> _instance;
        static thread_local AppInstance* _localInstance;
    };

    template <std::derived_from<IService> T>
    auto App::Find() -> T*
    {
        return GetServiceLocator().Find<T>();
    }
}
