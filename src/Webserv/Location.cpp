#include "Location.hpp"
#include "ConfigContext.hpp"

#include <iostream>

Location::Location()
	: _name("")
	, _root("")
	, _redirect("")
	, _redirect_code(0)
	, _autoindex(false)
{
	_indexes.push_back("index.html");
}

Location::Location(ConfigContext& ctxt)
	: _name(ctxt.getLocationName())
	, _root(ctxt.getRoot())
	, _redirect("/test_redirect/")
	, _redirect_code(301)
	, _autoindex(ctxt.getAutoindex())
{
	this->setIndexes(ctxt.getIndexes());
}

Location::Location(const Location& src)
	: _name(src._name)
	, _root(src._root)
	, _redirect(src._redirect)
	, _redirect_code(src._redirect_code)
	, _autoindex(src._autoindex)
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
	}
	return (*this);
}

Location::~Location	() {}
