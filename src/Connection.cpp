#include "Connection.hpp"

#include <iostream>

Connection::Connection( void )
{
}

Connection::Connection( const Connection& src )
{
	(void) src;
}

Connection&	Connection::operator=( const Connection& rhs )
{
	if (this != &rhs) {
	}
	return (*this);
}

Connection::~Connection( void )
{
	std::cout << "Connection Object Destroyed" << std::endl;
}

bool	Connection::receiveRequest()
{
	char	buf[10];
	int		bytesRecvd;


	while ((bytesRecvd = recv(clientFd, buf, sizeof(buf), 0)) > 0) {
		request.append(buf, bytesRecvd);
	}

	if (bytesRecvd == 0 && epollEvent.events & EPOLLRDHUP) {
		connClosed = true;
		return false;
	}
	if (epollEvent.events & EPOLLERR || epollEvent.events & EPOLLHUP) {
		connClosed = true;
		return false;
	}

	connClosed = false;
	return (true);
};

bool	Connection::parseRequest()
{
	return (true);
};

void	Connection::sendResponse(int epollFd)
{
	ssize_t bytesSent = send(clientFd,
			response.c_str() + respOffset,
			response.length() - respOffset,
			MSG_NOSIGNAL);

	if (bytesSent > 0) {
		respOffset += bytesSent;

		if (respOffset >= (int)response.length()) {
			// full response sent --> stop watching EPOLLOUT
			struct epoll_event	ev;

			ev.events = EPOLLIN | EPOLLRDHUP; // keep listening for reads
			ev.data.fd = clientFd;
			if (epoll_ctl(epollFd, EPOLL_CTL_MOD, clientFd, &ev) < 0) {
				perror("epoll_ctl MOD");
				connClosed = true;
			}

			response.clear();
			respOffset = 0;
		}
	}
	else if (bytesSent < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			perror("send");
			connClosed = true;
		}
	}
};

std::string	Connection::build_response(void)
{
	std::string header =
		"HTTP/1.1 200 OK\r\n"
		"Connection: Keep-Alive\r\n"
		"Content-Type: text/html; charset=utf-8\r\n"
		"Content-Length: 185\r\n"
		"Cache-Control: s-maxage=300, public, max-age=0\r\n"
		"Content-Language: en-US\r\n"
		"Date: Thu, 06 Dec 2018 17:37:18 GMT\r\n"
		"ETag: \"2e77ad1dc6ab0b53a2996dfd4653c1c3\"\r\n"
		"Server: meinheld/0.6.1\r\n"
		"Strict-Transport-Security: max-age=63072000\r\n"
		"X-Content-Type-Options: nosniff\r\n"
		"X-Frame-Options: DENY\r\n"
		"X-XSS-Protection: 1; mode=block\r\n"
		"Vary: Accept-Encoding,Cookie\r\n"
		"Age: 7\r\n"
		"\r\n";

	std::string body =
		"<!doctype html>\r\n"
		"<html lang=\"en\""
		">\r\n"
		"<head>\r\n"
		"<meta charset=\"utf-8\""
		">\r\n"
		"<title>A basic webpage</title>\r\n"
		"</head>\r\n"
		"<body>\r\n"
		"<h1>Basic HTML webpage</h1>\r\n"
		"<p>Hello, world!</p>\r\n"
		"</body>\r\n"
		"</html>\r\n";
	return (header + body);
}
