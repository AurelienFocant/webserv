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

#define MAX_EVENTS 1024

// We need to be able to check every connection independently
// We put those is a std::map, which is a <key, value> pair
// with the key being the clientFd of the connection
#include "Connection.hpp"

int	setUpServer()
{
	struct sockaddr_in server_addr;
	int serverSocket;
	int ret;

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(8080);

	//serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); // atomique, a la place d'utiliser fcntl avec O_NONBLOCK
	if (serverSocket < 0) {
		perror("ERROR! serverSocket: ");
		return (-1);
	}

	// Pouvoir retry sans erreur avant 60s (reutiliser 8080):
	int	enable = 1;
	if	(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
		perror("ERROR! setsockopt: ");
		return -1;
	}

	ret = bind(serverSocket, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if (ret < 0)
		perror("ERROR! bind: ");

	ret = listen(serverSocket, SOMAXCONN);
	if (ret < 0)
		perror("ERROR! listen: ");

	return (serverSocket);
}

Connection*	add_clientFD_to_epoll(int epollFd, int listenSocket)
{
	Connection	*res = new Connection;
	socklen_t	client_addr_len = sizeof(res->clientAddr);
	struct epoll_event	ev;

	res->clientFd = accept(listenSocket, (struct sockaddr *) &res->clientAddr, &client_addr_len);
	if (res->clientFd < 0) {
		perror("ERROR! accept: ");
		throw std::runtime_error("accept socket failed");
	}

	fcntl(res->clientFd, F_SETFL, O_NONBLOCK);

	ev.events = EPOLLIN | EPOLLRDHUP;
	ev.data.fd = res->clientFd;
	epoll_ctl(epollFd, EPOLL_CTL_ADD, res->clientFd, &ev);
	return (res);
}

void	main_loop(int epollFd, int listenSocket)
{
	struct epoll_event			ready_events[MAX_EVENTS];
	std::map<int, Connection*>	connections;

	while (1) {
		int efd_count = epoll_wait(epollFd, ready_events, MAX_EVENTS, 100);
		if (efd_count < 0)
			perror("ERROR! epoll_wait: ");

		for (int i = 0; i < efd_count; i++) {
			int	fd = ready_events[i].data.fd;

			if (fd == listenSocket) {
				Connection* newConnection = add_clientFD_to_epoll(epollFd, listenSocket);
				newConnection->epollEvent = ready_events[i];				// Reference epollEvent in its Connection
				connections[newConnection->clientFd] = newConnection;	// Add Connection to map<int, Connection>
			}
			else {
				// Find the connection that matches the fd of ready_event[i]
				std::map<int, Connection*>::iterator	it;	// declare iterator
				it = connections.find(fd);				// find the right key
				if (it != connections.end()) {			// check before dereference
					Connection* currConn = it->second;	// currConn is the value of <key, value>


					// If socket is ready for reading
					if (ready_events[i].events & EPOLLIN) {
						currConn->receiveRequest();

						// Close Connection if needed
						if (currConn->connClosed) {
							epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
							close(fd);
							connections.erase(it);
							continue ;
						}
					}


					// We'll need to do loads of stuff in here
					currConn->parseRequest();


					// If response is empty --> build it
					// and tell epoll we want it to tell us
					// when the socket is ready for writing
					if (currConn->response.empty()) {
						struct epoll_event	ev;
						ev.events = EPOLLOUT | EPOLLRDHUP;
						ev.data.fd = currConn->clientFd;
						epoll_ctl(epollFd, EPOLL_CTL_MOD, currConn->clientFd, &ev);
						currConn->response = currConn->build_response();

						currConn->sendResponse(epollFd);
					}


					// If the socket is ready for writing
					if (ready_events[i].events & EPOLLOUT) {
						currConn->sendResponse(epollFd);

						// Close Connection if needed
						if (currConn->connClosed) {
							epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
							close(fd);
							connections.erase(it);
							continue ;
						}
					}

					// Close connection if error
					if (ready_events[i].events & EPOLLERR || ready_events[i].events & EPOLLHUP) {
						epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
						close(fd);
						connections.erase(it);
					}
				}
			}
		}
	}
}

int	main()
{
	int	listenSocket;
	struct epoll_event	ev_hints;

	listenSocket = setUpServer();
	if (listenSocket < 0)
		return (1);

	int epollFd = epoll_create1(0);
	if (epollFd < 0) {
		perror("ERROR! epoll: ");
		return (1);
	}

	ev_hints.events = EPOLLIN;
	ev_hints.data.fd = listenSocket;
	epoll_ctl(epollFd, EPOLL_CTL_ADD, listenSocket, &ev_hints);

	try {
		main_loop(epollFd, listenSocket);
	}
	catch (std::exception &e) {
		std::cerr << "Exception happened: " << e.what() << std::endl;
	}

	return (0);
}
