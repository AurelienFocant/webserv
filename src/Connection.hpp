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

class Request;

class Connection
{
	public:
		int					clientFd;
		struct sockaddr_in	clientAddr;
		std::string			request;
		std::string			response;
		int					respOffset;
		bool				connClosed;
		struct epoll_event	epollEvent;
		Request				request_message;



		Connection	( void );
		Connection	( const Connection& src );
		Connection&	operator= ( const Connection& rhs );
		~Connection	( void );

		bool	receiveRequest();
		void	sendResponse(int epollFd);

		std::string	build_response();

};

#endif // CONNECTION_HPP
