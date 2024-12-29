#include "sox_code_generator.h"

#include "shared/log/spdlog/spdlog_logger_builder.h"
#include "sl/data/sox/sox_file.h"
#include "sl/tool/sox_code_generator/code_context.h"
#include "sl/tool/sox_code_generator/code_generator.h"
#include "sl/tool/sox_code_generator/sox_name_util.h"

namespace sunlight
{
    SoxCodeGenerator::SoxCodeGenerator()
        : _logService(std::make_shared<LogService>())
    {
        SpdLogLoggerBuilder builder;
        builder.ConfigureConsole().SetLogLevel(LogLevel::Debug).SetAsync(false);

        _logService->Add(-1, builder.CreateLogger());
        GetServiceLocator().Add<ILogService>(_logService);
    }

    SoxCodeGenerator::~SoxCodeGenerator()
    {
    }

    auto SoxCodeGenerator::GetName() const -> std::string_view
    {
        return "sox_code_generator";
    }

    void SoxCodeGenerator::OnStartUp(std::span<char*> args)
    {
        if (std::ssize(args) < 3)
        {
            std::cout << fmt::format("use >> this_app.exe [data_path] [output_path]\n");

            this->Shutdown();

            return;
        }

        try
        {
            do
            {
                const std::filesystem::path inputPath = std::filesystem::path(args[1]);
                if (!is_directory(inputPath))
                {
                    SUNLIGHT_LOG_ERROR(GetServiceLocator(),
                        fmt::format("[{}] input path: {} is not directory",
                            GetName(), args[1]));

                    break;
                }

                const auto outputPath = std::filesystem::path(args[2]);

                create_directory(outputPath);

                if (!is_directory(outputPath))
                {
                    SUNLIGHT_LOG_ERROR(GetServiceLocator(),
                        fmt::format("[{}] output path: {} is not directory",
                            GetName(), args[2]));

                    break;
                }

                int64_t workCount = GenerateSoxCodeFile(inputPath, outputPath);

                SUNLIGHT_LOG_INFO(GetServiceLocator(),
                    fmt::format("[{}] {} files done",
                        GetName(), workCount));
                
            } while (false);
        }
        catch (const std::exception& e)
        {
            SUNLIGHT_LOG_ERROR(GetServiceLocator(),
                fmt::format("[{}] exception throws. e: {}",
                    GetName(), e.what()));
        }

        ::system("pause");

        this->Shutdown();
    }

    void SoxCodeGenerator::OnShutdown()
    {
    }

    auto SoxCodeGenerator::GenerateSoxCodeFile(const std::filesystem::path& inputPath, const std::filesystem::path& outputPath) const -> int64_t
    {
        std::vector<std::tuple<std::string, std::string, std::string>> datas;

        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(inputPath))
        {
            const auto& path = entry.path();
            if (!path.has_filename() || !path.has_extension())
            {
                continue;
            }

            const char* soxExtension = ".sox";
            if (::_strnicmp(path.extension().string().c_str(), soxExtension, ::strlen(soxExtension)))
            {
                continue;
            }

            SUNLIGHT_LOG_INFO(GetServiceLocator(),
                fmt::format("[{}] read: {}",
                    GetName(), path.string()));

            SoxFile fileData = SoxFile::CreateFrom(path);
            ModifyExceptionCase(fileData);

            CodeContext context;
            context.fileName = ToFileNameConvention(ParseFileName(fileData.name), fileData.name);
            context.namespace_ = "sunlight::sox";
            context.tableSuffix = "Table";

            CodeGenerator codeGenerator(context);
            std::pair<std::string, std::string> file = codeGenerator.Generate(fileData);

            datas.emplace_back(context.fileName, file.first, file.second);
        }

        for (const auto& [name, header, cpp] : datas)
        {
            auto writeFile = [](const std::filesystem::path& path, const std::string& name, const std::string& content)
                {
                    auto filePath = path / name;

                    std::ofstream ofs(filePath);
                    if (!ofs.is_open())
                    {
                        throw std::runtime_error(fmt::format("file: {} generate fail", filePath.string()));
                    }

                    ofs.write(content.c_str(), content.length());
                    ofs.close();
                };

            writeFile(outputPath, fmt::format("{}.h", name), header);
            writeFile(outputPath, fmt::format("{}.cpp", name), cpp);
        }

        return std::ssize(datas);
    }

    void SoxCodeGenerator::ModifyExceptionCase(SoxFile& file) const
    {
        for (size_t i = 0; i < file.columnNames.size(); ++i)
        {
            std::string& columnName = file.columnNames[i].name;
            if (std::ranges::any_of(columnName, [](char ch) { return ch == '*'; }))
            {
                std::string temp = columnName;
                std::ranges::transform(columnName, columnName.begin(), [](char ch)
                    {
                        if (ch == '*')
                        {
                            return 'x';
                        }

                        return ch;
                    });

                SUNLIGHT_LOG_WARN(GetServiceLocator(),
                    fmt::format("[{}] invalid column: [{}] is revised to: [{}]",
                        GetName(), temp, columnName));
            }

            if (std::ranges::any_of(columnName, [](char ch) { return std::isspace(ch); }))
            {
                std::string temp = columnName;
                std::ranges::transform(columnName, columnName.begin(), [](char ch)
                    {
                        if (std::isspace(ch))
                        {
                            return '_';
                        }

                        return ch;
                    });

                SUNLIGHT_LOG_WARN(GetServiceLocator(),
                    fmt::format("[{}] invalid column: [{}] is revised to: [{}]",
                        GetName(), temp, columnName));
            }

            if (!columnName.compare("_ATTACK4RESERVED4"))
            {
                if (i < file.columnNames.size() - 1)
                {
                    std::string& next = file.columnNames[i + 1].name;
                    if (!next.compare("_ATTACK4RESERVED4"))
                    {
                        std::pair temp = { columnName , next };
                        columnName = "_ATTACK4RESERVED2";
                        next = "_ATTACK4RESERVED3";

                        SUNLIGHT_LOG_WARN(GetServiceLocator(),
                            fmt::format("[{}] invalid column [{}, {}] is revised to: [{}, {}]",
                                GetName(), temp.first, temp.second, columnName, next));
                    }
                }
            }

            if (!columnName.compare("_ATTACK3RESERVED3"))
            {
                if (i < file.columnNames.size() - 1)
                {
                    std::string& next = file.columnNames[i + 1].name;
                    if (!next.compare("_ATTACK3RESERVED3"))
                    {
                        std::string temp = columnName;

                        std::string& prev = file.columnNames[i - 1].name;
                        if (!prev.compare("_ATTACK3RESERVED1"))
                        {
                            columnName = "_ATTACK3RESERVED2";
                        }
                        else
                        {
                            columnName = "_ATTACK4RESERVED2";
                        }

                        SUNLIGHT_LOG_WARN(GetServiceLocator(),
                            fmt::format("[{}] invalid column: [{}] is revised to: [{}]",
                                GetName(), temp, columnName));
                    }
                }
            }
        }
    }
}
