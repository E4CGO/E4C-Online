#include "Socket.h"

bool Socket::Disconnect()
{
	if (sock != INVALID_SOCKET)
	{
		closesocket(sock);
		sock = INVALID_SOCKET;
		addr = nullptr;
		return true;
	}
	return false;
}

std::string Socket::IPtoStr(const struct sockaddr_in* addr)
{
	char str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(addr->sin_addr), str, INET_ADDRSTRLEN);
	std::string ip = str;
	ip += ":";
	ip += std::to_string(addr->sin_port);
	return ip;
}