#include "Encode.h"

#undef _WINSOCKAPI_
#define _WINSOCKAPI_
#include <windows.h>
#include <filesystem>
#include <codecvt>

//TODO
wchar_t* Encode::char_to_wchar(const char* text)
{
	setlocale(LC_ALL, "Japanese_Japan.932");
	const size_t newsize = strlen(text) + 1;
	size_t convertedChars = 0;
	wchar_t* wct = new wchar_t[newsize];
	wct = (wchar_t*)malloc(newsize * sizeof(wchar_t));
	mbstowcs_s(&convertedChars, wct, newsize, text, _TRUNCATE);
	return wct;
}

std::wstring Encode::string_to_wstring(const std::string& str)
{
	setlocale(LC_ALL, "Japanese_Japan.932");
	int size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

std::string Encode::utf8_to_string(const char8_t* text)
{
	std::filesystem::path path(text);
	return path.string();
}

std::string Encode::shift_jis_to_utf8(const std::string& shiftJISStr) {
	// Shift_JISからワイド文字列に変換
	int wideStrSize = MultiByteToWideChar(932, 0, shiftJISStr.c_str(), -1, nullptr, 0);
	std::wstring wideStr(wideStrSize, 0);
	MultiByteToWideChar(932, 0, shiftJISStr.c_str(), -1, &wideStr[0], wideStrSize);

	// ワイド文字列からUTF-8に変換
	int utf8StrSize = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string utf8Str(utf8StrSize, 0);
	WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, &utf8Str[0], utf8StrSize, nullptr, nullptr);

	return utf8Str;
}


std::string Encode::utf8_to_shift_jis(const std::string& utf8Str) {
	// UTF-8 -> UTF-16 (WideChar)
	int wideStrSize = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
	if (wideStrSize == 0) {
		return ""; // 変換失敗
	}

	std::vector<wchar_t> wideStr(wideStrSize);
	MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wideStr[0], wideStrSize);

	// UTF-16 -> Shift_JIS (MultiByte)
	UINT codePage = 932; // Shift_JIS のコードページ
	int shiftJisStrSize = WideCharToMultiByte(codePage, 0, &wideStr[0], -1, nullptr, 0, nullptr, nullptr);
	if (shiftJisStrSize == 0) {
		return ""; // 変換失敗
	}

	std::vector<char> shiftJisStr(shiftJisStrSize);
	WideCharToMultiByte(codePage, 0, &wideStr[0], -1, &shiftJisStr[0], shiftJisStrSize, nullptr, nullptr);

	return std::string(shiftJisStr.begin(), shiftJisStr.end() - 1); // 終端文字を除外
}
