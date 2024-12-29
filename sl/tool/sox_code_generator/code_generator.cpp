#include "code_generator.h"
#include "sox_name_util.h"
#include "code_context.h"
#include "sl/data/sox/sox_file.h"
#include "sl/tool/sox_code_generator/code_writer.h"

namespace sunlight
{
	CodeGenerator::CodeGenerator(const CodeContext& context)
		: _context(context)
	{
	}

	auto CodeGenerator::Generate(const SoxFile& file) const -> std::pair<std::string, std::string>
    {
		const SoxNameNotation notation = ParseFileName(file.name);

        const std::string dataClassName = ToPascal(notation, file.name);
        const std::string tableClassName = fmt::format("{}Table", dataClassName);

		std::string header = GenerateHeader(file, tableClassName, dataClassName);
		std::string cpp = GenerateCpp(file, tableClassName, dataClassName);

		return std::make_pair(std::move(header), std::move(cpp));
	}

	auto CodeGenerator::GenerateHeader(const SoxFile& file, const std::string& tableClassName, const std::string& dataClassName) const -> std::string
    {
		std::ostringstream oss;

		CodeWriter namespaceWriter(&oss, 0);
		namespaceWriter.WriteLine("#pragma once");
        namespaceWriter.WriteLine(CodeContext::DO_NOT_EDIT);
		namespaceWriter.BreakLine();
        namespaceWriter.WriteLine("#include \"sl/data/sox/sox_table_interface.h\"");
		namespaceWriter.BreakLine();

		namespaceWriter.WriteLine("namespace {}", _context.namespace_);
		{
			BraceGuard namespaceBraceGuard = namespaceWriter.MakeBraceGuard();

			{
                CodeWriter classWriter = namespaceWriter.AddIndent();
                CodeWriter bodyWriter = classWriter.AddIndent();

                classWriter.WriteLine("struct {}", dataClassName);
                {
                    BraceGuard classBraceGuard = classWriter.MakeClassBraceGuard();
                    
                    bodyWriter.WriteLine("explicit {}(StreamReader<std::vector<char>::const_iterator>& reader);", dataClassName);
                    bodyWriter.BreakLine();

                    for (const SoxColumn& column : file.columnNames)
                    {
                        bodyWriter.WriteLine("{} {} = {{}};", ToCodeType(column.type), ToMemberConvention(ParseColumnName(file.columnNames.at(1).name), column.name));
                    }
                }
                classWriter.BreakLine();
			}

            {
                CodeWriter classWriter = namespaceWriter.AddIndent();
                CodeWriter bodyWriter = classWriter.AddIndent();

                classWriter.WriteLine("class {0} final : public ISoxTable, public std::enable_shared_from_this<{0}>", tableClassName);
                {
                    BraceGuard classBraceGuard = classWriter.MakeClassBraceGuard();

                    classWriter.WriteLine("public:");
                    bodyWriter.WriteLine("void LoadFromFile(const std::filesystem::path& path) override;");
                    bodyWriter.BreakLine();
                    bodyWriter.WriteLine("auto Find(int32_t index) const -> const {}*;", dataClassName);
                    bodyWriter.WriteLine("auto Get() const -> const std::vector<{}>&;", dataClassName);
                    bodyWriter.BreakLine();
                    classWriter.WriteLine("private:");
                    bodyWriter.WriteLine("std::unordered_map<int32_t, {}*> _umap;", dataClassName);
                    bodyWriter.WriteLine("std::vector<{}> _vector;", dataClassName);
                    bodyWriter.BreakLine();
                }
            }
		}

		return oss.str();
	}

	auto CodeGenerator::GenerateCpp(const SoxFile& file, const std::string& tableClassName, const std::string& dataClassName) const -> std::string
    {
		SoxNameNotation notation = ParseColumnName(file.columnNames.at(1).name);

		std::ostringstream oss;

		CodeWriter namespaceWriter(&oss, 0);
		namespaceWriter.WriteLine("#include \"{}.h\"", _context.fileName);
        namespaceWriter.BreakLine();
		namespaceWriter.WriteLine("#include \"sl/data/sox/sox_file.h\"");
		namespaceWriter.BreakLine();
		namespaceWriter.WriteLine("namespace {}", _context.namespace_);
		{
			BraceGuard namespaceBraceGuard = namespaceWriter.MakeBraceGuard();

			CodeWriter methodWriter = namespaceWriter.AddIndent();
			CodeWriter bodyWriter = methodWriter.AddIndent();

            methodWriter.WriteLine("{}::{}(StreamReader<std::vector<char>::const_iterator>& reader)", dataClassName, dataClassName);
            {
                BraceGuard methodBodyBraceGuard = methodWriter.MakeBraceGuard();

                auto GetReaderString = [](const SoxColumn& data)
                    {
                        switch (data.type)
                        {
                        case SoxValueType::String:
                            return "reader.ReadString(reader.Read<uint16_t>())";
                        case SoxValueType::Int:
                            return "reader.Read<int32_t>()";
                        case SoxValueType::Float:
                            return "reader.Read<float>()";
                        default:
                            throw std::runtime_error(fmt::format("unknown sox bytes type: {}, column: {}",
                                ToString(data.type), data.name));
                        }
                    };

                for (const auto& column : file.columnNames)
                {
                    bodyWriter.WriteLine("{} = {};", ToMemberConvention(notation, column.name), GetReaderString(column));
                }
            }
            methodWriter.BreakLine();

			methodWriter.WriteLine("void {}::LoadFromFile(const std::filesystem::path& path)", tableClassName);
			{
				BraceGuard methodBodyBraceGuard = methodWriter.MakeBraceGuard();
				bodyWriter.WriteLine("SoxFile fileData = SoxFile::CreateFrom(path / \"{}.sox\");", file.name);
				bodyWriter.WriteLine("StreamReader<std::vector<char>::const_iterator> reader(fileData.gameData.begin(), fileData.gameData.end());");
				bodyWriter.BreakLine();
				bodyWriter.WriteLine("_vector.reserve(fileData.rowCount);");
				bodyWriter.WriteLine("for (int64_t i = 0; i < fileData.rowCount; ++i)");
				{
					BraceGuard loopGuard = bodyWriter.MakeBraceGuard();
					CodeWriter loopWriter = bodyWriter.AddIndent();

					loopWriter.WriteLine("_vector.emplace_back(reader);");
				}
				bodyWriter.BreakLine();
				bodyWriter.WriteLine("for (auto& data : _vector)");
				{
					BraceGuard loopGuard = bodyWriter.MakeBraceGuard();
					CodeWriter loopWriter = bodyWriter.AddIndent();

					loopWriter.WriteLine("_umap[data.index] = &data;");
				}
			}
			methodWriter.BreakLine();
			methodWriter.WriteLine("auto {1}::Find(int32_t index) const -> const {0}*", dataClassName, tableClassName);
			{
				BraceGuard methodBodyBraceGuard = methodWriter.MakeBraceGuard();
				bodyWriter.WriteLine("auto iter = _umap.find(index);");
				bodyWriter.BreakLine();
				bodyWriter.WriteLine("return iter != _umap.end() ? iter->second : nullptr;");
			}
			methodWriter.BreakLine();
			methodWriter.WriteLine("auto {1}::Get() const -> const std::vector<{0}>&", dataClassName, tableClassName);
			{
				BraceGuard methodBodyBraceGuard = methodWriter.MakeBraceGuard();
				bodyWriter.WriteLine("return _vector;");
			}
		}

		return oss.str();
	}

    auto CodeGenerator::ToCodeType(SoxValueType type) -> std::string
    {
        switch (type)
        {
        case SoxValueType::String:
            return "std::string";
        case SoxValueType::Int:
            return "int32_t";
        case SoxValueType::Float:
            return "float";
        case SoxValueType::None:
        default:
            assert(false);
            return "std::nullptr_t";
        }
    }
}
