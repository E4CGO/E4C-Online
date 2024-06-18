#pragma once

#include <unordered_map>

#include "TAKOEngine/Network/Socket.h"

/*
* サーバーソケット
* クライアント管理せず、全部受け入れる
*/

class UDPServerSocket : public Socket
{
public:
	UDPServerSocket() = default;
	~UDPServerSocket() override = default;

	bool Connect(const char* address, const char* port) override;

	int Send(int client_id, const void* data, size_t size); // 送信
	int Receive(void* buffer, size_t size, struct sockaddr_in& client_addr); // 受信
	int SendAll(const void* data, size_t size);
	void Remove(int client_id) { if (clients.find(client_id) != clients.end()) clients.erase(client_id); };

	std::unordered_map<int, struct sockaddr_in>& GetClients() { return clients; }
	void Register(int id, struct sockaddr_in addr);
private:
	std::unordered_map<int, struct sockaddr_in> clients;
};

class UDPClientSocket : public Socket
{
public:
	UDPClientSocket() = default;
	~UDPClientSocket() override = default;

	bool Connect(const char* address, const char* port) override;

	int Send(const void* data, size_t size); // 送信
	int Receive(void* buffer, size_t size); // 受信
};
