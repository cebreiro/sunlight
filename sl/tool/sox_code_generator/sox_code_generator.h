#pragma once
#include "shared/app/app_intance.h"

namespace sunlight
{
    struct SoxFile;
}

namespace sunlight
{
    class SoxCodeGenerator final : public AppInstance
    {
    public:
        SoxCodeGenerator();
        ~SoxCodeGenerator();

        auto GetName() const -> std::string_view override;

    private:
        void OnStartUp(std::span<char*> args) override;
        void OnShutdown() override;

    private:
        auto GenerateSoxCodeFile(const std::filesystem::path& inputPath, const std::filesystem::path& outputPath) const -> int64_t;
        void ModifyExceptionCase(SoxFile& file) const;

    private:
        SharedPtrNotNull<LogService> _logService;
    };
}
