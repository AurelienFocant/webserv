#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <stdlib.h>
#include <cstdio>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <iostream>
#include <string.h>
#include <poll.h>
#include <sys/epoll.h>

#include <map>

#include <fcntl.h>
#include <unistd.h>

#include <cerrno>

#include "Request.hpp"
#include "Response.hpp"
#include "VirtualServer.hpp"

class Request;
class Response;
class Webserv;

class Connection
{
	private:
		int			_fd;
		uint32_t	_event;
		const int	_epoll_fd;

	public:
		bool   		(Webserv::*handler)(Connection & conn);

		Request		request;
		Response	response;

		void	sendResponse();

		bool	connClosed;



		// struct sockaddr_in	clientAddr;
		// int					respOffset;
		// bool				connClosed;


		VirtualServer		virtual_server;

		Connection	();
		Connection	(int fd, const int& epoll_fd, bool (Webserv::*f)(Connection & conn));
		Connection	( const Connection& src );
		Connection&	operator= ( const Connection& rhs );
		~Connection	( void );

		// bool	receiveRequest();
		// void	sendResponse(int epollFd);
		//
		// std::string	build_response();

		void		setEvent(uint32_t event);
		uint32_t	getEvent(void) const;
		int			getFd(void) const;

	
};

#endif // CONNECTION_HPP
