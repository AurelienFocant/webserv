#include "Connection.hpp"

#include <iostream>

Connection::Connection()
	: fd(-1)
	, handler(NULL)
{
}

Connection::Connection(int fd, bool (Webserv::*f)(Connection & conn))
	: fd(fd)
	, handler(f)
{
}

Connection::Connection( const Connection& src )
	: fd(src.fd)
	, handler(src.handler)
{
	(void) src;
}

Connection&	Connection::operator= ( const Connection& rhs )
{
	if (this != &rhs) {
		fd = rhs.fd;
		handler = rhs.handler;
	}
	return (*this);
}

Connection::~Connection( void )
{
}
