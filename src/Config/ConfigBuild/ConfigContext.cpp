#include "VirtualServer.hpp"
#include "ConfigBuilder.hpp"
#include <vector>

ConfigContext::ConfigContext(void)
    : _type(MAIN)
	, _port(-1)
	, _root()
	, _serverName()
	, _locationName()
	, _locations()
	, _indexes()
	, _autoindex(false)
{
}

ConfigContext::ConfigContext(ContextType t)
    : _type(t)
	, _port(-1)
	, _root()
	, _serverName()
	, _locationName()
	, _locations()
	, _indexes()
	, _autoindex(false)
{
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
    }
    return *this;
}

ConfigContext::~ConfigContext(void)
{
}

void ConfigContext::inheritFrom(const ConfigContext &parent)
{
	if (parent._port != -1)
		_port = parent._port;
	if (!parent._root.empty())
		_root = parent._root;
	if (!parent._serverName.empty())
		_serverName = parent._serverName;
	if (!(parent._indexes.empty()))
		_indexes.assign(parent._indexes.begin(), parent._indexes.end());
	if (parent._autoindex)
		_autoindex = parent._autoindex;

}

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
	_locations[name] = loc;
}

void ConfigContext::setIndexes(std::vector<std::string> const& src)
{
	_indexes.assign(src.begin(), src.end());
}

std::vector<std::string>&	ConfigContext::getIndexes(void)
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
