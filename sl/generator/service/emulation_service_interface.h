#pragma once
#include <boost/mysql/error_code.hpp>

namespace sunlight
{
    class IEmulationService : public IService
    {
    public:
        virtual ~IEmulationService() = default;

        virtual auto Run() -> Future<void> = 0;
        virtual void Shutdown() = 0;
        virtual void Join(boost::system::error_code& ec) = 0;
    };
}
