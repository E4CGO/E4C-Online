#ifndef _WINSOCK_WRAPPER_H_
#define _WINSOCK_WRAPPER_H_

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <WS2tcpip.h>

#pragma comment( lib, "WS2_32" )

#endif