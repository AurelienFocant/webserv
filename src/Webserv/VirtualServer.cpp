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
	, _cgi_timeout(60)
	, _max_body_size(1048576)
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
	, _cgi_timeout(ctxt.getCGITimeout())
	, _max_body_size(ctxt.getMaxBodySize())
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
		_cgi_timeout		= rhs._cgi_timeout;
		_max_body_size		= rhs._max_body_size;
	}
	return (*this);
}

VirtualServer::~VirtualServer( void )
{
}
