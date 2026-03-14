#include "Connection.hpp"

#include <iostream>

#define BUFFER_SIZE	32000

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
	const char 		*data;
	size_t		to_send = 0;

	if (request_handler._response.getState() != Response::SENDING)
		return;

	data = request_handler._response.getDataToSend(to_send);

	if (data && request_handler._response._offset == 0) {
		std::stringstream	stream(data);
		char	buff[256];
		stream.getline(buff, 256);
		std::cerr << "[Status line]: " << buff << std::endl;
	}

	if (to_send > request_handler._response._offset)
		std::cout << "left to send: " << to_send - request_handler._response._offset << " / " << request_handler._response.getDataSize() << std::endl;

/* 	if (to_send > MAX_CHUNK_SIZE)
		to_send = MAX_CHUNK_SIZE; */

	ssize_t bytes_sent = send(_fd, data, to_send, MSG_NOSIGNAL);

	if (bytes_sent > 0)
	{
		request_handler._response.updateBytesSend(bytes_sent);

		if (request_handler._response.isDone()) {
			std::cout << "Final offset: " << request_handler._response._offset << std::endl;
			std::cerr << "[sendResponse] LAST SEND - Response complete!" << std::endl;
			//std::cerr << "///////////////////////////////////////////" << std::endl;
			std::cerr << "******************************************* END" << std::endl;
		}
	}
	else if (bytes_sent < 0)
		return;
}

void	Connection::sendCgiContent(int& bytes_sent)
{
	const std::string& content = request_handler.getRequest().getBody();
	if (content.size() <= cgi_stdin_offset)
		return ;
	bytes_sent = write(cgi_fd[1], content.c_str() + cgi_stdin_offset, content.size() - cgi_stdin_offset);
	if (bytes_sent > 0)
		cgi_stdin_offset += bytes_sent;
	return ;
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

bool	Connection::hasCgiTimedOut(void)
{
	const VirtualServer* server = request_handler.getVirtualServer();
	if (!server)
		return (false);
	std::time_t	now = std::time(NULL);
	if (std::difftime(now, cgi_timeout) > request_handler.getVirtualServer()->getCGITimeout())
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
	, request_handler()
	, child_pid(0)
	, cgi_stdin_offset(0)
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
	, child_pid(src.child_pid)
	, cgi_stdin_offset(src.cgi_stdin_offset)
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

