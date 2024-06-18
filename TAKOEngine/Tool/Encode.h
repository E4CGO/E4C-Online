#pragma once
#include <string>

class Encode
{
public:
	Encode() {};
	~Encode() {};

	static wchar_t* char_to_wchar(const char* text);
	static std::wstring string_to_wstring(const std::string& str);
	static std::string utf8_to_string(const char8_t* text);
};