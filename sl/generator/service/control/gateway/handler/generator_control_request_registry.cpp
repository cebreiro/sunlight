#include "generator_control_request_registry.h"

#include "sl/generator/service/control/gateway/handler/generator_control_reqeust_handler.hpp"
#include "sl/generator/service/control/gateway/handler/generator_control_reqeust_router.h"

namespace sunlight
{
    bool GeneratorControlRequestRegistry::Register(GeneratorControlRequestRouter& router, const ServiceLocator& serviceLocator)
    {
        bool result = true;

        result &= Add(router, std::make_shared<GeneratorControlAuthenticationHandler>(serviceLocator));
        result &= Add(router, std::make_shared<GeneratorControlAccountPasswordChangeHandler>(serviceLocator));
        result &= Add(router, std::make_shared<GeneratorControlAccountCreationHandler>(serviceLocator));
        result &= Add(router, std::make_shared<GeneratorControlSystemResourceInfoHandler>(serviceLocator));
        result &= Add(router, std::make_shared<GeneratorControlUserCountHandler>(serviceLocator));

        _handlers.shrink_to_fit();

        return result;
    }

    template <typename T>
    bool GeneratorControlRequestRegistry::Add(GeneratorControlRequestRouter& router, SharedPtrNotNull<T> handler)
    {
        static_assert(std::derived_from<T, IGeneratorControlRequestHandler>);

        [[maybe_unused]]
        const bool added = router.AddHandler(std::dynamic_pointer_cast<GeneratorControlRequestHandlerT<typename T::request_type>>(handler));
        assert(added);

        _handlers.emplace_back(std::move(handler));

        return added;
    }
}
