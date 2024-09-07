#include "app.h"

#include "shared/app/app_intance.h"
#include "shared/service/service_locator.h"
#include "shared/execution/executor/static_thread_pool.h"

namespace sunlight
{
    std::atomic<AppInstance*> App::_instance = nullptr;
    thread_local AppInstance* App::_localInstance = nullptr;

    void App::SetInstance(AppInstance* instance)
    {
        _instance.store(instance);
    }

    void App::Shutdown()
    {
        if (AppInstance* instance = LoadInstance(); instance != nullptr)
        {
            instance->Shutdown();
        }
    }

    auto App::GetServiceLocator() -> ServiceLocator&
    {
        if (AppInstance* instance = LoadInstance(); instance != nullptr)
        {
            return instance->GetServiceLocator();
        }

        static ServiceLocator nullObject;
        return nullObject;
    }

    auto App::LoadInstance() -> AppInstance*
    {
        if (_localInstance == nullptr)
        {
            _localInstance = _instance.load();
        }

        return _localInstance;
    }
}
