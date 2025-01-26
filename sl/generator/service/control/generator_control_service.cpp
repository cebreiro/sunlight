#include "generator_control_service.h"

#include "shared/process/query_sys_info.h"
#include "shared/execution/executor/impl/asio_executor.h"
#include "sl/generator/service/control/gateway/generator_control_api_gateway_server.h"
#include "sl/generator/service/database/database_service.h"
#include "sl/generator/service/hash/safe_hash_service.h"
#include "sl/generator/service/log_memcache/log_mem_cache_query_option.h"
#include "sl/generator/service/log_memcache/log_mem_cache_query_result.h"
#include "sl/generator/service/log_memcache/log_mem_cache_service.h"
#include "sl/generator/service/world/world_service.h"

namespace sunlight
{
    GeneratorControlService::GeneratorControlService(ServiceLocator& serviceLocator, execution::AsioExecutor& executor)
        : _serviceLocator(serviceLocator)
        , _executor(executor)
        , _apiGatewayServer(std::make_shared<GeneratorControlAPIGatewayServer>(_serviceLocator, executor))
    {
    }

    void GeneratorControlService::StartGatewayServer(uint16_t port)
    {
        _apiGatewayServer->StartUp(port);
    }

    auto GeneratorControlService::Authenticate(std::string id, std::string password) -> Future<std::optional<int32_t>>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

        std::optional<db::dto::Account> result = co_await _serviceLocator.Get<DatabaseService>().FindAccount(id);
        if (!result.has_value())
        {
            co_return std::nullopt;
        }

        if (!co_await _serviceLocator.Get<SafeHashService>().Compare(result->password, password))
        {
            co_return std::nullopt;
        }

        co_return result->gmLevel;
    }

    auto GeneratorControlService::CreateAccount(std::string id, std::string password, int8_t gmLevel, std::string* optOutError) -> Future<bool>
    {
        [[maybe_unused]]
        const auto self = shared_from_this();

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
        [[maybe_unused]]
        const auto self = shared_from_this();

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

    auto GeneratorControlService::GetUserCount() -> Future<int32_t>
    {
        co_return co_await _serviceLocator.Get<WorldService>().GetUserCount();
    }

    auto GeneratorControlService::GetSystemResourceInfo(std::optional<int32_t>& outCpuLoadPercentage,
        std::optional<double>& outFreeMemoryGB) -> Future<void>
    {
        Future<std::optional<int32_t>> future1 = StartAsync(_executor, []() -> std::optional<int32_t>
            {
                return QueryCPULoadPercentage();
            });

        Future<std::optional<double>> future2 = StartAsync(_executor, []() -> std::optional<double>
            {
                return QueryFreePhysicalMemoryGB();
            });

        co_await WaitAll(_executor, future1, future2);

        outCpuLoadPercentage = future1.Get();
        outFreeMemoryGB = future2.Get();

        co_return;
    }

    auto GeneratorControlService::GetWorldInfo() -> Future<std::vector<api::WorldInfo>>
    {
        co_return co_await _serviceLocator.Get<WorldService>().GetWorldInfo();
    }

    auto GeneratorControlService::GetLog(std::vector<api::LogItem>& result,
        std::vector<LogLevel> logLevels,
        std::optional<std::chrono::system_clock::time_point> start,
        std::optional<std::chrono::system_clock::time_point> last) -> Future<void>
    {
        std::vector<LogMemCacheQueryResult> queryResultSet;

        for (LogLevel logLevel : logLevels)
        {
            queryResultSet.clear();

            LogMemCacheQueryOption option;
            option.logLevel = logLevel;
            option.dateLowerBound = start;
            option.dateUpperBound = last;

            co_await _serviceLocator.Get<LogMemCacheService>().GetLog(queryResultSet, option);

            for (LogMemCacheQueryResult& queryResult : queryResultSet)
            {
                api::LogItem& logItem = result.emplace_back();

                logItem.set_date_time(queryResult.dateTime.time_since_epoch().count());
                logItem.set_log_level(static_cast<int32_t>(queryResult.logLevel));
                logItem.set_message(std::move(queryResult.log));
            }
        }

        co_return;
    }

    auto GeneratorControlService::OpenZone(int32_t worldId, int32_t zoneId, uint16_t port, std::string* errorMessage) -> Future<bool>
    {
        try
        {
            _serviceLocator.Get<WorldService>().StartZone(worldId, zoneId, port);

            co_return true;
        }
        catch (const std::exception& e)
        {
            if (errorMessage)
            {
                *errorMessage = e.what();
            }
        }

        co_return false;
    }

    auto GeneratorControlService::CloseZone(int32_t worldId, int32_t zoneId, std::string* errorMessage) -> Future<bool>
    {
        try
        {
            _serviceLocator.Get<WorldService>().StopZone(worldId, zoneId);

            co_return true;
        }
        catch (const std::exception& e)
        {
            if (errorMessage)
            {
                *errorMessage = e.what();
            }
        }

        co_return false;
    }

    auto GeneratorControlService::GetName() const -> std::string_view
    {
        return "generator_control_service";
    }
}
