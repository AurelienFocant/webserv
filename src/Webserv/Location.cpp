#include "Location.hpp"
#include "ConfigContext.hpp"

#include <iostream>

Location::Location()
	: _name("")
	, _root("")
	, _autoindex(false)
	, _cgi_on(false)
{
	_indexes.push_back("index.html");
}

Location::Location(ConfigContext& ctxt)
	: _name(ctxt.getLocationName())
	, _root(ctxt.getRoot())
	, _autoindex(ctxt.getAutoindex())
	, _cgi_on(ctxt.getCGI())
	, _keepalive_time(ctxt.getKeepalive_time())
	, _keepalive_timeout(ctxt.getKeepalive_timeout())
{
	this->setIndexes(ctxt.getIndexes());
}

Location::Location(const Location& src)
	: _name(src._name)
	, _root(src._root)
	, _autoindex(src._autoindex)
	, _cgi_on(false)
{
	this->setIndexes(src.getIndexes());
}

Location&	Location::operator= (const Location& rhs)
{
	if (this != &rhs) {
		_name = rhs._name;
		_root = rhs._root;
		_autoindex = rhs._autoindex;
		_indexes = rhs._indexes;
		_cgi_on = rhs._cgi_on;
	}
	return (*this);
}

Location::~Location	() {}
