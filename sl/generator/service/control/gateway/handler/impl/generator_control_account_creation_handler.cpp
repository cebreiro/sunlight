#include "generator_control_account_creation_handler.h"

#include <boost/scope/scope_exit.hpp>

#include "sl/generator/api/generated/response.pb.h"
#include "sl/generator/service/control/gateway/generator_control_api_gateway_connection.h"
#include "sl/generator/service/database/database_service.h"
#include "sl/generator/service/hash/safe_hash_service.h"

namespace sunlight
{
    GeneratorControlAccountCreationHandler::GeneratorControlAccountCreationHandler(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    auto GeneratorControlAccountCreationHandler::HandleRequest(GeneratorControlAPIGatewayConnection& connection,
        int32_t requestId, const api::AccountCreationReuqest& request) -> Future<void>
    {
        if (connection.state != GeneratorControlAPIGatewayConnection::State::Authenticated)
        {
            throw std::runtime_error("unauthenticated request");
        }

        bool success = false;
        std::string error;

        boost::scope::scope_exit exit([requestId, &success, &error, &connection]()
            {
                api::Response response;
                response.set_request_id(requestId);

                api::AccountCreationResponse& creationResponse = *response.mutable_account_creation();
                creationResponse.set_success(success);
                creationResponse.set_error_message(error);

                connection.Send(std::move(response));
            });

        try
        {
            std::string encoded = co_await _serviceLocator.Get<SafeHashService>().Hash(request.password());

            std::optional<db::dto::Account> result = co_await _serviceLocator.Get<DatabaseService>().CreateAccount(
                request.id(), std::move(encoded), static_cast<int8_t>(request.gm_level()));

            success = result.has_value();
        }
        catch (const std::exception& e)
        {
            error = e.what();
        }

        co_return;
    }
}
