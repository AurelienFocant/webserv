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
#include <sys/types.h>
#include <sys/wait.h>

#include "Request.hpp"
#include "Response.hpp"
#include "RequestHandler.hpp"
#include "VirtualServer.hpp"

#define	MAX_CHUNK_SIZE 8192

class RequestHandler;
class Request;
class Response;
class Webserv;

class Connection
{
	private:
	//Copy constructor private for testing
		Connection	( const Connection& src );

		int					_fd;
		const int			_epoll_fd;
		struct epoll_event	_event;

		const	std::time_t	_first_conn;
				std::time_t	_last_conn;
		//if cgi

	public:
		bool			conn_closed;
		VirtualServer	virtual_server; // Remove and keep only the one in Request Handler? 
		RequestHandler	request_handler;

		bool	hasTimedOut(void);
		std::string	receive();
		void	sendResponse();
		pid_t		child_pid;
		int			cgi_fd[2];
		std::time_t	cgi_timeout;

		void		setEvent(struct epoll_event event);
		struct epoll_event	getEvent(void) const;
		int			getFd(void) const;
		const int&	getEpollFd(void) const {return (_epoll_fd);};

		std::time_t getFirstConnTime()	const;
		void		setLastConnTime(std::time_t time);
		std::time_t getLastConnTime()	const;

		Connection	(int fd, const int& epoll_fd, std::time_t start_time);
		Connection&	operator= ( const Connection& rhs );
		~Connection	( void );

};

#endif // CONNECTION_HPP
