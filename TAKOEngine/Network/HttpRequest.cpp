#include "HttpRequest.h"

#include "TAKOEngine/Network/Network.h"

HttpRequest::HttpRequest(const char* url)
{
	SetUrl(url);

	std::string version = OpenSSL_version(0);
	char* test = version.data();
}
HttpRequest::~HttpRequest()
{
	if (thread != nullptr)
	{
		if (thread->joinable()) thread->join();
		delete thread;
		thread = nullptr;
	}

	this->Release();
}

void HttpRequest::Release()
{
	closesocket(sock);

	if (port == PORT::HTTPS)
	{
		OPENSSL_cleanup();
		if (ssl)
		{
			if (!SSL_in_init(ssl))
			{
				if (SSL_shutdown(ssl) != 1) {
					ERR_print_errors_fp(stderr);
				}
			}
			SSL_free(ssl);
		}
		if (ctx)
		{
			SSL_CTX_free(ctx);
			ERR_free_strings();
		}
	}
}

void HttpRequest::SetUrl(const char* url)
{
	this->url = url;
	if (this->url.starts_with("https://")) // https://
	{
		port = PORT::HTTPS;
	}
	else if (this->url.starts_with("http://")) // http://
	{
		port = PORT::HTTP;
	}

	std::string temp = this->url.substr(this->url.find_first_of(":") + 3);
	size_t firstSlash = temp.find_first_of("/");
	host = temp.substr(0, firstSlash);
	path = temp.substr(firstSlash);

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET;
	getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &addr);

	if (addr)
	{
		char ipAddress[64];
		char* buffer = ipAddress;
		Network::addrToChar(addr, buffer, sizeof(ipAddress));
		ip = ipAddress;
	}
	else
	{
		state = STATE::FAIL;
	}
}

void HttpRequest::Send()
{
	if (state == STATE::FAIL) return;
	// ソケットの生成
	sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (sock == INVALID_SOCKET)
	{
		state = STATE::FAIL;
		return;
	}

	state = STATE::SENDED;

	// 送信待ち
	thread = new std::thread(&HttpRequest::Request, this);
}
std::string HttpRequest::RequestHeader()
{
	// Request Data
	std::string request = "";
	// Header
	// METHOD
	switch (method)
	{
	case HttpRequest::GET: request += "GET "; break;
	case HttpRequest::POST: request += "POST "; break;
	case HttpRequest::PUT:break;
	case HttpRequest::DEL:break;
	case HttpRequest::PATCH:break;
	}
	// PATH
	request += path;
	// HTTP
	request += " HTTP/1.1\r\n";
	// Connection
	request += "Connection: ";
	switch (connection)
	{
	case HttpRequest::CLOSE:
		request += "close";
		break;
	case HttpRequest::KEEP_ALIVE:
		request += "keep-alive";
		break;
	}
	request += "\r\n";
	// Auth
	if (token.size() > 0)
	{
		request += "Authorization: Bearer ";
		request += token + "\r\n";
	}
	// Host
	request += "Host: ";
	request += host + "\r\n";

	// Accept
	request += "Accept: ";
	switch (acceptType)
	{
	case HttpRequest::JSON:
		request += "application/json";
		break;
	case HttpRequest::TEXT:
		request += "text/html; charset=UTF-8";
		break;
	}
	request += "\r\n";

	// Content-Type
	request += "Content-Type: ";
	switch (responseType)
	{
	case HttpRequest::JSON:
		request += "application/json";
		break;
	case HttpRequest::TEXT:
		request += "text/html; charset=UTF-8";
		break;
	}
	request += "\r\n";

	// Data
	if (!sendData.empty())
	{
		// Content-Length
		std::string j = sendData.dump();
		request += "Content-Length: ";
		request += std::to_string(j.size()) + "\r\n\r\n";
		// Datat
		request += j;
	}
	else
	{
		request += +"\r\n\r\n";
	}
	return request;
}

bool HttpRequest::SendRequest(const char* buffer, size_t size)
{
	// サーバーへ接続
	if (connect(sock, addr->ai_addr, static_cast<int>(addr->ai_addrlen)) == SOCKET_ERROR)
	{
		return false;
	}

	if (port == PORT::HTTPS)
	{
		// SSL
		// OpenSSLのエラー文言を読み込み
		//SSL_load_error_strings();

		// OpenSSLライブラリ初期化。
		//SSL_library_init();
		OPENSSL_init_ssl(OPENSSL_INIT_NO_LOAD_CRYPTO_STRINGS, nullptr);
		// SSL通信構造体のインスタンス
		// TLS通信でセキュア通信を設定(TLS_client_method()を使用する。)
		ctx = SSL_CTX_new(TLS_client_method());

		// SSL通信構造体からsslの生成を行う
		ssl = SSL_new(ctx);
		// SSLにソケットを関連付ける
		SSL_set_fd(ssl, static_cast<int>(sock));

		// SSL接続
		if (SSL_connect(ssl) != 1)
		{
			return false;
		}

		if (SSL_write(ssl, buffer, static_cast<int>(size)) < 1)
		{
			// 送信失敗
			return false;
		}
	}
	else
	{
		// 80
		if (send(sock, buffer, static_cast<int>(size), 0) < 0)
		{
			// 送信失敗
			return false;
		}
	}
	state = STATE::SENDED;
	return true;
}

std::string HttpRequest::ResponseData()
{
	if (port == PORT::HTTP)
	{
		std::string responseText = "";
		while (1)
		{
			char buf[256] = { "" };
			int recvSize;
			recvSize = recv(sock, buf, 255, 0);
			if (recvSize > 0)
			{
				responseText += std::string(buf);
			}
			else
			{
				if (responseText.size() > 0)
				{
					break;
				}
				state = STATE::FAIL;
			}
		}
		return responseText;
	}
	else if (port == PORT::HTTPS)
	{
		BYTE recvData[1];
		ZeroMemory(recvData, sizeof(recvData));
		BYTE responseData[65535];
		ZeroMemory(responseData, sizeof(responseData));

		int iResult = 0;
		int iApend = -1;
		do {
			iResult = SSL_read(ssl, recvData, sizeof(recvData));
			if (iResult == 0) {
				break;
			}
			iApend += iResult;
			memcpy_s(responseData + iApend, sizeof(responseData), recvData, iResult);
		} while (iResult > 0);

		OPENSSL_cleanup();

		// BYTEを文字に
		char str[(sizeof responseData) + 1];
		memcpy(str, responseData, sizeof responseData);
		str[sizeof responseData] = 0; // End Flag

		return str;
	}
	return "";
}

void HttpRequest::Request()
{
	std::string header = RequestHeader();
	if (!SendRequest(header.c_str(), header.size()))
	{
		// 送信エラー
		state = STATE::FAIL;
		//Release();
		return;
	}

	std::string responseText = ResponseData();

	// Response
	size_t position = 0; // 分断用
	position = responseText.find("HTTP/") + strlen("HTTP/1.1 ");
	response.status = std::stoi(responseText.substr(position, responseText.find(" ", position) - position));

	if (response.status != 200)
	{
		state = STATE::FAIL;
		//Release();
		return;
	}

	switch (responseType)
	{
	case HttpRequest::JSON:
	{
		std::string content = responseText.substr(responseText.find("\r\n\r\n") + strlen("\r\n\r\n"));
		response.data = nlohmann::json::parse(content);
		break;
	}
	case HttpRequest::TEXT:
		break;
	}

	//Release();
	state = STATE::RESPONSE;
}