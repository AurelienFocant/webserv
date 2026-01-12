#include "Location.hpp"

#include <iostream>



/* ///////////LOCATION/////////////////// */

Location::Location()
	: _name("")
	, _root("")
	, _alias("")
	, _autoindex(false)
{
}

Location::Location(const Location& src)
	: _name(src._name)
	, _root(src._root)
	, _alias(src._alias)
	, _autoindex(src._autoindex)
{
}

Location&	Location::operator= (const Location& rhs)
{
	if (this != &rhs) {
		_name = rhs._name;
		_root = rhs._root;
		_alias = rhs._alias;
		_autoindex = rhs._autoindex;
	}
	return (*this);
}

Location::~Location	() {}
