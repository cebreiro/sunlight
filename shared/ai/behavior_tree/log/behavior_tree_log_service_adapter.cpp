#include "behavior_tree_log_service_adapter.h"

#include <fmt/format.h>
#include "shared/log/log_service_interface.h"

namespace sunlight
{
    BehaviorTreeLogServiceAdapter::BehaviorTreeLogServiceAdapter(ILogService& logService, LogLevel logLevel)
        : _logService(logService)
        , _logLevel(logLevel)
    {
    }

    void BehaviorTreeLogServiceAdapter::LogBehaviorTreeStart(const std::string& btName)
    {
        _logService.Log(_logLevel, fmt::format("behavior_tree[{}] start", btName), std::source_location::current());
    }

    void BehaviorTreeLogServiceAdapter::LogBehaviorTreeEnd(const std::string& btName, bool success)
    {
        _logService.Log(_logLevel,
            fmt::format("behavior_tree[{}] end. result_state: {}",
                btName, success ? "success" : "failure"), std::source_location::current());
    }

    void BehaviorTreeLogServiceAdapter::LogBehaviorTreeFinalize(const std::string& btName)
    {
        _logService.Log(_logLevel,
            fmt::format("behavior_tree[{}] finalize.", btName), std::source_location::current());
    }

    void BehaviorTreeLogServiceAdapter::LogNodeStart(const std::string& btName, const std::string& nodeName)
    {
        _logService.Log(_logLevel, fmt::format("behavior_tree[{}] node[{}] start", btName, nodeName), std::source_location::current());
    }

    void BehaviorTreeLogServiceAdapter::LogNodeEnd(const std::string& btName, const std::string& nodeName, bool success)
    {
        _logService.Log(_logLevel,
            fmt::format("behavior_tree[{}] node[{}] end. result_state: {}",
                btName, nodeName, success ? "success" : "failure"), std::source_location::current());
    }

    void BehaviorTreeLogServiceAdapter::LogNodeSuspend(const std::string& btName, const std::string& nodeName)
    {
        _logService.Log(_logLevel,
            fmt::format("behavior_tree[{}] node[{}] suspend",
                btName, nodeName), std::source_location::current());
    }

    void BehaviorTreeLogServiceAdapter::LogNodeResumeEvent(const std::string& btName, const std::string& nodeName, const std::string& event)
    {
        _logService.Log(_logLevel,
            fmt::format("behavior_tree[{}] node[{}] resume event[{}]",
                btName, nodeName, event), std::source_location::current());
    }
}
