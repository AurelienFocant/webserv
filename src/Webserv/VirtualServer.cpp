#include "VirtualServer.hpp"
#include "ConfigContext.hpp"

VirtualServer::VirtualServer(void)
	: _port()
	, _root()
	, _alias()
	, _server_name()
	, _redirect()
	, _redirect_code()
	, _autoindex()
	, _keepalive_time()
	, _keepalive_timeout()
	, _error_pages()
	, _indexes()
	, _allowed_methods()
	, _locations()
{
}

VirtualServer::VirtualServer(ConfigContext const& ctxt)
	: _port(ctxt.getPort())
	, _root(ctxt.getRoot())
	, _alias(ctxt.getAlias())
	, _server_name(ctxt.getServerName())
	, _redirect(ctxt.getRedirect())
	, _redirect_code(ctxt.getRedirectCode())
	, _autoindex(ctxt.getAutoindex())
	, _keepalive_time(ctxt.getKeepalive_time())
	, _keepalive_timeout(ctxt.getKeepalive_timeout())
	, _error_pages(ctxt.getErrorPages())
	, _indexes(ctxt.getIndexes())
	, _allowed_methods(ctxt.getAllowedMethods())
	, _locations(ctxt.getLocations())
{
}

VirtualServer::VirtualServer(const VirtualServer& src)
	: _port(src._port)
	, _root(src._root)
	, _alias(src._alias)
	, _server_name(src._server_name)
	, _redirect(src._redirect)
	, _redirect_code(src._redirect_code)
	, _autoindex(src._autoindex)
	, _keepalive_time(src._keepalive_time)
	, _keepalive_timeout	(src._keepalive_timeout)
	, _error_pages(src._error_pages)
	, _indexes(src._indexes)
	, _allowed_methods(src._allowed_methods)
	, _locations(src._locations)
{
}

VirtualServer&	VirtualServer::operator=( const VirtualServer& rhs )
{
	if (this != &rhs) {
		_port				= rhs._port;
		_root				= rhs._root;
		_alias				= rhs._alias;
		_server_name		= rhs._server_name;
		_redirect			= rhs._redirect;
		_redirect_code		= rhs._redirect_code;
		_autoindex			= rhs._autoindex;
		_keepalive_time		= rhs._keepalive_time;
		_keepalive_timeout	= rhs._keepalive_timeout;
		_error_pages		= rhs._error_pages;
		_indexes			= rhs._indexes;
		_allowed_methods	= rhs._allowed_methods;
		_locations			= rhs._locations;
	}
	return (*this);
}

VirtualServer::~VirtualServer( void )
{
}

const std::map<std::string, Location>& VirtualServer::getLocations() const
{
	return _locations;
}

void VirtualServer::setLocations(const std::map<std::string, Location> &locs)
{
	_locations = locs;
}

const Location& VirtualServer::getLocationAt(std::string key) const
{
	return _locations.at(key);
}

void VirtualServer::setLocationsAt(std::string key, Location & loc)
{
	// _locations[key] = loc;
	_locations.insert(std::make_pair(key, loc));
}

void	VirtualServer::setRoot(std::string root)
{
	_root = root;
}

std::string		VirtualServer::getRoot(void) const
{
	return (_root);
}

void	VirtualServer::setPort(unsigned int port)
{
	_port = port;
}

unsigned int	VirtualServer::getPort(void) const
{
	return (_port);
}

void			VirtualServer::setServName(std::string name)
{
	_server_name = name;
}

std::string		VirtualServer::getServName(void) const
{
	return (_server_name);
}

std::vector<std::string> const&	VirtualServer::getIndexes(void) const
{
	return _indexes;
}

void	VirtualServer::setIndexes(std::vector<std::string> const& src)
{
	_indexes.assign(src.begin(), src.end());
}

bool	VirtualServer::getAutoindex(void) const
{
	return (_autoindex);
}

void	VirtualServer::setAutoindex(bool b)
{
	_autoindex = b;
}
