#pragma once
#include "sl/data/sox/sox_value_type.h"

namespace sunlight
{
    class CodeWriter;
    struct CodeContext;

    struct SoxFile;
}

namespace sunlight
{
	class CodeGenerator
	{
	public:
		explicit CodeGenerator(const CodeContext& context);
		
		auto Generate(const SoxFile& fileData) const -> std::pair<std::string, std::string>;

	private:
		auto GenerateHeader(const SoxFile& file, const std::string& tableClassName,const std::string& dataClassName) const -> std::string;

		auto GenerateCpp(const SoxFile& fileData, const std::string& tableClassName, const std::string& dataClassName) const -> std::string;

	private:
        static auto ToCodeType(SoxValueType type) -> std::string;

	private:
		const CodeContext& _context;
	};
}
