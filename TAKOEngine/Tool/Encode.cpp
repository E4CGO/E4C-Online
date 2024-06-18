#include "Encode.h"

#include <locale>
#include <memory>
#undef _WINSOCKAPI_
#define _WINSOCKAPI_
#include <windows.h>
#include <filesystem>

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