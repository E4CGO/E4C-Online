#include "Network.h"

#include <WS2tcpip.h>

// 初期化
bool Network::Initialize()
{
	// WinSock初期化
	WSADATA	wsaData;
	WSAStartup(
		MAKEWORD(2, 2), //WORD 型 WinSock のバージョン MAKEWORD マクロを使用する
		&wsaData //WSADATA 構造体へのポインタ（ここに初期化された情報が入る）
	);

	return true;
}

void Network::Finalize()
{
	WSACleanup();
}

// IPアドレス取得
bool Network::GetIpAddress(char* buffer, size_t size)
{
	//ホスト名を取得する
	char hostname[256];
	if (gethostname(hostname, sizeof(hostname)) != 0)
	{
		return false;
	}
	//ホスト名からIPアドレスを取得する
	struct addrinfo hints, * res;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET;
	if (getaddrinfo(hostname, nullptr, &hints, &res) != 0)
	{
		return false;
	}
	struct in_addr addr;
	addr.S_un = ((struct sockaddr_in*)(res->ai_addr))->sin_addr.S_un;

	switch (res->ai_family)
	{
	case AF_INET:
	case AF_INET6:
		addrToChar(res, buffer, size);
		break;
	default:
		return false;
	}
	return true;
}

void Network::addrToChar(struct addrinfo*& addr, char*& buffer, size_t size)
{
	switch (addr->ai_family)
	{
	case AF_INET:
		inet_ntop(addr->ai_family, &((struct sockaddr_in*)addr->ai_addr)->sin_addr, buffer, size);
		break;
	case AF_INET6:
		inet_ntop(addr->ai_family, &((struct sockaddr_in6*)addr->ai_addr)->sin6_addr, buffer, size);
		break;
	}
}