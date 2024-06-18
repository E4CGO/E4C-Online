#include "UDPSocket.h"

#include "TAKOEngine/Network/Network.h"

bool UDPServerSocket::Connect(const char* address, const char* port)
{
	// アドレス情報設定
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	int iResult = getaddrinfo(address, port, &hints, &addr);
	if (iResult != 0)
	{
		return false;
	}
	// ソケット作成
	sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (sock == INVALID_SOCKET)
	{
		return false;
	}
	// ソケットの受け入れ情報を関連付け
	iResult = bind(sock, addr->ai_addr, static_cast<int>(addr->ai_addrlen));
	if (iResult == SOCKET_ERROR)
	{
		return false;
	}

	FD_ZERO(&fds);
	FD_SET(sock, &fds);

	return true;
}

// 送信
int UDPServerSocket::Send(int id, const void* data, size_t size)
{
	int iResult = sendto(sock, reinterpret_cast<const char*>(data), static_cast<int>(size), 0, reinterpret_cast<sockaddr*>(&clients[id]), sizeof(clients[id]));
	if (iResult == SOCKET_ERROR)
	{
		return -1;
	}
	return iResult;
}
int UDPServerSocket::SendAll(const void* data, size_t size)
{
	int iResult = 0;
	for (std::pair<const int, struct sockaddr_in>& client : clients)
	{
		if (Send(client.first, data, size) < 0)
		{
			iResult = -1;
		}
	}
	return iResult;
}

// 受信
int UDPServerSocket::Receive(void* buffer, size_t size, struct sockaddr_in& client_addr)
{
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	fd_set	fd_work;
	memcpy(&fd_work, &fds, sizeof(fd_set));
	// fdsに設定されたソケットが読み込み可能になるまで待ちます
	int n = select(0, &fd_work, NULL, NULL, &tv);
	// タイムアウトの場合にselectは0を返します
	if (n <= 0) return -1;

	// データ受信
	int addr_len = sizeof(struct sockaddr_in);
	int iResult = recvfrom(sock, reinterpret_cast<char*>(buffer), static_cast<int>(size), 0, reinterpret_cast<struct sockaddr*>(&client_addr), &addr_len);

	if (iResult == SOCKET_ERROR)
	{
		return -1;
	}

	return iResult;
}

void UDPServerSocket::Register(int id, struct sockaddr_in addr)
{
	if (clients.find(id) != clients.end()) return;

	clients[id] = addr;

	std::string log = "New UDP Connection: " + IPtoStr(&addr);
	T_CONSOLE.Log(log.c_str());
}

bool UDPClientSocket::Connect(const char* address, const char* port)
{
	// アドレス情報設定
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	int iResult = getaddrinfo(address, port, &hints, &addr);
	if (iResult != 0)
	{
		return false;
	}
	// ソケット作成
	sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (sock == INVALID_SOCKET)
	{
		return false;
	}

	FD_ZERO(&fds);
	FD_SET(sock, &fds);

	return true;
}

int UDPClientSocket::Send(const void* data, size_t size)
{
	int iResult = sendto(sock, reinterpret_cast<const char*>(data), static_cast<int>(size), 0, addr->ai_addr, static_cast<int>(addr->ai_addrlen));
	if (iResult == SOCKET_ERROR)
	{
		return -1;
	}
	return iResult;
}

// 受信
int UDPClientSocket::Receive(void* buffer, size_t size)
{
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	fd_set	fd_work;
	memcpy(&fd_work, &fds, sizeof(fd_set));
	// fdsに設定されたソケットが読み込み可能になるまで待ちます
	int n = select(0, &fd_work, NULL, NULL, &tv);
	// タイムアウトの場合にselectは0を返します
	if (n <= 0) return -1;

	struct sockaddr_in sv_addr;
	int addr_len = sizeof(struct sockaddr_in);
	int iResult = recvfrom(sock, reinterpret_cast<char*>(buffer), static_cast<int>(size), 0, reinterpret_cast<struct sockaddr*>(&sv_addr), &addr_len);
	//int iResult = recv(sock, reinterpret_cast<char*>(buffer), static_cast<int>(size), 0);
	if (iResult == SOCKET_ERROR)
	{
		return -1;
	}

	return iResult;
}