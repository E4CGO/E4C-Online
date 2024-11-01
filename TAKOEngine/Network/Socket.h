#pragma once

#include "TAKOEngine/Network/Network.h"
#include <vector>
#include "TAKOEngine/Runtime/tentacle_lib.h"

#pragma comment(lib, "ws2_32.lib")

class Socket
{
public:
	Socket() = default;
	virtual ~Socket() = default;

	bool IsConnecting() const { return sock != INVALID_SOCKET; }

	virtual bool Connect(const char* address, const char* port) = 0;

	virtual bool Disconnect();

	std::string IPtoStr(const struct sockaddr_in* addr);
protected:
	SOCKET sock = INVALID_SOCKET;
	struct addrinfo* addr = nullptr;
	fd_set fds{};
};
