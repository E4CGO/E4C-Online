#include "url.h"

std::string Url::host = "http://localhost/";
std::unordered_map<Url::PATHS, const char*> Url::paths = {
	{ Url::PATHS::VERSION,			"api/version" },
	{ Url::PATHS::USER,				"api/user" },
	{ Url::PATHS::LOGIN,			"api/login" },
	{ Url::PATHS::LOGIN_PAGE,		"login" },
	{ Url::PATHS::REGISTER_PAGE,	"register" },
	{ Url::PATHS::CHARGE_PAGE,		"charge"},
};

void Url::SetHost(const char* host)
{
	Url::host = host;
}
std::string Url::GetUrl(Url::PATHS path)
{
	std::string link = Url::host;
	link += Url::paths[path];
	return link;
}