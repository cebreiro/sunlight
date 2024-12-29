#include "code_writer.h"

namespace sunlight
{
	CodeWriter::CodeWriter(std::ostringstream* oss, int64_t indent)
		: _oss(oss)
		, _indent(indent)
	{
		assert(_oss != nullptr);
	}
	
	CodeWriter::CodeWriter(std::ostringstream* oss, int64_t indent,
		std::initializer_list<std::string> contents)
		: _oss(oss)
		, _indent(indent)
	{
		std::ranges::for_each(contents, [this](const std::string& content)
			{
				WriteLine(content);
			});
	}

	void CodeWriter::BreakLine()
	{
		(*_oss) << '\n';
	}

	void CodeWriter::WriteLine(const std::string& line)
	{
		WriteLine(0, line);
	}

	void CodeWriter::WriteLine(int64_t addIndent, const std::string& line)
	{
		for (int64_t i = 0; i < _indent + addIndent; ++i)
		{
			(*_oss) << "    ";
		}

		(*_oss) << line << '\n';
	}

	CodeWriter CodeWriter::AddIndent(int64_t level)
	{
		return CodeWriter(_oss, _indent + level);
	}

	BraceGuard CodeWriter::MakeBraceGuard()
	{
		return BraceGuard(_oss, _indent, false);
	}

	BraceGuard CodeWriter::MakeClassBraceGuard()
	{
		return BraceGuard(_oss, _indent, true);
	}
}
