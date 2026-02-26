#include "Location.hpp"
#include "ConfigContext.hpp"

Location::Location(ConfigContext& ctxt)
	: _name(ctxt.getLocationName())
	, _root(ctxt.getRoot())
	, _alias(ctxt.getAlias())
	, _redirect(ctxt.getRedirect())
	, _redirect_code(ctxt.getRedirectCode())
	, _autoindex(ctxt.getAutoindex())
	, _cgi(ctxt.getCGI())
	, _cgi_timeout(ctxt.getCGITimeout())
	, _cgi_exec(ctxt.getCGIExec())
	, _virtual(ctxt.getVirtualLocation())
	, _max_body_size(ctxt.getMaxBodySize())
	, _keepalive_time(ctxt.getKeepalive_time())
	, _keepalive_timeout(ctxt.getKeepalive_timeout())
	, _error_pages(ctxt.getErrorPages())
	, _indexes(ctxt.getIndexes())
	, _allowed_methods(ctxt.getAllowedMethods())
{
}

Location::Location(const Location& src)
	: _name(src._name)
	, _root(src._root)
	, _alias(src._alias)
	, _redirect(src._redirect)
	, _redirect_code(src._redirect_code)
	, _autoindex(src._autoindex)
	, _cgi(src._cgi)
	, _cgi_timeout(src._cgi_timeout)
	, _cgi_exec(src._cgi_exec)
	, _virtual(src._virtual)
	, _max_body_size(src._max_body_size)
	, _keepalive_time(src._keepalive_time)
	, _keepalive_timeout(src._keepalive_timeout)
	, _error_pages(src._error_pages)
	, _indexes(src._indexes)
	, _allowed_methods(src._allowed_methods)
{
}

Location&	Location::operator= (const Location& rhs)
{
	if (this != &rhs) {
		_name				= rhs._name;
		_root				= rhs._root;
		_alias				= rhs._alias;
		_redirect			= rhs._redirect;
		_redirect_code		= rhs._redirect_code;
		_autoindex			= rhs._autoindex;
		_indexes			= rhs._indexes;
		_cgi				= rhs._cgi;
		_virtual			= rhs._virtual;
		_keepalive_time		= rhs._keepalive_time;
		_keepalive_timeout	= rhs._keepalive_timeout;
		_error_pages		= rhs._error_pages;
		_indexes			= rhs._indexes;
		_allowed_methods	= rhs._allowed_methods;
		_cgi_timeout		= rhs._cgi_timeout;
		_cgi_exec			= rhs._cgi_exec;
		_max_body_size 		= rhs._max_body_size;
	}
	return (*this);
}

Location::~Location	() {}
