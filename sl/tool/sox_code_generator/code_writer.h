#pragma once
#include "sl/tool/sox_code_generator/brace_guard.h"

namespace sunlight
{
	class CodeWriter final
	{
	public:
		CodeWriter(std::ostringstream* oss, int64_t indent);
		CodeWriter(std::ostringstream* oss, int64_t indent, std::initializer_list<std::string> contents);

		void BreakLine();
		void WriteLine(const std::string& line);
		void WriteLine(int64_t addIndent, const std::string& line);

		template <typename... Args>
		void WriteLine(const std::string& line, Args&&... args);

		template <typename... Args>
		void WriteLine(int64_t addIndent, const std::string& line, Args&&... args);

		CodeWriter AddIndent(int64_t level = 1);
		BraceGuard MakeBraceGuard();
		BraceGuard MakeClassBraceGuard();
	
	private:
		std::ostringstream* _oss;
		int64_t _indent = 0;
	};

	template <typename ... Args>
	void CodeWriter::WriteLine(const std::string& line, Args&&... args)
	{
		WriteLine(fmt::vformat(line, fmt::make_format_args(args...)));
	}

	template <typename ... Args>
	void CodeWriter::WriteLine(int64_t addIndent, const std::string& line, Args&&... args)
	{
		WriteLine(addIndent, fmt::vformat(line, fmt::make_format_args(args...)));
	}
}
