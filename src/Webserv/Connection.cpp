#include "Connection.hpp"

#include <iostream>

Connection::Connection()
	: _fd(-1)
	, _epoll_fd(-1) // a surveiller avec obj map
	, _first_conn(0)
	, _last_conn(0)
	, handler(NULL)
	, connClosed(false)
{
}

Connection::Connection(int fd, const int& epoll_fd, bool (Webserv::*f)(Connection & conn))
	: _fd(fd)
	, _epoll_fd(epoll_fd)
	, _first_conn(std::time(NULL))
	, _last_conn(0)
	, handler(f)
	, connClosed(false)
{
}

Connection::Connection( const Connection& src )
	: _fd(src._fd)
	, _epoll_fd(src._epoll_fd)
	, _first_conn(src._first_conn)
	, _last_conn(src._last_conn)
	, handler(src.handler)
	, connClosed(src.connClosed)
{
	(void) src;
}

Connection&	Connection::operator= ( const Connection& rhs )
{
	if (this != &rhs) {
		_fd = rhs._fd;
		_last_conn = rhs._last_conn;
		handler = rhs.handler;
		connClosed = rhs.connClosed;
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

void	Connection::sendResponse()
{
	size_t data_size = 0; 
	const char *data = response.getDataToSend(data_size);

	if (!data || !data_size)
	{
		if (response.isDone())
		{
			if (response.getHeader("Connection") == "close") // || !keep-alive -> HTTP/1.0
			{
				connClosed = true;
				response.cleanResponse();
				return;
			}

			// full response sent --> stop watching EPOLLOUT
			struct epoll_event	ev;
			ev.events = EPOLLIN | EPOLLRDHUP; // keep listening for reads
			ev.data.fd = _fd;

			if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, _fd, &ev) < 0) {
				perror("epoll_ctl MOD");
				connClosed = true;
			}
		}
		else
		{
			std::cout << "[Error] No data to send but response not done" << std::endl;
			connClosed = true;
		}
		return ;
	}

	ssize_t bytesSent = send(_fd, data, data_size, MSG_NOSIGNAL);

	if (bytesSent > 0)
		response.updateBytesSend(bytesSent);
	else if (bytesSent < 0)
		return;
};

uint32_t	Connection::getEvent(void) const
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
