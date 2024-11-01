#include "app_intance.h"

#include <cassert>
#include <iostream>
#include <fmt/format.h>
#include "shared/app/app.h"

namespace sunlight
{
    AppInstance::AppInstance()
    {
        App::SetInstance(this);
    }

    AppInstance::~AppInstance()
    {
    }

    auto AppInstance::Run(std::span<char*> args) -> int32_t
    {
        _running.store(true);

        try
        {
            this->OnStartUp(args);
        }
        catch (const std::exception& e)
        {
            std::cout << fmt::format("fail to start application. exception: {}", e.what());

            return EXIT_FAILURE;
        }

        _running.wait(true);

        this->OnShutdown();
        this->OnExit(_errorCodes);

        App::SetInstance(nullptr);

        return _errorCodes.empty() ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    void AppInstance::Shutdown()
    {
        _running.store(false);
        _running.notify_one();
    }

    bool AppInstance::IsRunning() const
    {
        return _running.load();
    }

    auto AppInstance::GetServiceLocator() -> ServiceLocator&
    {
        return _serviceLocator;
    }

    auto AppInstance::GetServiceLocator() const -> const ServiceLocator&
    {
        return _serviceLocator;
    }

    void AppInstance::OnExit(std::vector<boost::system::error_code>&)
    {
    }
}
