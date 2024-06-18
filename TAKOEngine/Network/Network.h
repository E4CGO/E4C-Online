#pragma once

class Network
{
public:
	Network() = default;
	~Network() = default;

	// 初期化
	static bool Initialize();

	// 終了化
	static void Finalize();

	// IPアドレス取得
	static bool GetIpAddress(char* buffer, size_t size);
	static void addrToChar(struct addrinfo*& addr, char*& buffer, size_t size);
};