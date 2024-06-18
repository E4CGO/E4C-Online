#include "Logger.h"

#undef _WINSOCKAPI_
#define _WINSOCKAPI_
#include <windows.h>
#include <stdio.h>

// ÉçÉOèoóÕ
void Logger::Print(const char* format, ...)
{
	char message[1024];
	va_list args;
	va_start(args, format);
	vsnprintf(message, sizeof(message), format, args);
	va_end(args);

	::OutputDebugStringA(message);
}