#pragma once
#include <string>

namespace sunlight
{
    class IBehaviorTreeLogger
    {
    public:
        virtual ~IBehaviorTreeLogger() = default;

        virtual void LogBehaviorTreeStart(const std::string& btName) = 0;
        virtual void LogBehaviorTreeEnd(const std::string& btName, bool success) = 0;
        virtual void LogBehaviorTreeFinalize(const std::string& btName) = 0;

        virtual void LogNodeStart(const std::string& btName, const std::string& nodeName) = 0;
        virtual void LogNodeEnd(const std::string& btName, const std::string& nodeName, bool success) = 0;
        virtual void LogNodeSuspend(const std::string& btName, const std::string& nodeName) = 0;
        virtual void LogNodeResumeEvent(const std::string& btName, const std::string& nodeName, const std::string& event) = 0;
    };
}
