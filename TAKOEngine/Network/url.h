#pragma once

#define URL_HOST "http://localhost/"
#define URL_VERSION URL_HOST "api/version"
#define URL_USER URL_HOST "api/user"
#define URL_LOGIN URL_HOST "api/login"

#define URL_LOGIN_WEB URL_HOST L"login"
#define URL_REGISTER_WEB URL_HOST L"register"
#define URL_CHARGE_WEB URL_HOST L"charge"

#include <string>
#include <unordered_map>

class Url final
{
public:
	Url() = delete;
	~Url() = delete;
	static void SetHost(const char* host);
public:
	enum PATHS
	{
		VERSION,
		USER,
		LOGIN,
		LOGIN_PAGE,
		REGISTER_PAGE,
		CHARGE_PAGE,
	};
public:
	static std::string GetUrl(Url::PATHS path);
public:
	static std::unordered_map<PATHS, const char*> paths;
	static std::string host;
};

#define URL(path) (Url::GetUrl(path).c_str())