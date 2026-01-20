#include "Connection.hpp"

#include <iostream>

Connection::Connection()
	: _fd(-1)
	, handler(NULL)
{
}

Connection::Connection(int fd, bool (Webserv::*f)(Connection & conn))
	: _fd(fd)
	, handler(f)
{
}

Connection::Connection( const Connection& src )
	: _fd(src._fd)
	, handler(src.handler)
{
	(void) src;
}

Connection&	Connection::operator= ( const Connection& rhs )
{
	if (this != &rhs) {
		_fd = rhs._fd;
		handler = rhs.handler;
	}
	return (*this);
}

Connection::~Connection( void )
{
}

void	Connection::setEvent(uint32_t event)
{
	_event = event;
}

uint32_t	Connection::getEvent(void) const
{
	return (_event);
}

int	Connection::getFd(void) const
{
	return (_fd);
}
