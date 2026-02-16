#include "Connection.hpp"

#include <iostream>

void	Connection::sendResponse()
{
	const char *data;
	size_t		data_size = 0;

	data = response.getDataToSend(data_size);

	if (!data || !data_size)
	{
		if (response.isDone())
		{
			if (response.getHeader("Connection") == "close") // || !keep-alive -> HTTP/1.0
			{
				conn_closed = true;
				response.cleanResponse();
				return;
			}

			// full response sent --> stop watching EPOLLOUT
			struct epoll_event	ev;
			ev.events = EPOLLIN | EPOLLRDHUP; // keep listening for reads
			ev.data.fd = _fd;

			if (epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, _fd, &ev) < 0) {
				perror("epoll_ctl MOD");
				conn_closed = true;
			}
		}
		else
		{
			std::cerr << "[Error] No data to send but response not done" << std::endl;
			conn_closed = true;
		}
		return ;
	}

	ssize_t bytesSent = send(_fd, data, data_size, MSG_NOSIGNAL);

	if (bytesSent > 0)
		response.updateBytesSend(bytesSent);
	else if (bytesSent < 0)
		return;
};

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
	, _last_conn(0)
	, conn_closed(false)
{
}

Connection::Connection( const Connection& src )
	: _fd(src._fd)
	, _epoll_fd(src._epoll_fd)
	, _first_conn(src._first_conn)
	, _last_conn(src._last_conn)
	, conn_closed(src.conn_closed)
{
	(void) src;
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

