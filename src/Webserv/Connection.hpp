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
#include <ctime>

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

		const	std::time_t	_first_conn;
				std::time_t	_last_conn;

	public:
		bool   		(Webserv::*handler)(Connection & conn);

		Request		request;
		Response	response;

		void	sendResponse();

		bool	conn_closed;
		bool	hasTimedOut(void);



		VirtualServer		virtual_server;
		// bool	receiveRequest();
		// void	sendResponse(int epollFd);
		//
		// std::string	build_response();

		void		setEvent(uint32_t event);
		uint32_t	getEvent(void) const;
		int			getFd(void) const;

		std::time_t getFirstConnTime()	const;
		void		setLastConnTime(std::time_t time);
		std::time_t getLastConnTime()	const;

	

		Connection	(int fd, const int& epoll_fd, std::time_t start_time, bool (Webserv::*f)(Connection & conn));
		Connection	( const Connection& src );
		Connection&	operator= ( const Connection& rhs );
		~Connection	( void );

};

#endif // CONNECTION_HPP
