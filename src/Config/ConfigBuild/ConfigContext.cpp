#include "ConfigContext.hpp"
#include "VirtualServer.hpp"
#include <vector>

const int DEFAULTPORT = 80;
const std::string DEFAULTROOT = "html";
const std::string DEFAULTNAME = "";

ConfigContext::ConfigContext(ContextType t)
    : _type(t)
	, _port(DEFAULTPORT)
	, _root(DEFAULTROOT)
	, _alias("")
	, _serverName(DEFAULTNAME)
	, _locationName("")
	, _autoindex(false)
	, _keepalive_time(3600)
	, _keepalive_timeout(75)
	, _redirect_code(0)
	, _redirect("")
	, _cgi(false)
	, _cgi_timeout(60)
	, _virtualLocation(false)
	, _max_body_size(1048576)
	, _locations()
{
	_error_pages = _initDefaultErrorPages();

	_indexes.push_back("index.html");

	_allowed_methods.insert("GET");
}

ConfigContext::ConfigContext(const ConfigContext &src)
    : _type(src._type)
	, _port(src._port)
	, _root(src._root)
	, _alias(src._alias)
	, _serverName(src._serverName)
	, _locationName(src._locationName)
	, _autoindex(src._autoindex)
	, _keepalive_time(src._keepalive_time)
	, _keepalive_timeout(src._keepalive_timeout)
	, _redirect_code(src._redirect_code)
	, _redirect(src._redirect)
	, _cgi(src._cgi)
	, _cgi_timeout(src._cgi_timeout)
	, _virtualLocation(src._virtualLocation)
	, _max_body_size(src._max_body_size)
	, _error_pages(src._error_pages)
	, _indexes(src._indexes)
	, _allowed_methods(src._allowed_methods)
	, _locations(src._locations)
{
}

ConfigContext& ConfigContext::operator=(const ConfigContext &rhs)
{
    if (this != &rhs) {
        _type = rhs._type;
        _port = rhs._port;
        _root = rhs._root;
		_alias = rhs._alias;
        _serverName = rhs._serverName;
		_locationName = rhs._locationName;
		_autoindex = rhs._autoindex;
		_keepalive_time = rhs._keepalive_time;
		_keepalive_timeout = rhs._keepalive_timeout;
		_redirect_code = rhs._redirect_code;
		_redirect = rhs._redirect;
		_cgi = rhs._cgi;
		_cgi_timeout = rhs._cgi_timeout;
		_virtualLocation = rhs._virtualLocation;
		_error_pages = rhs._error_pages;
		_indexes = rhs._indexes;
		_allowed_methods = rhs._allowed_methods;
        _locations = rhs._locations;
		_max_body_size = rhs._max_body_size;
	}
	return *this;
}

ConfigContext::~ConfigContext(void)
{
}


// The Codes implemented here refer to HTTPenum.hpp
std::map<int, std::string>	ConfigContext::_initDefaultErrorPages(void)
{
	std::map<int, std::string> pages;

	pages[400] = "./data/webserv_default_error_pages/400.html";

	for (int i = 401; i <= 417; ++i) {
		pages[i] = "./data/webserv_default_error_pages/404.html";
	}

	for (int i = 500; i < 505; ++i) {
		pages[i] = "./data/webserv_default_error_pages/500.html";
	}

	return (pages);
}

void ConfigContext::inheritFrom(const ConfigContext &parent)
{
	_port = parent._port;
	_root = parent._root;
	_alias = parent._alias;
	_serverName = parent._serverName;
	_indexes = parent._indexes;
	_autoindex = parent._autoindex;
	_keepalive_time = parent._keepalive_time;
	_keepalive_timeout = parent._keepalive_timeout;
	_redirect_code = parent._redirect_code;
	_redirect = parent._redirect;
	_allowed_methods = parent._allowed_methods;
	_error_pages = parent._error_pages;
	_max_body_size = parent._max_body_size;
	_cgi_timeout = parent._cgi_timeout;
}
