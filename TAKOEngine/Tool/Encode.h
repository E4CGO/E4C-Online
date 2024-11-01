//! @file Encode.h
//! @note 

#ifndef __INCLUDED_TOOL_ENCODE_H__
#define __INCLUDED_TOOL_ENCODE_H__
#include <string>


class Encode
{
public:
	Encode() {};
	~Encode() {};

	static wchar_t* char_to_wchar(const char* text);
	static std::wstring string_to_wstring(const std::string& str);
	static std::string utf8_to_string(const char8_t* text);
	static std::string shift_jis_to_utf8(const std::string& shiftJISStr);
	static std::string utf8_to_shift_jis(const std::string& utf8Str);
};
#endif
