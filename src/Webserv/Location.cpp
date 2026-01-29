#include "Location.hpp"
#include "ConfigContext.hpp"

#include <iostream>

Location::Location()
	: _name("")
	, _root("")
	, _redirect("")
	, _redirect_code(0)
	, _autoindex(false)
	, _cgi_on(false)
{
	_indexes.push_back("index.html");
}

Location::Location(ConfigContext& ctxt)
	: _name(ctxt.getLocationName())
	, _root(ctxt.getRoot())
	, _redirect(ctxt.getRedirect())
	, _redirect_code(ctxt.getRedirectCode())
	, _autoindex(ctxt.getAutoindex())
	, _keepalive_time(ctxt.getKeepalive_time())
	, _keepalive_timeout(ctxt.getKeepalive_timeout())
	, _cgi_on(ctxt.getCGI())
{
	this->setIndexes(ctxt.getIndexes());
}

Location::Location(const Location& src)
	: _name(src._name)
	, _root(src._root)
	, _redirect(src._redirect)
	, _redirect_code(src._redirect_code)
	, _autoindex(src._autoindex)
	, _keepalive_time(src._keepalive_time)
	, _keepalive_timeout(src._keepalive_timeout)
	, _cgi_on(false)
{
	this->setIndexes(src.getIndexes());
}

Location&	Location::operator= (const Location& rhs)
{
	if (this != &rhs) {
		_name = rhs._name;
		_root = rhs._root;
		_redirect = rhs._redirect;
		_redirect_code = rhs._redirect_code;
		_autoindex = rhs._autoindex;
		_indexes = rhs._indexes;
		_cgi_on = rhs._cgi_on;
	}
	return (*this);
}

Location::~Location	() {}
