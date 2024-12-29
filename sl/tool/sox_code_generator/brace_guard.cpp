#include "brace_guard.h"

namespace sunlight
{
	BraceGuard::BraceGuard(std::ostringstream* oss, int64_t indent, bool isClass)
		: _oss(oss)
		, _indent(indent)
		, _isClass(isClass)
	{
		Write("{\n");
	}

	BraceGuard::~BraceGuard()
	{
		std::string content = _isClass ? "};\n" : "}\n";

		Write(content);
	}

	void BraceGuard::Write(const std::string& content)
	{
		for (int64_t i = 0; i < _indent; ++i)
		{
			(*_oss) << "    ";
		}

		(*_oss) << content;
	}
}
