#include "generator_control_service.h"

#include "sl/generator/service/control/gateway/generator_control_api_gateway_server.h"
#include "sl/generator/service/database/database_service.h"
#include "sl/generator/service/hash/safe_hash_service.h"

namespace sunlight
{
    GeneratorControlService::GeneratorControlService(ServiceLocator& serviceLocator, execution::AsioExecutor& executor)
        : _serviceLocator(serviceLocator)
        , _apiGatewayServer(std::make_shared<GeneratorControlAPIGatewayServer>(executor))
    {
    }

    void GeneratorControlService::Initialize()
    {
        _apiGatewayServer->Initialize(_serviceLocator);
    }

    void GeneratorControlService::StartGatewayServer(uint16_t port)
    {
        _apiGatewayServer->StartUp(port);
    }

    auto GeneratorControlService::Authenticate(std::string id, std::string password) -> Future<std::optional<int32_t>>
    {
        std::optional<db::dto::Account> result = co_await _serviceLocator.Get<DatabaseService>().FindAccount(id);
        if (!result.has_value())
        {
            co_return std::nullopt;
        }

        co_return 1;

        /*if (!co_await _serviceLocator.Get<SafeHashService>().Compare(result->password, password))
        {
            co_return std::nullopt;
        }

        co_return result->gmLevel;*/
    }

    auto GeneratorControlService::CreateAccount(std::string id, std::string password, int8_t gmLevel, std::string* optOutError) -> Future<bool>
    {
        bool success = false;

        try
        {
            std::string encoded = co_await _serviceLocator.Get<SafeHashService>().Hash(password);

            std::optional<db::dto::Account> result = co_await _serviceLocator.Get<DatabaseService>().CreateAccount(
                id, std::move(encoded), gmLevel);

            success = result.has_value();

            if (optOutError && !success)
            {
                *optOutError = "fail to add account to database";
            }
        }
        catch (const std::exception& e)
        {
            if (optOutError)
            {
                *optOutError = e.what();
            }
        }

        co_return success;
    }

    auto GeneratorControlService::ChangeAccountPassword(std::string id, std::string password, std::optional<int8_t> optRequesterLevel, std::string* optOutError) -> Future<bool>
    {
        bool success = false;

        try
        {
            DatabaseService& databaseService = _serviceLocator.Get<DatabaseService>();

            if (optRequesterLevel.has_value())
            {
                std::optional<db::dto::Account> dtoAccount = co_await databaseService.FindAccount(id);
                if (!dtoAccount.has_value())
                {
                    if (optOutError)
                    {
                        *optOutError = fmt::format("fail to find account from database. account: {}", id);
                    }

                    co_return false;
                }

                if (dtoAccount->gmLevel >= *optRequesterLevel)
                {
                    if (optOutError)
                    {
                        *optOutError = fmt::format("access denied. target accounts has a higher or equal level", id);
                    }

                    co_return false;
                }
            }

            std::string encoded = co_await _serviceLocator.Get<SafeHashService>().Hash(password);

            success = co_await databaseService.ChangeAccountPassword(id, std::move(encoded));

            if (optOutError && !success)
            {
                *optOutError = "fail to change password";
            }
        }
        catch (const std::exception& e)
        {
            if (optOutError)
            {
                *optOutError = e.what();
            }
        }

        co_return success;
    }

    auto GeneratorControlService::GetName() const -> std::string_view
    {
        return "generator_control_service";
    }
}
