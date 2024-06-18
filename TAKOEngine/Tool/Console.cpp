#include "Console.h"

#include <iostream>
#include <stdio.h>

#undef _WINSOCKAPI_
#define _WINSOCKAPI_
#include <windows.h>

void Console::Open()
{
	if (isOpen) return;

	if (::AttachConsole(ATTACH_PARENT_PROCESS) == FALSE) {
		// エクスプローラから起動した場合は新規にコンソールを割り当てる
		AllocConsole();
	}
	FILE* fpOut = NULL;
	::freopen_s(&fpOut, "CONOUT$", "w", stdout);
	FILE* fpErr = NULL;
	::freopen_s(&fpErr, "CONOUT$", "w", stderr);
	FILE* fpIn = NULL;
	::freopen_s(&fpIn, "CONIN$", "r", stdin);

	isOpen = true;
}
void Console::Close()
{
	if (!isOpen) return;
	FreeConsole();
	isOpen = false;
}

void Console::Log(const char* text)
{
	if (!isOpen) return;
	std::cout << text << std::endl;
}