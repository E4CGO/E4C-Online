#pragma once

#include <string>
#include <thread>
#include <functional>
#include <thread>
#include "TAKOEngine/Network/WinSock2Wrapper.h"
#include "External/tinygltf/json.hpp"
#include <openssl/crypto.h>	// opensslの暗号化ヘッダ
#include <openssl/ssl.h>	// opensslのssl通信ヘッダ
#include <openssl/err.h>	// opensslのエラーヘッダ

struct HttpResponse
{
	int status = -1;
	nlohmann::json data;
};

class HttpRequest
{
public:
	HttpRequest(const char* url);
	~HttpRequest();

	void Send();

	void SetUrl(const char* url);
	void SetPort(const int& port) { this->port = port; }
	void SetToken(const char* token) { this->token = token; }
	void SetMethod(int method) { this->method = method; }
	void SetData(const nlohmann::json& data) { this->sendData = data; }
	std::thread* GetThread() { return thread; }
	const HttpResponse GetResponse() const { return response; }
	bool IsDone() { return (state == STATE::RESPONSE || state == STATE::FAIL); }
	int GetState() { return state; }
private:
	void Release();

	std::string RequestHeader();
	std::string ResponseData();
	bool SendRequest(const char* buffer, size_t size);
	// Thread用
	void Request();
public:
	enum METHOD
	{
		GET,
		POST,
		// Laravel
		PUT,
		DEL,
		PATCH
	};
	enum DATA_TYPE
	{
		JSON,
		TEXT
	};
	enum STATE
	{
		DEFAULT = -1,
		SENDED,
		RESPONSE,
		FAIL
	};
	enum CONNECTION
	{
		CLOSE = 0,
		KEEP_ALIVE,
	};
	enum PORT
	{
		HTTP = 80,
		HTTPS = 443,
	};
private:
	SOCKET sock;

	int state = STATE::DEFAULT;
	std::string url;
	std::string path;
	std::string host;
	std::string ip;
	std::string token = "";	// Bearer Token
	struct addrinfo* addr;
	int port = HTTP;

	nlohmann::json sendData = {};
	HttpResponse response = {};

	int method = METHOD::GET;
	int acceptType = DATA_TYPE::JSON;
	int responseType = DATA_TYPE::JSON;
	int connection = CONNECTION::CLOSE;

	std::thread* thread = nullptr;
	std::function<void()> OnDone = nullptr; // 成功
	std::function<void()> OnFail = nullptr; // 失敗

	// SSL
	SSL* ssl; // ssl
	SSL_CTX* ctx; // SSL_CTX
};