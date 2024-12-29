#pragma once

namespace sunlight
{
	enum class SoxNameNotation
	{
		AllUpperUnderBar,
		AllLowerUnderBar,
		Pascal,
		Hungarian,
		ParsingFailed
	};

	auto ParseFileName(const std::string& fileName) -> SoxNameNotation;
	auto ToPascal(SoxNameNotation notation, const std::string& str) -> std::string;
	auto ToFileNameConvention(SoxNameNotation notation, const std::string& str) -> std::string;
	bool CheckHungarianTypeString(const std::string& str);
	auto ParseColumnName(const std::string& columnName) -> SoxNameNotation;
	auto ToMemberConvention(SoxNameNotation notation, const std::string& str) -> std::string;
}
