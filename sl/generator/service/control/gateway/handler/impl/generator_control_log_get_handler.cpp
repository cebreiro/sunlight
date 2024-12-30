#include "generator_control_log_get_handler.h"

#include "sl/generator/service/control/generator_control_service.h"
#include "sl/generator/service/control/gateway/generator_control_api_gateway_connection.h"

namespace
{
    auto GetSystemClockTimePointFromInt64(int64_t value) -> std::chrono::system_clock::time_point
    {
        return std::chrono::system_clock::time_point{ std::chrono::system_clock::duration(value) };
    }
}

namespace sunlight
{
    GeneratorControlLogGetHandler::GeneratorControlLogGetHandler(const ServiceLocator& serviceLocator)
        : _serviceLocator(serviceLocator)
    {
    }

    auto GeneratorControlLogGetHandler::HandleRequest(GeneratorControlAPIGatewayConnection& connection,
        int32_t requestId, const api::LogGetRequest& request) -> Future<void>
    {
        std::vector<LogLevel> logLevels;

        for (int32_t logLevel : request.log_level() | std::views::filter(IsValidLogLevel))
        {
            logLevels.emplace_back(static_cast<LogLevel>(logLevel));
        }

        using optional_time_point_type = std::optional<std::chrono::system_clock::time_point>;

        const optional_time_point_type start =
            request.has_start_date_time() ? GetSystemClockTimePointFromInt64(request.start_date_time()) : optional_time_point_type();

        const optional_time_point_type last =
            request.has_last_date_time() ? GetSystemClockTimePointFromInt64(request.last_date_time()) : optional_time_point_type();

        std::vector<api::LogItem> logItems;

        co_await _serviceLocator.Get<GeneratorControlService>().GetLog(logItems, std::move(logLevels), start, last);

        api::Response response;
        response.set_request_id(requestId);

        // ensure to not null
        api::LogGetResponse& logGetResponse = *response.mutable_log_get();

        if (!logItems.empty())
        {
            auto iter = std::make_move_iterator(logItems.begin());
            auto end = std::make_move_iterator(logItems.end());

            logGetResponse.mutable_log_item_list()->Add(iter, end);
        }

        connection.Send(std::move(response));
    }
}
