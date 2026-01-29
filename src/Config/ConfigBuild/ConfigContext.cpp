#include "ConfigContext.hpp"
#include "VirtualServer.hpp"
#include "ConfigBuilder.hpp"
#include <vector>

const int DEFAULTPORT = 80;
const std::string DEFAULTROOT = "html";
const std::string DEFAULTNAME = "";

ConfigContext::ConfigContext(ContextType t)
    : _type(t)
	, _port(DEFAULTPORT)
	, _root(DEFAULTROOT)
	, _serverName(DEFAULTNAME)
	, _locationName("")
	, _locations()
	, _autoindex(false)
	, _cgi(false)
{
	_indexes.push_back("index.html");
}

ConfigContext::ConfigContext(const ConfigContext &src)
    : _type(src._type)
	, _port(src._port)
	, _root(src._root)
	, _serverName(src._serverName)
	, _locationName(src._locationName)
	, _locations(src._locations)
	, _indexes(src._indexes)
	, _autoindex(src._autoindex)
	, _cgi(src._cgi)
{
}

ConfigContext& ConfigContext::operator=(const ConfigContext &rhs)
{
    if (this != &rhs) {
        _type = rhs._type;
        _port = rhs._port;
        _root = rhs._root;
        _serverName = rhs._serverName;
		_locationName = rhs._locationName;
        _locations = rhs._locations;
		_indexes = rhs._indexes;
		_autoindex = rhs._autoindex;
		_cgi = rhs._cgi;
    }
    return *this;
}

ConfigContext::~ConfigContext(void)
{
}



void ConfigContext::inheritFrom(const ConfigContext &parent)
{
	_port = parent._port;
	_root = parent._root;
	_serverName = parent._serverName;
	_indexes = parent._indexes;
	_autoindex = parent._autoindex;
}

bool ConfigContext::isCGI(std::string location_name)
{
	if (location_name[location_name.size()] != '$')
		return (false);

	_cgi = true;
	return (true);
}


// Getters Setters
ContextType ConfigContext::getType(void) const
{
	return _type;
}

int ConfigContext::getPort(void) const
{
	return _port;
}

std::string ConfigContext::getRoot(void) const
{
	return _root;
}

std::string ConfigContext::getServerName(void) const
{
	return _serverName;
}

void ConfigContext::setPort(int port)
{
	_port = port;
}

void ConfigContext::setRoot(const std::string &root)
{
	_root = root;
}

void ConfigContext::setServerName(const std::string &serverName)
{
	_serverName = serverName;
}

std::string ConfigContext::getLocationName(void) const
{
	return _locationName;
}

const std::map<std::string, Location> &ConfigContext::getLocations(void) const
{
	return _locations;
}

void ConfigContext::setLocationName(const std::string &name)
{
	_locationName = name;
}

void ConfigContext::addLocation(const std::string &name, const Location &loc)
{
	_locations.insert(std::make_pair(name, loc));
}

void ConfigContext::setIndexes(std::vector<std::string> const& src)
{
	_indexes.assign(src.begin(), src.end());
}

std::vector<std::string> const&	ConfigContext::getIndexes(void) const
{
	return (_indexes);
}

bool	ConfigContext::getAutoindex(void) const
{
	return (_autoindex);
}

void	ConfigContext::setAutoindex(bool b)
{
	_autoindex = b;
}

bool	ConfigContext::getCGI(void) const
{
	return (_cgi);
}
