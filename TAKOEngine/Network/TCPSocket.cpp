#include "TCPSocket.h"

bool TCPServerSocket::Connect(const char* address, const char* port)
{
	// アドレス情報設定
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
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
	// クライアントの接続待ち受け準備
	iResult = listen(sock, max_conn);
	if (iResult == SOCKET_ERROR)
	{
		return false;
	}

	// ノンブロッキング設定
	SetBlocking(false);

	FD_ZERO(&fds);
	FD_SET(sock, &fds);

	return true;
}

void TCPServerSocket::SetBlocking(bool block)
{
	u_long value = (block) ? 0 : 1;	// 0:ブロッキング / 1:ノンブロッキング
	ioctlsocket(sock, FIONBIO, &value);
}

// 接続許可
bool TCPServerSocket::Accept()
{
	Client client;
	int size = sizeof(sockaddr_in);
	client.client_id = static_cast<int>(clients.size());
	client.sock = accept(sock, reinterpret_cast<sockaddr*>(&client.addr), &size);
	if (client.sock == INVALID_SOCKET)
	{
		return false;
	}
	clients.emplace_back(client);

	std::string log = "New TCP Connection: " + IPtoStr(&client.addr);
	T_CONSOLE.Log(log.c_str());

	return true;
}

// 送信
int TCPServerSocket::Send(int id, const void* data, size_t size)
{
	int iResult = send(clients[id].sock, reinterpret_cast<const char*>(data), static_cast<int>(size), 0);
	if (iResult == SOCKET_ERROR)
	{
		return -1;
	}
	return iResult;
}
// 受信
int TCPServerSocket::Receive(int id, void* buffer, size_t size)
{
	int recvsize = recv(clients[id].sock, reinterpret_cast<char*>(buffer), static_cast<int>(size), 0);
	if (recvsize == SOCKET_ERROR)
	{
		return -1;
	}
	return recvsize;
}

bool TCPServerSocket::Disconnect()
{
	for (Client& client : clients)
	{
		closesocket(client.sock);
	}
	return Socket::Disconnect();
}

bool TCPClientSocket::Connect(const char* address, const char* port)
{
	// アドレス情報設定
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
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

	// タイムアウト設定(10000ms)
	int timeout = 10000;
	int result = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout));
	if (result == SOCKET_ERROR)
	{
		return false;
	}

	//	サーバー接続
	result = connect(sock, addr->ai_addr, static_cast<int>(addr->ai_addrlen));
	if (result == SOCKET_ERROR)
	{
		return false;
	}

	FD_ZERO(&fds);
	FD_SET(sock, &fds);

	return true;
}

int TCPClientSocket::Send(const void* data, size_t size) // 送信
{
	int iResult = send(sock, reinterpret_cast<const char*>(data), static_cast<int>(size), 0);
	if (iResult == SOCKET_ERROR)
	{
		return -1;
	}
	return iResult;
}

int TCPClientSocket::Receive(void* buffer, size_t size) // 受信
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

	int iResult = recv(sock, reinterpret_cast<char*>(buffer), static_cast<int>(size), 0);
	if (iResult == SOCKET_ERROR)
	{
		return -1;
	}

	return iResult;
}