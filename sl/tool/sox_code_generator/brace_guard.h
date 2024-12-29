#pragma once

namespace sunlight
{
	class BraceGuard
	{
	public:
		BraceGuard(std::ostringstream* oss, int64_t indent, bool isClass);
		~BraceGuard();

	private:
		void Write(const std::string& content);

	private:
		std::ostringstream* _oss;
        int64_t _indent;
		bool _isClass;
	};
}
