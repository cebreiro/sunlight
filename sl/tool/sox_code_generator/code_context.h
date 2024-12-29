#pragma once

namespace sunlight
{
	struct CodeContext
	{
		std::string fileName;
		std::string namespace_;
		std::string tableSuffix;

		static inline const char* DO_NOT_EDIT =
			"/**********************************************************\n"
			"*        AUTO GENERATED FILE BY sox_code_generator        *\n"
			"*                  DO NOT EDIT THIS FILE                  *\n"
			"**********************************************************/";
	};
}
