#include "Location.hpp"
#include "ConfigContext.hpp"

#include <iostream>

Location::Location()
	: _name("")
	, _root("")
	, _autoindex(false)
{
	_indexes.push_back("index.html");
}

Location::Location(ConfigContext& ctxt)
	: _name(ctxt.getLocationName())
	, _root(ctxt.getRoot())
	, _autoindex(ctxt.getAutoindex())
{
	this->setIndexes(ctxt.getIndexes());
}

Location::Location(const Location& src)
	: _name(src._name)
	, _root(src._root)
	, _autoindex(src._autoindex)
{
	this->setIndexes(src.getIndexes());
}

Location&	Location::operator= (const Location& rhs)
{
	if (this != &rhs) {
		_name = rhs._name;
		_root = rhs._root;
		_autoindex = rhs._autoindex;
	}
	return (*this);
}

Location::~Location	() {}
