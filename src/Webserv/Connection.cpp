#include "Connection.hpp"

#include <iostream>

#define BUFFER_SIZE	349

std::string	Connection::receive()
{
	int		bytes_read; 
	char	buf[BUFFER_SIZE];
	std::string	s;

	bytes_read = recv(_fd, &buf, BUFFER_SIZE, 0);
	if (!bytes_read)
	{
		conn_closed = true;
		return ("");
	}
	if (bytes_read < 0)
	{
		return ("");
	}

	s.append(buf, bytes_read);
	return (s);
}

void	Connection::sendResponse()
{
	const char *data;
	size_t		to_send = 0;

	data = request_handler._response.getDataToSend(to_send);

	if (to_send > MAX_CHUNK_SIZE)
		to_send = MAX_CHUNK_SIZE;

	ssize_t bytesSent = send(_fd, data, to_send, MSG_NOSIGNAL);

	if (bytesSent > 0)
		request_handler._response.updateBytesSend(bytesSent);
	else if (bytesSent < 0)
		return;
}

bool	Connection::hasTimedOut(void)
{
	std::time_t	now = std::time(NULL);

	if (std::difftime(now, _last_conn) > request_handler.getVirtualServer()->getKeepaliveTimeout())
		return (true);
	if (std::difftime(now, _first_conn) > request_handler.getVirtualServer()->getKeepaliveTime())
		return (true);
	return (false);
}


// Getters Setters
void	Connection::setEvent(struct epoll_event event)
{
	_event = event;
}

struct epoll_event	Connection::getEvent(void) const
{
	return (_event);
}

int	Connection::getFd(void) const
{
	return (_fd);
}

std::time_t Connection::getFirstConnTime() const
{
	return (_first_conn);
}

void		Connection::setLastConnTime(std::time_t time)
{
	_last_conn = time;
}

std::time_t Connection::getLastConnTime()	const
{
	return (_last_conn);
}

// Constructors and stuff
Connection::Connection(int fd, const int& epoll_fd, std::time_t time)
	: _fd(fd)
	, _epoll_fd(epoll_fd)
	, _first_conn(time)
	, _last_conn(time)
	, conn_closed(false)
	//, virtual_server()
	, request_handler()
	, child_pid(0)
	, cgi_timeout(0)
{
	cgi_fd[0] = -1;
	cgi_fd[1] = -1;
}

Connection::Connection( const Connection& src )
	: _fd(src._fd)
	, _epoll_fd(src._epoll_fd)
	, _first_conn(src._first_conn)
	, _last_conn(src._last_conn)
	, conn_closed(src.conn_closed)
	//, virtual_server(src.virtual_server)
	, child_pid(src.child_pid)
	, cgi_timeout(src.cgi_timeout)


{
	cgi_fd[0] = src.cgi_fd[0];
	cgi_fd[1] = src.cgi_fd[1];
}

Connection&	Connection::operator= ( const Connection& rhs )
{
	if (this != &rhs) {
		_fd = rhs._fd;
		_last_conn = rhs._last_conn;
		_last_conn = rhs._last_conn;
		conn_closed = rhs.conn_closed;
	}
	return (*this);
}

Connection::~Connection( void )
{
}

