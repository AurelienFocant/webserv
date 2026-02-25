#include "Connection.hpp"

#include <iostream>

void	Connection::sendResponse()
{
	const char *data;
	size_t		data_size = 0;

	data = request_handler._response.getDataToSend(data_size);

	ssize_t bytesSent = send(_fd, data, data_size, MSG_NOSIGNAL);
	std::cout << request_handler.response << std::endl;

	if (bytesSent > 0)
		request_handler.response.updateBytesSend(bytesSent);
	else if (bytesSent < 0)
		return;
}

bool	Connection::hasTimedOut(void)
{
	std::time_t	now = std::time(NULL);

	if (std::difftime(now, _last_conn) > this->virtual_server.getKeepaliveTimeout())
		return (true);
	if (std::difftime(now, _first_conn) > this->virtual_server.getKeepaliveTime())
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
	, virtual_server()
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
	, virtual_server(src.virtual_server)
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

